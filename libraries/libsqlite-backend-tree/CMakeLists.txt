cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-backend-tree VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-backend-tree test binary and register it with ctest" ON)
option(SQLITE_CPP_BUILD_BENCHMARKS "Build the sqlite-backend-tree vs. libsqlite3 comparative benchmark" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()
sqlite_cpp_require_backend_os()
sqlite_cpp_require_backend_pager()

# Header-only (SRS FR-3), same INTERFACE-library shape as the other
# sqlite-cpp libraries.
add_library(sqlite-backend-tree INTERFACE)
add_library(sqlite::backend::tree ALIAS sqlite-backend-tree)

target_compile_features(sqlite-backend-tree INTERFACE cxx_std_20)
target_include_directories(sqlite-backend-tree
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
# page_store.hpp is a thin adapter over sqlite::backend::pager (the real
# page cache + rollback journal); sqlite::backend::os is still linked
# directly for the same-format page-1 header detection page_store.hpp
# does before opening the pager (see that file's header comment).
target_link_libraries(sqlite-backend-tree INTERFACE sqlite::utils sqlite::backend::os sqlite::backend::pager)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-backend-tree EXPORT sqlite-backend-treeTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-backend-treeTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-tree)
configure_package_config_file(
    cmake/sqlite-backend-treeConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-treeConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-tree
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-treeConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-treeConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-treeConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-tree
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SQLITE_CPP_BUILD_BENCHMARKS)
    enable_testing()
    add_subdirectory(benchmarks)
endif()
