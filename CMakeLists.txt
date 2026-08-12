cmake_minimum_required(VERSION 3.24)
# C is needed here (not just in the per-target subdirectories) because
# cmake/SqliteCodegen.cmake's host tools (lemon, mksourceid, mkkeywordhash) are
# add_executable()'d from this scope, before any subdirectory's own project() runs.
# CXX is needed for the sqlite-cpp libraries under libraries/libsqlite-utils
# and its future siblings (SRS S3), which live alongside the original C
# sources/build rather than replacing them (SRS S2.5).
project(sqlite-workspace LANGUAGES C CXX)

option(SQLITE_BUILD_SHELL "Build the sqlite3 CLI shell" ON)
option(SQLITE_BUILD_TESTS "Build testfixture and register the TCL test suite with ctest" ON)
option(SQLITE_BUILD_BENCHMARKS "Build speedtest1" ON)
option(SQLITE_BUILD_APPLICATIONS "Build sqldiff and sqlite3-rsync" ON)
option(SQLITE_CPP_BUILD_UTILS "Build the sqlite-cpp sqlite::utils library (SRS phase 1)" ON)
option(SQLITE_CPP_BUILD_BACKEND_OS "Build the sqlite-cpp sqlite::backend::os library (SRS phase 2)" ON)

# Called unconditionally (rather than only under SQLITE_BUILD_TESTS below) so
# that libraries/libsqlite-utils's own tests/CMakeLists.txt -- added via
# sqlite_cpp_require_utils() before the SQLITE_BUILD_TESTS block runs -- gets
# a root CTestTestfile.cmake to register into; enable_testing() is otherwise
# a no-op declaration and calling it twice is harmless.
enable_testing()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteCodegen.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteCppDependency.cmake)

add_subdirectory(libraries/libsqlite3)

# SRS phase 1 (S12): sqlite-utils is the lowest-dependency sqlite-cpp
# library, added alongside (not replacing) the legacy libsqlite3 target above.
if(SQLITE_CPP_BUILD_UTILS)
    sqlite_cpp_require_utils()
endif()

# SRS phase 2 (S12): sqlite-backend-os, the FR-6 paradigm case (unix/Windows
# selected structurally -- see libraries/libsqlite-backend-os/docs/index.md
# for how that adapts to a header-only library).
if(SQLITE_CPP_BUILD_BACKEND_OS)
    sqlite_cpp_require_backend_os()
endif()

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
