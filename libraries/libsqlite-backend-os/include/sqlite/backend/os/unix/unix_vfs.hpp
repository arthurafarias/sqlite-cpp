#pragma once

#include "../open_flags.hpp"
#include "../result.hpp"
#include "../vfs_concept.hpp"
#include "unix_file.hpp"

#include <sqlite/utils/random/chacha20_prng.hpp>

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <limits.h>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>

#ifdef unix
#undef unix
#endif

// Legacy source: os_unix.c's unixOpen/unixDelete/unixAccess/unixFullPathname/
// unixSleep/unixCurrentTimeInt64, and (via sqlite::utils::random,
// SRS S3.2) sqlite3_randomness's entropy source. Simplified relative to the
// legacy VFS in ways documented at each method below; every simplification
// preserves the documented *contract* of the corresponding sqlite3_vfs
// method while dropping legacy performance/robustness refinements that are
// each, individually, a substantial further port (fd reuse across pending
// unlinked files, directory-fsync-after-journal-creation durability,
// VxWorks/Apple-specific paths, symlink-aware path canonicalization).
namespace sqlite::backend::os::unix {

class unix_vfs {
public:
    using file_type = unix_file;

    // unixOpen(): translates open_flags to POSIX open(2) flags and creates
    // the file. Deferred vs. the legacy VFS: reusing an already-open fd for
    // a main-database file pending delete (findReusableFd), and generating
    // a VFS-internal temp filename when `path` is empty (temp files here
    // must be named by the caller, e.g. via a real temp-directory path).
    result open(std::string_view path, open_flags flags, unix_file& out) {
        if (path.empty()) return result::cantopen;
        std::string path_str(path);

        int posix_flags = 0;
        bool readwrite = (flags & open_flags::readwrite) == open_flags::readwrite;
        posix_flags |= readwrite ? O_RDWR : O_RDONLY;
        if ((flags & open_flags::create) == open_flags::create) posix_flags |= O_CREAT;
        if ((flags & open_flags::exclusive) == open_flags::exclusive) posix_flags |= (O_EXCL | O_CREAT);

        int fd;
        do {
            fd = ::open(path_str.c_str(), posix_flags, 0644);
        } while (fd < 0 && errno == EINTR);
        if (fd < 0) {
            if (errno == EACCES || errno == EPERM) return result::perm;
            return result::cantopen;
        }
        out = unix_file(fd);
        return result::ok;
    }

    // unixDelete(): deferred vs. legacy: fsync()ing the containing directory
    // after unlink (a durability refinement -- makes the deletion itself
    // crash-safe on filesystems where directory entries are not implicitly
    // synced -- rather than a correctness requirement of "the file no longer
    // exists after this call returns SQLITE_OK").
    result delete_file(std::string_view path, bool /*sync_dir*/) const {
        std::string path_str(path);
        if (::unlink(path_str.c_str()) != 0) {
            if (errno == ENOENT) return result::ok;
            return result::ioerr_delete;
        }
        return result::ok;
    }

    // unixAccess(): SQLITE_ACCESS_EXISTS semantics (the common case this
    // library's callers need -- checking whether a database/journal file is
    // already there). SQLITE_ACCESS_READWRITE's access(W_OK|R_OK) variant is
    // not separately exposed by this concept, which only distinguishes
    // "exists or not" (see vfs_concept.hpp).
    result access(std::string_view path, bool& exists) const {
        std::string path_str(path);
        struct stat st{};
        exists = (::stat(path_str.c_str(), &st) == 0) && (!S_ISREG(st.st_mode) || st.st_size > 0);
        return result::ok;
    }

    // unixFullPathname(): resolves via realpath(3) when the path already
    // exists (matching legacy behavior for existing files/symlinks exactly,
    // since realpath() is POSIX's own symlink-resolving canonicalizer).
    // For a path that does not yet exist (e.g. creating a new database),
    // legacy still resolves each *existing* leading path component; this
    // port instead just prepends the current working directory to a
    // relative path, which is correct whenever the containing directory
    // itself isn't a dangling symlink -- the common case.
    [[nodiscard]] std::string full_pathname(std::string_view path) const {
        std::string path_str(path);
        char resolved[PATH_MAX];
        if (::realpath(path_str.c_str(), resolved) != nullptr) {
            return std::string(resolved);
        }
        if (!path.empty() && path.front() == '/') return path_str;
        char cwd[PATH_MAX];
        if (::getcwd(cwd, sizeof(cwd)) == nullptr) return path_str;
        return std::string(cwd) + "/" + path_str;
    }

    // sqlite3OsRandomness()'s entropy source, ported to draw from
    // sqlite::utils::random rather than a VFS-specific /dev/urandom read --
    // see chacha20_prng.hpp's header comment for why (SRS S3.2).
    int randomness(int n, void* buf) const {
        random_.randomness(n, buf);
        return n;
    }

    int sleep(int microseconds) const {
        if (microseconds <= 0) return 0;
        struct timespec ts{};
        ts.tv_sec = microseconds / 1000000;
        ts.tv_nsec = (microseconds % 1000000) * 1000;
        ::nanosleep(&ts, nullptr);
        return microseconds;
    }

    [[nodiscard]] std::int64_t current_time_millis() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }

private:
    mutable sqlite::utils::random::chacha20_prng random_;
};

static_assert(vfs<unix_vfs>);

} // namespace sqlite::backend::os::unix
