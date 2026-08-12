#pragma once

#include "../windows_file.hpp"
#include "../windows_vfs.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <cstring>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace sqlite::backend::os::windows::testing {

using sqlite::utils::testing::test_group;

namespace {
std::string make_temp_path(const char* suffix) {
    wchar_t dir[MAX_PATH];
    GetTempPathW(MAX_PATH, dir);
    wchar_t name[MAX_PATH];
    GetTempFileNameW(dir, L"scp", 0, name);
    DeleteFileW(name);
    int len = WideCharToMultiByte(CP_UTF8, 0, name, -1, nullptr, 0, nullptr, nullptr);
    std::string result(static_cast<std::size_t>(len - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, name, -1, result.data(), len, nullptr, nullptr);
    return result + suffix;
}
} // namespace

inline const test_group windows_file_tests("windows_file", {
    {"open/write/read round-trips through a real file", [](auto& ctx) {
        windows_vfs vfs;
        std::string path = make_temp_path(".db");
        windows_file f;
        result r = vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f);
        ctx.check(r == result::ok);
        ctx.check(f.is_open());

        const char msg[] = "hello, sqlite-cpp";
        ctx.check(f.write(msg, sizeof(msg), 0) == result::ok);

        char readback[sizeof(msg)] = {};
        ctx.check(f.read(readback, sizeof(readback), 0) == result::ok);
        ctx.check(std::string(readback, sizeof(readback)) == std::string(msg, sizeof(msg)));

        f.close();
        DeleteFileA(path.c_str());
    }},
    {"reading past EOF short-reads and zero-fills the tail", [](auto& ctx) {
        windows_vfs vfs;
        std::string path = make_temp_path(".db");
        windows_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);
        ctx.check(f.write("ab", 2, 0) == result::ok);

        char buf[8];
        std::memset(buf, 0x7f, sizeof(buf));
        result r = f.read(buf, sizeof(buf), 0);
        ctx.check(r == result::ioerr_short_read);
        ctx.equal(buf[0], 'a');
        ctx.equal(buf[1], 'b');
        ctx.equal(buf[2], char{0});
        ctx.equal(buf[7], char{0});

        f.close();
        DeleteFileA(path.c_str());
    }},
    {"truncate and file_size agree", [](auto& ctx) {
        windows_vfs vfs;
        std::string path = make_temp_path(".db");
        windows_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);
        ctx.check(f.write("0123456789", 10, 0) == result::ok);
        ctx.check(f.truncate(4) == result::ok);

        std::int64_t size = -1;
        ctx.check(f.file_size(size) == result::ok);
        ctx.equal(size, std::int64_t{4});

        f.close();
        DeleteFileA(path.c_str());
    }},
    {"lock/unlock through a real file handle follows the shared/reserved/exclusive protocol", [](auto& ctx) {
        windows_vfs vfs;
        std::string path = make_temp_path(".db");
        windows_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);

        ctx.check(f.lock(lock_level::shared) == result::ok);
        ctx.check(f.lock(lock_level::reserved) == result::ok);
        ctx.check(f.lock(lock_level::exclusive) == result::ok);
        ctx.check(f.unlock(lock_level::none) == result::ok);

        f.close();
        DeleteFileA(path.c_str());
    }},
    {"sync succeeds on a real file", [](auto& ctx) {
        windows_vfs vfs;
        std::string path = make_temp_path(".db");
        windows_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);
        ctx.check(f.write("x", 1, 0) == result::ok);
        ctx.check(f.sync(sync_flags::normal) == result::ok);
        f.close();
        DeleteFileA(path.c_str());
    }},
});

} // namespace sqlite::backend::os::windows::testing
