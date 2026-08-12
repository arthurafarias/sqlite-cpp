#pragma once

#include "../detail/byte_range_lock.hpp"
#include "../device_characteristics.hpp"
#include "../file_concept.hpp"
#include "../lock_level.hpp"
#include "../result.hpp"
#include "posix_locker.hpp"

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

#ifdef unix
#undef unix
#endif

// Legacy source: os_unix.c's unixRead/unixWrite/unixTruncate/unixSync/
// unixFileSize/unixClose/unixSectorSize/unixDeviceCharacteristics, plus
// unixLock/unixUnlock/unixCheckReservedLock (delegated to
// detail::byte_range_lock_state_machine, see that header's note on the
// single-open-handle scope reduction this port makes). Uses pread(2)/
// pwrite(2) rather than the legacy seekAndRead/seekAndWrite lseek+read/write
// pairs, since both are POSIX-standard, avoid a shared file-position race
// between threads sharing an fd, and are what the legacy code itself prefers
// when USE_PREAD is defined.
namespace sqlite::backend::os::unix {

class unix_file {
public:
    unix_file() = default;
    explicit unix_file(int fd) : fd_(fd), lock_state_(posix_locker(fd)) {}

    unix_file(const unix_file&) = delete;
    unix_file& operator=(const unix_file&) = delete;
    unix_file(unix_file&& other) noexcept
        : fd_(other.fd_), lock_state_(std::move(other.lock_state_)) {
        other.fd_ = -1;
    }
    unix_file& operator=(unix_file&& other) noexcept {
        if (this != &other) {
            (void)close();
            fd_ = other.fd_;
            lock_state_ = std::move(other.lock_state_);
            other.fd_ = -1;
        }
        return *this;
    }
    ~unix_file() { (void)close(); }

    [[nodiscard]] bool is_open() const noexcept { return fd_ >= 0; }
    [[nodiscard]] int native_handle() const noexcept { return fd_; }

    result close() {
        if (fd_ < 0) return result::ok;
        int rc;
        do {
            rc = ::close(fd_);
        } while (rc != 0 && errno == EINTR);
        fd_ = -1;
        return result::ok;
    }

    // unixRead(): a short read zero-fills the remainder of the buffer and
    // reports SQLITE_IOERR_SHORT_READ (a distinct, recoverable code from a
    // hard I/O error -- e.g. it's what happens reading past EOF, which is
    // routine when probing a file's tail).
    result read(void* buf, int amt, std::int64_t offset) const {
        ::ssize_t got = ::pread(fd_, buf, static_cast<std::size_t>(amt), static_cast<off_t>(offset));
        if (got == amt) return result::ok;
        if (got < 0) return result::ioerr_read;
        std::memset(static_cast<char*>(buf) + got, 0, static_cast<std::size_t>(amt) - static_cast<std::size_t>(got));
        return result::ioerr_short_read;
    }

    // unixWrite()/seekAndWriteFd(): retries on a partial write (pwrite is
    // permitted to write fewer bytes than requested, e.g. if interrupted),
    // advancing the buffer/offset each time, until the full amount is
    // written or a write returns <=0.
    result write(const void* buf, int amt, std::int64_t offset) {
        const auto* p = static_cast<const char*>(buf);
        int remaining = amt;
        while (remaining > 0) {
            ::ssize_t wrote;
            do {
                wrote = ::pwrite(fd_, p, static_cast<std::size_t>(remaining), static_cast<off_t>(offset));
            } while (wrote < 0 && errno == EINTR);
            if (wrote <= 0) {
                if (wrote < 0 && errno != ENOSPC) return result::ioerr_write;
                return result::full;
            }
            remaining -= static_cast<int>(wrote);
            offset += wrote;
            p += wrote;
        }
        return result::ok;
    }

    result truncate(std::int64_t size) {
        if (::ftruncate(fd_, static_cast<off_t>(size)) != 0) return result::ioerr_truncate;
        return result::ok;
    }

    // unixSync(): fdatasync() is preferred over fsync() when available and
    // the caller only needs file *contents* durable (SQLITE_SYNC_DATAONLY),
    // since it can skip flushing inode metadata the file size hasn't
    // changed. Deferred: F_FULLFSYNC (Apple-only "really flush the disk
    // write cache" escape hatch) and SQLITE_NO_SYNC test instrumentation.
    result sync(sync_flags flags) const {
        int rc;
#if defined(_POSIX_SYNCHRONIZED_IO) && _POSIX_SYNCHRONIZED_IO > 0
        if ((flags & sync_flags::dataonly) == sync_flags::dataonly) {
            rc = ::fdatasync(fd_);
        } else {
            rc = ::fsync(fd_);
        }
#else
        rc = ::fsync(fd_);
#endif
        return rc == 0 ? result::ok : result::ioerr_fsync;
    }

    result file_size(std::int64_t& out) const {
        struct stat st{};
        if (::fstat(fd_, &st) != 0) return result::ioerr_fstat;
        out = static_cast<std::int64_t>(st.st_size);
        return result::ok;
    }

    result lock(lock_level requested) { return lock_state_.lock(requested); }
    result unlock(lock_level requested) { return lock_state_.unlock(requested); }
    result check_reserved_lock(bool& out) const { return lock_state_.check_reserved_lock(out); }

    [[nodiscard]] int sector_size() const noexcept { return 4096; }

    // unixDeviceCharacteristics(): the legacy default is 0 (no special
    // guarantees assumed) unless a specific filesystem is detected (F2FS/
    // btrfs atomic-write support, etc, an SQLITE_ENABLE_* opt-in this phase
    // does not port). Conservative-but-correct default.
    [[nodiscard]] device_characteristics characteristics() const noexcept {
        return device_characteristics::none;
    }

private:
    int fd_ = -1;
    detail::byte_range_lock_state_machine<posix_locker> lock_state_{posix_locker(-1)};
};

static_assert(file<unix_file>);

} // namespace sqlite::backend::os::unix
