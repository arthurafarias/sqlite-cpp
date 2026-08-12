#pragma once

#include "../unix_vfs.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <cstdlib>
#include <string>
#include <unistd.h>
#include <vector>

namespace sqlite::backend::os::unix::testing {

using sqlite::utils::testing::test_group;

inline const test_group unix_vfs_tests("unix_vfs", {
    {"access reports false for a nonexistent path and true after creating it", [](auto& ctx) {
        unix_vfs vfs;
        std::string path = "/tmp/sqlite_cpp_unix_vfs_test_access";
        ::unlink(path.c_str());

        bool exists = true;
        ctx.check(vfs.access(path, exists) == result::ok);
        ctx.check(!exists);

        unix_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);
        ctx.check(f.write("x", 1, 0) == result::ok);
        f.close();

        ctx.check(vfs.access(path, exists) == result::ok);
        ctx.check(exists);

        ::unlink(path.c_str());
    }},
    {"delete_file removes an existing file and is a no-op on a missing one", [](auto& ctx) {
        unix_vfs vfs;
        std::string path = "/tmp/sqlite_cpp_unix_vfs_test_delete";
        unix_file f;
        ctx.check(vfs.open(path, open_flags::readwrite | open_flags::create | open_flags::main_db, f) == result::ok);
        f.close();

        ctx.check(vfs.delete_file(path, true) == result::ok);
        bool exists = true;
        ctx.check(vfs.access(path, exists) == result::ok);
        ctx.check(!exists);

        // Deleting again (already gone) is still SQLITE_OK, not an error.
        ctx.check(vfs.delete_file(path, true) == result::ok);
    }},
    {"full_pathname turns a relative path into an absolute one", [](auto& ctx) {
        unix_vfs vfs;
        std::string abs = vfs.full_pathname("some_relative_file.db");
        ctx.check(!abs.empty());
        ctx.equal(abs.front(), '/');
    }},
    {"full_pathname leaves an already-absolute path's leading slash alone", [](auto& ctx) {
        unix_vfs vfs;
        std::string abs = vfs.full_pathname("/tmp/already_absolute.db");
        ctx.equal(abs.front(), '/');
    }},
    {"randomness fills the requested number of bytes", [](auto& ctx) {
        unix_vfs vfs;
        unsigned char buf[32] = {};
        int got = vfs.randomness(sizeof(buf), buf);
        ctx.equal(got, static_cast<int>(sizeof(buf)));
    }},
    {"sleep returns the requested duration and current_time_millis is plausible", [](auto& ctx) {
        unix_vfs vfs;
        ctx.equal(vfs.sleep(1000), 1000);
        auto now_ms = vfs.current_time_millis();
        // Sanity bound: any time after 2020-01-01 and before year 2100.
        ctx.check(now_ms > 1577836800000LL);
        ctx.check(now_ms < 4102444800000LL);
    }},
});

} // namespace sqlite::backend::os::unix::testing
