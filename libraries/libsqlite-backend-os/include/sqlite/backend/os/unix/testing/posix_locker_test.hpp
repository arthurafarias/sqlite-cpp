#pragma once

#include "../posix_locker.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace sqlite::backend::os::unix::testing {

using sqlite::utils::testing::test_group;

namespace {
int make_temp_fd(std::string& path_out) {
    std::string tmpl = "/tmp/sqlite_cpp_posix_locker_test_XXXXXX";
    std::vector<char> buf(tmpl.begin(), tmpl.end());
    buf.push_back('\0');
    int fd = ::mkstemp(buf.data());
    path_out.assign(buf.data());
    return fd;
}
} // namespace

inline const test_group posix_locker_tests("posix_locker", {
    {"lock then unlock the same range succeeds", [](auto& ctx) {
        std::string path;
        int fd = make_temp_fd(path);
        ctx.check(fd >= 0);

        posix_locker locker(fd);
        ctx.check(locker.lock_range(0, 1, /*exclusive=*/true) == result::ok);
        ctx.check(locker.unlock_range(0, 1) == result::ok);

        ::close(fd);
        ::unlink(path.c_str());
    }},
    {"a same-process, different-fd relock on the same range replaces rather than conflicts (fcntl is process-scoped)", [](auto& ctx) {
        std::string path;
        int fd_a = make_temp_fd(path);
        ctx.check(fd_a >= 0);
        int fd_b = ::open(path.c_str(), O_RDWR);
        ctx.check(fd_b >= 0);

        posix_locker a(fd_a);
        posix_locker b(fd_b);
        ctx.check(a.lock_range(200, 10, /*exclusive=*/true) == result::ok);
        // Documented in byte_range_lock.hpp: this port doesn't reproduce the
        // legacy unixInodeInfo same-process/multi-fd interlocking, so a
        // second fd in this same process silently succeeds here (POSIX
        // advisory locks are scoped to the process, not the fd) rather than
        // returning SQLITE_BUSY the way two different *processes* would see.
        ctx.check(b.lock_range(200, 10, /*exclusive=*/true) == result::ok);

        ::close(fd_a);
        ::close(fd_b);
        ::unlink(path.c_str());
    }},
});

} // namespace sqlite::backend::os::unix::testing
