#pragma once

#include "../win32_locker.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace sqlite::backend::os::windows::testing {

using sqlite::utils::testing::test_group;

namespace {
HANDLE make_temp_handle(std::string& path_out) {
    wchar_t dir[MAX_PATH];
    GetTempPathW(MAX_PATH, dir);
    wchar_t name[MAX_PATH];
    GetTempFileNameW(dir, L"scl", 0, name);
    int len = WideCharToMultiByte(CP_UTF8, 0, name, -1, nullptr, 0, nullptr, nullptr);
    path_out.resize(static_cast<std::size_t>(len - 1));
    WideCharToMultiByte(CP_UTF8, 0, name, -1, path_out.data(), len, nullptr, nullptr);
    return CreateFileW(name, GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
}
} // namespace

inline const test_group win32_locker_tests("win32_locker", {
    {"lock then unlock the same range succeeds", [](auto& ctx) {
        std::string path;
        HANDLE h = make_temp_handle(path);
        ctx.check(h != INVALID_HANDLE_VALUE);

        win32_locker locker(h);
        ctx.check(locker.lock_range(0, 1, /*exclusive=*/true) == result::ok);
        ctx.check(locker.unlock_range(0, 1) == result::ok);

        CloseHandle(h);
        DeleteFileA(path.c_str());
    }},
    {"an exclusive lock from one handle blocks a conflicting lock from a second handle on the same file", [](auto& ctx) {
        std::string path;
        HANDLE h1 = make_temp_handle(path);
        ctx.check(h1 != INVALID_HANDLE_VALUE);
        HANDLE h2 = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        ctx.check(h2 != INVALID_HANDLE_VALUE);

        win32_locker a(h1);
        win32_locker b(h2);
        ctx.check(a.lock_range(500, 10, /*exclusive=*/true) == result::ok);
        // Unlike POSIX fcntl (process-scoped), Win32 byte-range locks are
        // scoped to the *handle*: a second, independently opened handle on
        // the same file conflicts here even within the same process.
        ctx.check(b.lock_range(500, 10, /*exclusive=*/true) == result::busy);
        ctx.check(a.unlock_range(500, 10) == result::ok);
        ctx.check(b.lock_range(500, 10, /*exclusive=*/true) == result::ok);
        ctx.check(b.unlock_range(500, 10) == result::ok);

        CloseHandle(h1);
        CloseHandle(h2);
        DeleteFileA(path.c_str());
    }},
});

} // namespace sqlite::backend::os::windows::testing
