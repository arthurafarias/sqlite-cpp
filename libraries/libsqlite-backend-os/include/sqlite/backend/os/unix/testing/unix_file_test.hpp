#pragma once

#include "../unix_file.hpp"
#include "../unix_vfs.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vector>

namespace sqlite::backend::os::unix::testing {

using sqlite::utils::testing::test_group;

namespace {
std::string make_temp_path(const char* suffix) {
    std::string path = "/tmp/sqlite_cpp_unix_file_test_XXXXXX";
    std::vector<char> buf(path.begin(), path.end());
    buf.push_back('\0');
    int fd = ::mkstemp(buf.data());
    if (fd >= 0) ::close(fd);
    std::string result(buf.data());
    ::unlink(result.c_str());
    return result + suffix;
}
} // namespace

inline const test_group unix_file_tests("unix_file", {
    {"open/write/read round-trips through a real file", [](auto& ctx) {
        unix_vfs vfs;
        std::string path = make_temp_path(".db");
        unix_file f;
        result r = vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f);
        ctx.check(r == result::ok);
        ctx.check(f.is_open());

        const char msg[] = "hello, sqlite-cpp";
        ctx.check(f.write(msg, sizeof(msg), 0) == result::ok);

        char readback[sizeof(msg)] = {};
        ctx.check(f.read(readback, sizeof(readback), 0) == result::ok);
        ctx.check(std::string(readback, sizeof(readback)) == std::string(msg, sizeof(msg)));

        f.close();
        ::unlink(path.c_str());
    }},
    {"reading past EOF short-reads and zero-fills the tail", [](auto& ctx) {
        unix_vfs vfs;
        std::string path = make_temp_path(".db");
        unix_file f;
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
        ::unlink(path.c_str());
    }},
    {"truncate and file_size agree", [](auto& ctx) {
        unix_vfs vfs;
        std::string path = make_temp_path(".db");
        unix_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);
        ctx.check(f.write("0123456789", 10, 0) == result::ok);
        ctx.check(f.truncate(4) == result::ok);

        std::int64_t size = -1;
        ctx.check(f.file_size(size) == result::ok);
        ctx.equal(size, std::int64_t{4});

        f.close();
        ::unlink(path.c_str());
    }},
    {"lock/unlock through a real file descriptor follows the shared/reserved/exclusive protocol", [](auto& ctx) {
        unix_vfs vfs;
        std::string path = make_temp_path(".db");
        unix_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);

        ctx.check(f.lock(lock_level::shared) == result::ok);
        ctx.check(f.lock(lock_level::reserved) == result::ok);
        ctx.check(f.lock(lock_level::exclusive) == result::ok);
        ctx.check(f.unlock(lock_level::none) == result::ok);

        f.close();
        ::unlink(path.c_str());
    }},
    {"sync succeeds on a real file", [](auto& ctx) {
        unix_vfs vfs;
        std::string path = make_temp_path(".db");
        unix_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);
        ctx.check(f.write("x", 1, 0) == result::ok);
        ctx.check(f.sync(sync_flags::normal) == result::ok);
        f.close();
        ::unlink(path.c_str());
    }},
});

} // namespace sqlite::backend::os::unix::testing
