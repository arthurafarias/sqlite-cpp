cmake_minimum_required(VERSION 3.24)
project(sqlite3-workspace LANGUAGES C CXX)

option(SQLITE_BUILD_SHELL "Build the sqlite3-shell-legacy CLI shell" ON)
option(SQLITE_BUILD_TESTS "Build testfixture and register the TCL test suite with ctest" ON)
option(SQLITE_BUILD_BENCHMARKS "Build speedtest1-legacy" ON)
option(SQLITE_BUILD_APPLICATIONS "Build sqldiff-legacy and sqlite3-rsync-legacy" ON)

enable_testing()

include(cmake/SqliteCppLibrary.cmake)

add_subdirectory(libraries/sqlite3-legacy)
add_subdirectory(libraries/sqlite3-legacy-alternate)

# SRS 005: each of the ten SRS-001 sec.1.3 subsystem libraries builds as its
# own SHARED object over the csrc/ this document's FR-1 populated from
# sqlite3-legacy-alternate, with no target_link_libraries() between siblings
# (see cmake/SqliteCppLibrary.cmake for why -- the real C symbol graph is
# cyclic, and CMake refuses target-level cycles between SHARED libraries).
# libraries/sqlite3 (below) is the umbrella that links the whole set together
# and resolves what each leaves unresolved.
add_subdirectory(libraries/sqlite3-utils)
add_subdirectory(libraries/sqlite3-backend-os)
add_subdirectory(libraries/sqlite3-backend-pager)
add_subdirectory(libraries/sqlite3-backend-tree)
add_subdirectory(libraries/sqlite3-core-virtual-machine)
add_subdirectory(libraries/sqlite3-core-command-processor)
add_subdirectory(libraries/sqlite3-core-interface)
add_subdirectory(libraries/sqlite3-compiler-tokenizer)
add_subdirectory(libraries/sqlite3-compiler-parser)
add_subdirectory(libraries/sqlite3-compiler-code-generator)

# SRS 005: the umbrella library -- links all ten subsystem libraries above
# and exposes the sqlite3_state public interface (struct sqlite3 renamed at
# the API boundary; see srs/005-*.md).
add_subdirectory(libraries/sqlite3)

# SRS 001 FR-5: each sqlite3-dependent application links against the split
# above instead of embedding the amalgamation or linking libsqlite3-legacy.
if(SQLITE_BUILD_SHELL)
    # add_subdirectory(applications/sqlite3-shell-legacy)
endif()
if(SQLITE_BUILD_APPLICATIONS)
    # add_subdirectory(applications/sqldiff-legacy)
    # add_subdirectory(applications/sqlite3-rsync-legacy)
    add_subdirectory(applications/example-001)
    add_subdirectory(applications/example-002)
endif()
if(SQLITE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(SQLITE_BUILD_BENCHMARKS)
    # add_subdirectory(applications/speedtest1-legacy)
endif()
