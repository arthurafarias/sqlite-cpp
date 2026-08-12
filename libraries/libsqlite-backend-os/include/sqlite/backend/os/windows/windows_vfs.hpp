#pragma once

#include "../open_flags.hpp"
#include "../result.hpp"
#include "../vfs_concept.hpp"
#include "windows_file.hpp"

#include <sqlite/utils/random/chacha20_prng.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Legacy source: os_win.c's winOpen/winDelete/winAccess/winFullPathname/
// winSleep/winCurrentTimeInt64 (SRS S3.3.1). Same scope reductions as
// unix::unix_vfs (see that header's comment): no fd/handle reuse for a
// main-database file pending delete, no VFS-generated temp filenames, and
// (per SRS S3.2) randomness draws from sqlite::utils::random rather than a
// VFS-specific CryptGenRandom/BCryptGenRandom call.
namespace sqlite::backend::os::windows {

namespace detail_ {
inline std::wstring to_wide(std::string_view utf8) {
    if (utf8.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
    std::wstring result(static_cast<std::size_t>(len), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), result.data(), len);
    return result;
}
inline std::string to_utf8(const std::wstring& wide) {
    if (wide.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
    std::string result(static_cast<std::size_t>(len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), result.data(), len, nullptr, nullptr);
    return result;
}
} // namespace detail_

class windows_vfs {
public:
    using file_type = windows_file;

    result open(std::string_view path, open_flags flags, windows_file& out) {
        if (path.empty()) return result::cantopen;
        std::wstring wide_path = detail_::to_wide(path);

        bool readwrite = (flags & open_flags::readwrite) == open_flags::readwrite;
        DWORD access = readwrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;
        DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

        DWORD disposition;
        bool create = (flags & open_flags::create) == open_flags::create;
        bool exclusive = (flags & open_flags::exclusive) == open_flags::exclusive;
        if (exclusive && create) disposition = CREATE_NEW;
        else if (create) disposition = OPEN_ALWAYS;
        else disposition = OPEN_EXISTING;

        HANDLE h = CreateFileW(wide_path.c_str(), access, share, nullptr, disposition,
                                FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h == INVALID_HANDLE_VALUE) {
            DWORD err = GetLastError();
            if (err == ERROR_ACCESS_DENIED) return result::perm;
            return result::cantopen;
        }
        out = windows_file(h);
        return result::ok;
    }

    result delete_file(std::string_view path, bool /*sync_dir*/) const {
        std::wstring wide_path = detail_::to_wide(path);
        if (!DeleteFileW(wide_path.c_str())) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) return result::ok;
            return result::ioerr_delete;
        }
        return result::ok;
    }

    result access(std::string_view path, bool& exists) const {
        std::wstring wide_path = detail_::to_wide(path);
        DWORD attrs = GetFileAttributesW(wide_path.c_str());
        exists = (attrs != INVALID_FILE_ATTRIBUTES);
        return result::ok;
    }

    [[nodiscard]] std::string full_pathname(std::string_view path) const {
        std::wstring wide_path = detail_::to_wide(path);
        DWORD needed = GetFullPathNameW(wide_path.c_str(), 0, nullptr, nullptr);
        if (needed == 0) return std::string(path);
        std::wstring buf(needed, L'\0');
        DWORD written = GetFullPathNameW(wide_path.c_str(), needed, buf.data(), nullptr);
        if (written == 0) return std::string(path);
        buf.resize(written);
        return detail_::to_utf8(buf);
    }

    int randomness(int n, void* buf) const {
        random_.randomness(n, buf);
        return n;
    }

    int sleep(int microseconds) const {
        if (microseconds <= 0) return 0;
        Sleep(static_cast<DWORD>((microseconds + 999) / 1000));
        return microseconds;
    }

    [[nodiscard]] std::int64_t current_time_millis() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }

private:
    mutable sqlite::utils::random::chacha20_prng random_;
};

static_assert(vfs<windows_vfs>);

} // namespace sqlite::backend::os::windows
