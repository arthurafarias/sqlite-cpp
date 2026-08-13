cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-backend-pager VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-backend-pager test binary and register it with ctest" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()
sqlite_cpp_require_backend_os()

# Header-only (SRS FR-3), same INTERFACE-library shape as the other
# sqlite-cpp libraries.
add_library(sqlite-backend-pager INTERFACE)
add_library(sqlite::backend::pager ALIAS sqlite-backend-pager)

target_compile_features(sqlite-backend-pager INTERFACE cxx_std_20)
target_include_directories(sqlite-backend-pager
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(sqlite-backend-pager INTERFACE sqlite::utils sqlite::backend::os)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-backend-pager EXPORT sqlite-backend-pagerTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-backend-pagerTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-pager)
configure_package_config_file(
    cmake/sqlite-backend-pagerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-pagerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-pager
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-pagerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-pagerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-backend-pagerConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-backend-pager
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
