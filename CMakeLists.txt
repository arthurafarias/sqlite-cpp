cmake_minimum_required(VERSION 3.24)
# C is needed here (not just in the per-target subdirectories) because
# cmake/SqliteCodegen.cmake's host tools (lemon, mksourceid, mkkeywordhash) are
# add_executable()'d from this scope, before any subdirectory's own project() runs.
# CXX is enabled for the first four converted dependency leaves, through
# `sqlite-backend-tree`. C remains enabled for the six split libraries still
# at the SRS 001 stage, plus the legacy
# amalgamation, generators, and fixture.
project(sqlite3-workspace LANGUAGES C CXX)

option(SQLITE_BUILD_SHELL "Build the sqlite3-shell-legacy CLI shell" ON)
option(SQLITE_BUILD_TESTS "Build testfixture and register the TCL test suite with ctest" ON)
option(SQLITE_BUILD_BENCHMARKS "Build speedtest1-legacy" ON)
option(SQLITE_BUILD_APPLICATIONS "Build sqldiff-legacy and sqlite3-rsync-legacy" ON)

enable_testing()

add_subdirectory(libraries/sqlite3-legacy)
add_subdirectory(libraries/sqlite3-legacy-alternate)

# SRS 001 FR-4: each library builds as its own SHARED object over the
# csrc/ FR-3 populated, with no target_link_libraries() between siblings
# (see cmake/SqliteCppLibrary.cmake for why -- the real C symbol graph is
# cyclic, and CMake refuses target-level cycles between SHARED libraries).

add_subdirectory(libraries/sqlite3)
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

# SRS 001 FR-5: each sqlite3-dependent application links against the split
# above (cmake/SqliteCppLibrary.cmake's sqlite_cpp_link_full_split()) instead
# of embedding the amalgamation or linking libsqlite3-legacy.
if(SQLITE_BUILD_SHELL)
    # add_subdirectory(applications/sqlite3-shell-legacy)
endif()
if(SQLITE_BUILD_APPLICATIONS)
    # add_subdirectory(applications/sqldiff-legacy)
    # add_subdirectory(applications/sqlite3-rsync-legacy)
    add_subdirectory(applications/example-001)
endif()
if(SQLITE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(SQLITE_BUILD_BENCHMARKS)
    # add_subdirectory(applications/speedtest1-legacy)
endif()
