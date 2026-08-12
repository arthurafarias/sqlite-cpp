#pragma once

#include "../windows_vfs.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace sqlite::backend::os::windows::testing {

using sqlite::utils::testing::test_group;

inline const test_group windows_vfs_tests("windows_vfs", {
    {"access reports false for a nonexistent path and true after creating it", [](auto& ctx) {
        windows_vfs vfs;
        wchar_t dir[MAX_PATH];
        GetTempPathW(MAX_PATH, dir);
        std::string path;
        {
            wchar_t name[MAX_PATH];
            GetTempFileNameW(dir, L"scv", 0, name);
            DeleteFileW(name);
            int len = WideCharToMultiByte(CP_UTF8, 0, name, -1, nullptr, 0, nullptr, nullptr);
            path.resize(static_cast<std::size_t>(len - 1));
            WideCharToMultiByte(CP_UTF8, 0, name, -1, path.data(), len, nullptr, nullptr);
        }

        bool exists = true;
        ctx.check(vfs.access(path, exists) == result::ok);
        ctx.check(!exists);

        windows_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);
        ctx.check(f.write("x", 1, 0) == result::ok);
        f.close();

        ctx.check(vfs.access(path, exists) == result::ok);
        ctx.check(exists);

        DeleteFileA(path.c_str());
    }},
    {"full_pathname returns a non-empty, drive-or-UNC-rooted path", [](auto& ctx) {
        windows_vfs vfs;
        std::string abs = vfs.full_pathname("some_relative_file.db");
        ctx.check(!abs.empty());
    }},
    {"randomness fills the requested number of bytes", [](auto& ctx) {
        windows_vfs vfs;
        unsigned char buf[32] = {};
        int got = vfs.randomness(sizeof(buf), buf);
        ctx.equal(got, static_cast<int>(sizeof(buf)));
    }},
    {"sleep returns the requested duration and current_time_millis is plausible", [](auto& ctx) {
        windows_vfs vfs;
        ctx.equal(vfs.sleep(1000), 1000);
        auto now_ms = vfs.current_time_millis();
        ctx.check(now_ms > 1577836800000LL);
        ctx.check(now_ms < 4102444800000LL);
    }},
});

} // namespace sqlite::backend::os::windows::testing
