cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-backend-os VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-backend-os test binary and register it with ctest" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()

# Header-only (SRS FR-3), same INTERFACE-library shape as sqlite-utils
# (libraries/libsqlite-utils/CMakeLists.txt).
add_library(sqlite-backend-os INTERFACE)
add_library(sqlite::backend::os ALIAS sqlite-backend-os)

target_compile_features(sqlite-backend-os INTERFACE cxx_std_20)
target_include_directories(sqlite-backend-os
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(sqlite-backend-os INTERFACE sqlite::utils)

if(WIN32)
    target_link_libraries(sqlite-backend-os INTERFACE ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(sqlite-backend-os INTERFACE Threads::Threads)
endif()

# Note: CMAKE_CXX_EXTENSIONS defaults ON, which makes GCC/Clang predefine
# `unix` as a macro expanding to 1. Since this is an INTERFACE library,
# CXX_EXTENSIONS can't be forced on consumers here (it isn't a propagated
# usage requirement) -- every header under include/.../unix/ instead works
# around it locally with `#undef unix` (see unix/posix_locker.hpp).

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-backend-os EXPORT sqlite-backend-osTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-backend-osTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-os)
configure_package_config_file(
    cmake/sqlite-backend-osConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-osConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-os
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-osConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-osConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-osConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-os
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
