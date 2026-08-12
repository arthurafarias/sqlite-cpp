cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-utils VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-utils test binary and register it with ctest" ON)

# sqlite::utils is header-only (SRS FR-3): an INTERFACE library, no compiled
# translation unit of its own. Every function under include/ is `inline`,
# which is what makes multiple translation units including these headers
# ODR-safe (SRS S11.1) -- it is not a request for forced call-site inlining.
add_library(sqlite-utils INTERFACE)
add_library(sqlite::utils ALIAS sqlite-utils)

target_compile_features(sqlite-utils INTERFACE cxx_std_20)
target_include_directories(sqlite-utils
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

find_package(Threads REQUIRED)
target_link_libraries(sqlite-utils INTERFACE Threads::Threads)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-utils EXPORT sqlite-utilsTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-utilsTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-utils)
configure_package_config_file(
    cmake/sqlite-utilsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-utilsConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-utils
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-utilsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-utilsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-utilsConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-utils
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
