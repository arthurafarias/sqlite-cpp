# sqlite-backend-os

`sqlite::backend::os` -- the VFS/file abstraction layer: platform file I/O
and the byte-range locking protocol that gives SQLite its cross-process
concurrency guarantees. Phase 2 of the [phased delivery plan](../../../srs/001-sqlite-cpp-modularization.md#12-phased-delivery-plan)
in [SRS 001](../../../srs/001-sqlite-cpp-modularization.md), and the FR-6 "paradigm case" -- the one place
in the whole refactor where a real, load-bearing platform fork (unix vs.
Windows) exists, as opposed to a feature flag.

## Position in the dependency graph

```
sqlite-utils
     |
sqlite-backend-os        <- you are here
     |
sqlite-backend-pager / -tree      (not yet built)
     |
sqlite-core / sqlite-compiler     (not yet built)
```

Depends on `sqlite::utils` (SRS S3.1) for `sqlite::utils::random::chacha20_prng`
(entropy source for `sqlite3_randomness`) and `sqlite::utils::mutex`'s
`mutex_type` concept (the interface this library's `unix::pthread_mutex` and
`windows::win32_mutex` implement).

## Namespaces and legacy source mapping

| Namespace | Header(s) | Legacy source |
|---|---|---|
| `sqlite::backend::os` | `lock_level.hpp`, `open_flags.hpp`, `device_characteristics.hpp`, `result.hpp`, `file_concept.hpp`, `vfs_concept.hpp` | `os.h`, `os.c`, `sqlite3.h`'s `SQLITE_OPEN_*`/`SQLITE_IOCAP_*`/`SQLITE_STATUS_*` constants |
| `sqlite::backend::os::detail` | `detail/byte_range_lock.hpp` | `os_unix.c`'s `unixLock`/`posixUnlock`/`unixCheckReservedLock` and `os_win.c`'s `winLock`/`winUnlock`/`winCheckReservedLock`, unified into one shared implementation (see below) |
| `sqlite::backend::os::unix` | `unix/posix_locker.hpp`, `unix/unix_file.hpp`, `unix/unix_vfs.hpp`, `unix/pthread_mutex.hpp` | `os_unix.c`, `mutex_unix.c` |
| `sqlite::backend::os::windows` | `windows/win32_locker.hpp`, `windows/windows_file.hpp`, `windows/windows_vfs.hpp`, `windows/win32_mutex.hpp` | `os_win.c`, `os_win.h`, `mutex_w32.c` |

## The FR-6 paradigm case, adapted to a header-only library

FR-6 requires platform-family branching to be "two separate implementation
files selected by CMake `if()`, not `#ifdef`" -- written against a C build
where `target_sources()` picks one `.c` file or the other. For a header-only
C++ library, nothing is ever compiled into the library itself, so there is no
`target_sources()` step to gate. The adaptation this library makes: the
selection point moves to whichever consumer eventually instantiates a
concrete VFS -- `if(WIN32)` picks between including `unix/unix_vfs.hpp` or
`windows/windows_vfs.hpp` (this library's own `tests/CMakeLists.txt` does
exactly this to decide which platform's `testing/*_test.hpp` headers to
compile). No `#ifdef` branches on platform anywhere in this library's
sources; the two platforms are two disjoint sets of headers, full stop.

## One shared lock state machine, two lockers

`os.h`'s own documentation says "the same locking strategy and byte ranges
are used for Unix and Windows" -- both implement the identical
SHARED/RESERVED/PENDING/EXCLUSIVE protocol over the same three well-known
byte ranges, differing only in the OS primitive used to lock/unlock a byte
range (POSIX `fcntl()` vs. Win32 `LockFileEx()`/`UnlockFileEx()`). Rather
than have `os_unix.c`'s `unixLock` and `os_win.c`'s `winLock` duplicate that
protocol as they do in the legacy code (with a real risk of the two silently
diverging over time), `detail::byte_range_lock_state_machine<Locker>`
implements it exactly once, as a template over a small `byte_range_locker`
concept (`lock_range`/`unlock_range`). `unix::posix_locker` and
`windows::win32_locker` are the two ~40-line bodies of that concept;
`unix::unix_file` and `windows::windows_file` each just plug one in.

This consolidation surfaced a real platform difference the legacy code's
duplication had each handle separately, on its own: POSIX `fcntl()` can
atomically convert a held lock to a different mode on the same byte range
(a shared read lock to an exclusive write lock, or back), but Win32's
`LockFileEx`/`UnlockFileEx` cannot -- `winLock()`/`winUnlock()` explicitly
unlock first and, on a failed upgrade, restore the previous lock. The shared
state machine now always does the unlock-first sequence (a functionally
harmless extra syscall pair on unix, required correctness on Windows). This
was caught, not reasoned out in the abstract: an early version that didn't do
this passed all of the unix-side tests but failed
`windows_file`'s lock/unlock test the first time it actually ran under Wine
(see below) -- concrete evidence for why this library's own tests should run
for real on both platforms rather than being trusted by inspection.

## Known scope reductions vs. the legacy code

Each is called out at the point in the code where it matters; summarized
here:

- **No same-process, multiple-file-descriptor lock interlocking.** The
  legacy `unixInodeInfo`/`unixShmNode` machinery additionally tracks every
  open `unixFile*` on the same inode within the same process, because POSIX
  advisory locks are scoped to the process, not the file descriptor. This
  port tracks lock state per open handle only -- correct for at most one open
  handle per file per process (the common case), not for two handles on the
  same file within one process (`unix/testing/posix_locker_test.hpp`'s
  second test demonstrates the resulting, documented behavior directly).
- **No WAL shared memory** (`xShmMap`/`xShmLock`/`xShmBarrier`/`xShmUnmap`),
  **no memory-mapped I/O** (`xFetch`/`xUnfetch`), **no extension-loading
  hooks** (`xDlOpen` et al. -- these belong with a future
  `sqlite-core-interface::extension`, SRS S3.4.3). All are real
  `sqlite3_io_methods`/`sqlite3_vfs` entries, each a separable extension of
  the core file contract `file_concept.hpp`/`vfs_concept.hpp` define, which
  is what the rollback-journal path needs.
- **No fd/handle reuse for a main-database file pending delete**, **no
  VFS-generated temp filenames**, **no directory-fsync-after-journal-creation
  durability step**, **no NFS/Apple-specific locking workarounds** -- legacy
  performance/robustness refinements layered on top of the core open/lock
  contract, not requirements of the contract itself.
- **`sqlite::utils::random` instead of a VFS-specific entropy source**
  (`sqlite3OsRandomness`'s `/dev/urandom`/`CryptGenRandom` reads) -- see
  `sqlite-utils`'s own docs for why (SRS S3.2).

None of these change the *observable contract* this phase's tests check
(open/read/write/truncate/sync/lock/unlock/check-reserved-lock, and their
cross-process semantics); they are each, individually, a further port.

## Building and running tests independently

```sh
cmake -S libraries/libsqlite-backend-os -B build-backend-os
cmake --build build-backend-os
ctest --test-dir build-backend-os
```

Or, as part of the workspace build: `ctest --test-dir build -R sqlite-backend-os`.

### Verifying the Windows body from Linux

This library's Windows body is real Win32 code (`CreateFileW`, `ReadFile`/
`WriteFile`, `LockFileEx`/`UnlockFileEx`, `CRITICAL_SECTION`), not a stub --
but this development environment has no Windows machine. It's cross-compiled
with mingw-w64 and its test binary run for real under Wine instead:

```sh
cmake -S libraries/libsqlite-backend-os -B build-backend-os-win \
      -DCMAKE_TOOLCHAIN_FILE=../../cmake/mingw-w64-toolchain.cmake
cmake --build build-backend-os-win
wine build-backend-os-win/tests/sqlite_backend_os_tests.exe
```

This is a documented manual workflow (see `tests/CMakeLists.txt`'s comment),
not part of the default `ctest` run, since it depends on a mingw-w64 + Wine
host setup this library does not otherwise require. It is, however, real
verification, not a formality: the lock-conversion difference described
above was caught by an actual test failure under this exact workflow.

## Consuming this library

```cmake
include(cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_backend_os()
target_link_libraries(your_target PRIVATE sqlite::backend::os)
```
