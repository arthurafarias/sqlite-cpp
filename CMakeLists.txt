cmake_minimum_required(VERSION 3.24)
# C is needed here (not just in the per-target subdirectories) because
# cmake/SqliteCodegen.cmake's host tools (lemon, mksourceid, mkkeywordhash) are
# add_executable()'d from this scope, before any subdirectory's own project() runs.
project(sqlite-workspace LANGUAGES C)

option(SQLITE_BUILD_SHELL "Build the sqlite3 CLI shell" ON)
option(SQLITE_BUILD_TESTS "Build testfixture and register the TCL test suite with ctest" ON)
option(SQLITE_BUILD_BENCHMARKS "Build speedtest1" ON)
option(SQLITE_BUILD_APPLICATIONS "Build sqldiff and sqlite3-rsync" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteCodegen.cmake)

add_subdirectory(libraries/libsqlite3)

if(SQLITE_BUILD_SHELL)
    add_subdirectory(applications/sqlite3-shell)
endif()
if(SQLITE_BUILD_APPLICATIONS)
    add_subdirectory(applications/sqldiff)
    add_subdirectory(applications/sqlite3-rsync)
endif()
if(SQLITE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(SQLITE_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
