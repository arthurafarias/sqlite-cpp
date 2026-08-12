#pragma once

#include "../detail/byte_range_lock.hpp"
#include "../device_characteristics.hpp"
#include "../file_concept.hpp"
#include "../lock_level.hpp"
#include "../result.hpp"
#include "win32_locker.hpp"

#include <cstdint>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Legacy source: os_win.c's winRead/winWrite/winTruncate/winSync/
// winFileSize/winClose/winSectorSize/winDeviceCharacteristics, plus
// winLock/winUnlock/winCheckReservedLock (delegated to
// detail::byte_range_lock_state_machine, shared with unix::unix_file). Uses
// ReadFile/WriteFile with an explicit OVERLAPPED offset (Windows' equivalent
// of pread/pwrite -- avoids a separate SetFilePointerEx seek + the shared
// file-position race that would otherwise exist between threads on one
// handle), rather than legacy os_win.c's seek-then-ReadFile/WriteFile pairs.
namespace sqlite::backend::os::windows {

class windows_file {
public:
    windows_file() = default;
    explicit windows_file(HANDLE handle)
        : handle_(handle), lock_state_(win32_locker(handle)) {}

    windows_file(const windows_file&) = delete;
    windows_file& operator=(const windows_file&) = delete;
    windows_file(windows_file&& other) noexcept
        : handle_(other.handle_), lock_state_(std::move(other.lock_state_)) {
        other.handle_ = INVALID_HANDLE_VALUE;
    }
    windows_file& operator=(windows_file&& other) noexcept {
        if (this != &other) {
            (void)close();
            handle_ = other.handle_;
            lock_state_ = std::move(other.lock_state_);
            other.handle_ = INVALID_HANDLE_VALUE;
        }
        return *this;
    }
    ~windows_file() { (void)close(); }

    [[nodiscard]] bool is_open() const noexcept { return handle_ != INVALID_HANDLE_VALUE && handle_ != nullptr; }
    [[nodiscard]] HANDLE native_handle() const noexcept { return handle_; }

    result close() {
        if (!is_open()) return result::ok;
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
        return result::ok;
    }

    // winRead(): a short read (including reading entirely past EOF, which
    // ReadFile reports as a successful zero-byte read rather than an error)
    // zero-fills the remainder and reports SQLITE_IOERR_SHORT_READ, mirroring
    // unix_file::read()'s and the legacy contract.
    result read(void* buf, int amt, std::int64_t offset) const {
        OVERLAPPED overlapped{};
        overlapped.Offset = static_cast<DWORD>(static_cast<std::uint64_t>(offset) & 0xffffffffu);
        overlapped.OffsetHigh = static_cast<DWORD>(static_cast<std::uint64_t>(offset) >> 32);
        DWORD got = 0;
        if (!ReadFile(handle_, buf, static_cast<DWORD>(amt), &got, &overlapped)) {
            if (GetLastError() == ERROR_HANDLE_EOF) got = 0;
            else return result::ioerr_read;
        }
        if (static_cast<int>(got) == amt) return result::ok;
        std::memset(static_cast<char*>(buf) + got, 0, static_cast<std::size_t>(amt) - got);
        return result::ioerr_short_read;
    }

    // winWrite(): retries on a partial write, same loop shape as
    // unix_file::write().
    result write(const void* buf, int amt, std::int64_t offset) {
        const auto* p = static_cast<const char*>(buf);
        int remaining = amt;
        while (remaining > 0) {
            OVERLAPPED overlapped{};
            overlapped.Offset = static_cast<DWORD>(static_cast<std::uint64_t>(offset) & 0xffffffffu);
            overlapped.OffsetHigh = static_cast<DWORD>(static_cast<std::uint64_t>(offset) >> 32);
            DWORD wrote = 0;
            if (!WriteFile(handle_, p, static_cast<DWORD>(remaining), &wrote, &overlapped)) {
                if (GetLastError() == ERROR_DISK_FULL) return result::full;
                return result::ioerr_write;
            }
            if (wrote == 0) return result::full;
            remaining -= static_cast<int>(wrote);
            offset += wrote;
            p += wrote;
        }
        return result::ok;
    }

    result truncate(std::int64_t size) {
        LARGE_INTEGER pos{};
        pos.QuadPart = size;
        if (!SetFilePointerEx(handle_, pos, nullptr, FILE_BEGIN)) return result::ioerr_truncate;
        if (!SetEndOfFile(handle_)) return result::ioerr_truncate;
        return result::ok;
    }

    // winSync(): deferred vs. legacy: the SQLITE_NO_SYNC test-instrumentation
    // path (this is the real, unconditional FlushFileBuffers call).
    result sync(sync_flags /*flags*/) const {
        if (!FlushFileBuffers(handle_)) return result::ioerr_fsync;
        return result::ok;
    }

    result file_size(std::int64_t& out) const {
        LARGE_INTEGER size{};
        if (!GetFileSizeEx(handle_, &size)) return result::ioerr_fstat;
        out = size.QuadPart;
        return result::ok;
    }

    result lock(lock_level requested) { return lock_state_.lock(requested); }
    result unlock(lock_level requested) { return lock_state_.unlock(requested); }
    result check_reserved_lock(bool& out) const { return lock_state_.check_reserved_lock(out); }

    [[nodiscard]] int sector_size() const noexcept { return 4096; }

    [[nodiscard]] device_characteristics characteristics() const noexcept {
        return device_characteristics::none;
    }

private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
    detail::byte_range_lock_state_machine<win32_locker> lock_state_{win32_locker(INVALID_HANDLE_VALUE)};
};

static_assert(file<windows_file>);

} // namespace sqlite::backend::os::windows
