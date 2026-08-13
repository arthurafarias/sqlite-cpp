cmake_minimum_required(VERSION 3.24)
# C is needed here (not just in the per-target subdirectories) because
# cmake/SqliteCodegen.cmake's host tools (lemon, mksourceid, mkkeywordhash) are
# add_executable()'d from this scope, before any subdirectory's own project() runs.
# CXX is needed for the sqlite-cpp libraries under libraries/libsqlite-utils
# and its future siblings (SRS S3), which live alongside the original C
# sources/build rather than replacing them (SRS S2.5).
project(sqlite-workspace LANGUAGES C CXX)

option(SQLITE_BUILD_SHELL "Build the sqlite3-shell-legacy CLI shell" ON)
option(SQLITE_BUILD_TESTS "Build testfixture and register the TCL test suite with ctest" ON)
option(SQLITE_BUILD_BENCHMARKS "Build speedtest1-legacy" ON)
option(SQLITE_BUILD_APPLICATIONS "Build sqldiff-legacy and sqlite3-rsync-legacy" ON)

# Called unconditionally (rather than only under SQLITE_BUILD_TESTS below) so
# that libraries/libsqlite-utils's own tests/CMakeLists.txt -- added via
# sqlite_cpp_require_utils() before the SQLITE_BUILD_TESTS block runs -- gets
# a root CTestTestfile.cmake to register into; enable_testing() is otherwise
# a no-op declaration and calling it twice is harmless.
enable_testing()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteCodegen.cmake)

add_subdirectory(libraries/libsqlite3-legacy)

# SRS 001 FR-2-FR-6 (legacy consolidation, per-library split, dynamic
# linking, legacy executables, rewiring) are not yet done -- see
# srs/001-legacy-restructure-and-dynamic-linking.md S6. Nothing under
# libraries/ besides libsqlite3-legacy exists yet.

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
