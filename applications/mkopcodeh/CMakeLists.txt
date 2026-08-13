cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-5: mkopcodeh-legacy's complete C++ replacement -- unblocked (no
# sqlite3 dependency, no facade), so it is unsuffixed rather than
# reserved/gated like the sqlite3-dependent applications. See
# applications/mkopcodeh-legacy/mkopcodeh.tcl for the legacy source this is
# a faithful, deterministic port of (RR-7 generator-parity target).
project(mkopcodeh LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the mkopcodeh test binary and register it with ctest" ON)

add_executable(mkopcodeh main.cpp)
target_include_directories(mkopcodeh PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(mkopcodeh PRIVATE cxx_std_20)

include(GNUInstallDirs)
install(TARGETS mkopcodeh RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(SQLITE_CPP_BUILD_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
    sqlite_cpp_require_utils()
    enable_testing()
    add_subdirectory(tests)
endif()
