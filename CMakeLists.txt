cmake_minimum_required(VERSION 3.24)
# C is needed here (not just in the per-target subdirectories) because
# cmake/SqliteCodegen.cmake's host tools (lemon, mksourceid, mkkeywordhash) are
# add_executable()'d from this scope, before any subdirectory's own project() runs.
# CXX is declared up front for SRS 002 (C++ conversion, not yet started for any
# library): every libraries/<name> is plain C today (SRS 001), so nothing
# currently compiles C++, but the language needs to be available in this
# top-level project() call before any library that starts using it.
project(sqlite-workspace LANGUAGES C CXX)

option(SQLITE_BUILD_SHELL "Build the sqlite3-shell-legacy CLI shell" ON)
option(SQLITE_BUILD_TESTS "Build testfixture and register the TCL test suite with ctest" ON)
option(SQLITE_BUILD_BENCHMARKS "Build speedtest1-legacy" ON)
option(SQLITE_BUILD_APPLICATIONS "Build sqldiff-legacy and sqlite3-rsync-legacy" ON)

enable_testing()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteCodegen.cmake)

add_subdirectory(libraries/libsqlite3-legacy)

# SRS 001 FR-4: each library builds as its own SHARED object over the
# csrc/ FR-3 populated, with no target_link_libraries() between siblings
# (see cmake/SqliteCppLibrary.cmake for why -- the real C symbol graph is
# cyclic, and CMake refuses target-level cycles between SHARED libraries).
add_subdirectory(libraries/libsqlite-utils)
add_subdirectory(libraries/libsqlite-backend-os)
add_subdirectory(libraries/libsqlite-backend-pager)
add_subdirectory(libraries/libsqlite-backend-tree)
add_subdirectory(libraries/libsqlite-core-virtual-machine)
add_subdirectory(libraries/libsqlite-core-command-processor)
add_subdirectory(libraries/libsqlite-core-interface)
add_subdirectory(libraries/libsqlite-compiler-tokenizer)
add_subdirectory(libraries/libsqlite-compiler-parser)
add_subdirectory(libraries/libsqlite-compiler-code-generator)

# SRS 001 FR-5: each sqlite3-dependent application links against the split
# above (cmake/SqliteCppLibrary.cmake's sqlite_cpp_link_full_split()) instead
# of embedding the amalgamation or linking libsqlite3-legacy.
if(SQLITE_BUILD_SHELL)
    add_subdirectory(applications/sqlite3-shell-legacy)
endif()
if(SQLITE_BUILD_APPLICATIONS)
    add_subdirectory(applications/sqldiff-legacy)
    add_subdirectory(applications/sqlite3-rsync-legacy)
endif()
if(SQLITE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(SQLITE_BUILD_BENCHMARKS)
    add_subdirectory(applications/speedtest1-legacy)
endif()
