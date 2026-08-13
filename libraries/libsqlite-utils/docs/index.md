# sqlite-utils

`sqlite::utils` -- generic, SQL-unaware utilities used across every other
sqlite-cpp library. No dependency on any other `sqlite-*` library (SRS S3.1,
S3.2). This is phase 1 of the [phased delivery plan](../../../srs/001-sqlite-cpp-modularization.md#12-phased-delivery-plan)
in [SRS 001](../../../srs/001-sqlite-cpp-modularization.md): the lowest-dependency library, built first as
the template every later phase's library, testing, and documentation
conventions follow.

## Position in the dependency graph

```
sqlite-utils            <- you are here (no sqlite-* dependencies)
     |
     +-- sqlite-backend-os / -pager / -tree   (not yet built)
              |
         sqlite-core                          (not yet built)
              |
         sqlite-compiler                      (not yet built)
```

## Namespaces and legacy source mapping

| Namespace | Header(s) | Legacy source | Status |
|---|---|---|---|
| `sqlite::utils::memory` | `memory/allocator.hpp`, `memory/system_allocator.hpp` | `mem1.c` | System-malloc allocator ported. `mem0.c`/`mem2.c`/`mem3.c`/`mem5.c` (SQLITE_ZERO_MALLOC / debug / memsys5 alternate allocators) deferred -- not part of the default build configuration this phase targets. |
| `sqlite::utils::hash` | `hash/hash_table.hpp` | `hash.c`, `hash.h` | Fully ported: same Knuth multiplicative string hash, same ASCII case-insensitive lookup, same rehash growth policy. |
| `sqlite::utils::random` | `random/chacha20_prng.hpp` | `random.c` | ChaCha20 block function and refill scheme ported bit-for-bit; entropy *source* is `std::random_device` rather than the VFS `xRandomness` hook (which lives in `sqlite-backend-os`, not yet built). |
| `sqlite::utils::status` | `status/status_category.hpp`, `status/status_registry.hpp` | `status.c` | Fully ported (current value + high-water mark per category); synchronization is internal to the registry rather than a caller-held external mutex. |
| `sqlite::utils::text` | `text/utf.hpp`, `text/printf.hpp` | `utf.c`, `printf.c` | UTF-8 encode/decode ported bit-for-bit, including the documented lenient handling of invalid sequences. `mprintf`'s core conversions (`%d %i %u %x %X %o %c %s %z %q %Q %w %f %e %g %p %%`) are ported; `%n` and the control-character-escaping mode are deferred (see the header comment in `printf.hpp`). |
| `sqlite::utils::mutex` | `mutex/mutex_concept.hpp`, `mutex/mutex_id.hpp`, `mutex/noop_mutex.hpp` | `mutex.c`, `mutex.h`, `mutex_noop.c` | Interface (a `concept`) and the OS-independent no-op body only, per the SRS S3.2 table's explicit split -- pthread/Win32 bodies are deferred to `sqlite-backend-os` (SRS S3.3.1). |
| `sqlite::utils::testing` | `testing/test_group.hpp` and friends, `testing/fault_injector.hpp` | `fault.c` | The shared self-registering test harness every `*_test.hpp` in this library (and, per SRS FR-2, every later library) builds on, plus the benign-malloc-failure hooks, reclassified here as test infrastructure per the SRS S3.2 table. |

`util.c`'s number-parsing/formatting helpers beyond what `printf.c` covers are
not part of this phase; they will land with whichever later phase first needs
them (most likely `sqlite-compiler-tokenizer` or `sqlite-core-virtual-machine`).

## Building and running tests independently

This library is buildable and testable completely standalone:

```sh
cmake -S libraries/libsqlite-utils -B build-utils
cmake --build build-utils
ctest --test-dir build-utils
```

Or, as part of the full workspace build (`cmake -S . -B build && cmake --build build`),
run just this library's tests with:

```sh
ctest --test-dir build -R sqlite-utils
```

Every `include/sqlite/utils/**/testing/*_test.hpp` header self-registers a
named `test_group` (SRS FR-11) on inclusion; `tests/CMakeLists.txt` globs them
into one generated `all_tests.cpp`, mirroring the pattern this refactor is
modeled on (`/home/arthur/Projects/Software/opheap`'s
`include/.../testing/*_test.hpp` convention, SRS S1.4).

## Consuming this library

```cmake
include(cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()
target_link_libraries(your_target PRIVATE sqlite::utils)
```

`sqlite_cpp_require_utils()` (SRS FR-15) resolves an existing `sqlite::utils`
target, falls back to `find_package(sqlite-utils)`, and finally falls back to
building the in-tree library -- so a consumer builds standalone even with
nothing preinstalled.
