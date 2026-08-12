#pragma once

#include "../result.hpp"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Legacy source: os_win.c's winLockFile()/winUnlockFile() (the non-CE,
// non-legacy-LockFile path -- i.e. the LockFileEx/UnlockFileEx byte-range
// primitive; SRS S3.3.1). Implements detail::byte_range_locker over a Win32
// HANDLE, the Windows counterpart to unix::posix_locker, so
// detail::byte_range_lock_state_machine builds the same shared protocol on
// top of either.
namespace sqlite::backend::os::windows {

class win32_locker {
public:
    explicit win32_locker(HANDLE handle) noexcept : handle_(handle) {}

    result lock_range(std::int64_t start, std::int64_t len, bool exclusive) const {
        OVERLAPPED overlapped{};
        overlapped.Offset = static_cast<DWORD>(start & 0xffffffffu);
        overlapped.OffsetHigh = static_cast<DWORD>(static_cast<std::uint64_t>(start) >> 32);
        DWORD flags = LOCKFILE_FAIL_IMMEDIATELY | (exclusive ? LOCKFILE_EXCLUSIVE_LOCK : 0);
        DWORD len_low = static_cast<DWORD>(static_cast<std::uint64_t>(len) & 0xffffffffu);
        DWORD len_high = static_cast<DWORD>(static_cast<std::uint64_t>(len) >> 32);
        if (!LockFileEx(handle_, flags, 0, len_low, len_high, &overlapped)) {
            DWORD err = GetLastError();
            if (err == ERROR_LOCK_VIOLATION || err == ERROR_IO_PENDING) return result::busy;
            return result::ioerr_lock;
        }
        return result::ok;
    }

    result unlock_range(std::int64_t start, std::int64_t len) const {
        OVERLAPPED overlapped{};
        overlapped.Offset = static_cast<DWORD>(start & 0xffffffffu);
        overlapped.OffsetHigh = static_cast<DWORD>(static_cast<std::uint64_t>(start) >> 32);
        DWORD len_low = static_cast<DWORD>(static_cast<std::uint64_t>(len) & 0xffffffffu);
        DWORD len_high = static_cast<DWORD>(static_cast<std::uint64_t>(len) >> 32);
        if (!UnlockFileEx(handle_, 0, len_low, len_high, &overlapped)) {
            return result::ioerr_unlock;
        }
        return result::ok;
    }

private:
    HANDLE handle_;
};

} // namespace sqlite::backend::os::windows
