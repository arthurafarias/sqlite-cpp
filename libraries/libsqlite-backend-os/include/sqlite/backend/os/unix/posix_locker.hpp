#pragma once

#include "../result.hpp"

#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>

// Legacy source: os_unix.c's unixFileLock() and sqliteErrorFromPosixError()
// (SRS S3.3.1). Implements detail::byte_range_locker over a raw fd using
// POSIX advisory locks (fcntl(F_SETLK)), the primitive
// detail::byte_range_lock_state_machine builds the shared SHARED/RESERVED/
// PENDING/EXCLUSIVE protocol on top of.
// GCC/Clang predefine `unix` as a macro expanding to 1 when compiling in GNU
// (rather than strict-ISO) mode, which is CMake's default C++ dialect
// (CMAKE_CXX_EXTENSIONS=ON). FR-1 requires this library's namespace to
// literally be `sqlite::backend::os::unix` (matching SRS S3.3.1's table), so
// every header that opens it must undefine the macro first -- otherwise
// `namespace unix` itself fails to parse (it expands to `namespace 1`).
#ifdef unix
#undef unix
#endif

namespace sqlite::backend::os::unix {

class posix_locker {
public:
    explicit posix_locker(int fd) noexcept : fd_(fd) {}

    result lock_range(std::int64_t start, std::int64_t len, bool exclusive) const {
        struct flock fl{};
        fl.l_type = static_cast<short>(exclusive ? F_WRLCK : F_RDLCK);
        fl.l_whence = SEEK_SET;
        fl.l_start = static_cast<off_t>(start);
        fl.l_len = static_cast<off_t>(len);
        if (::fcntl(fd_, F_SETLK, &fl) != 0) {
            return posix_error_to_result(errno);
        }
        return result::ok;
    }

    result unlock_range(std::int64_t start, std::int64_t len) const {
        struct flock fl{};
        fl.l_type = F_UNLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = static_cast<off_t>(start);
        fl.l_len = static_cast<off_t>(len);
        if (::fcntl(fd_, F_SETLK, &fl) != 0) {
            return posix_error_to_result(errno);
        }
        return result::ok;
    }

private:
    static result posix_error_to_result(int posix_errno) noexcept {
        switch (posix_errno) {
            case EACCES:
            case EAGAIN:
            case ETIMEDOUT:
            case EBUSY:
            case EINTR:
            case ENOLCK:
                return result::busy;
            case EPERM:
                return result::perm;
            default:
                return result::ioerr_lock;
        }
    }

    int fd_;
};

} // namespace sqlite::backend::os::unix_
