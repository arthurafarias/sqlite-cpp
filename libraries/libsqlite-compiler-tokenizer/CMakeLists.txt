cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-compiler-tokenizer VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-compiler-tokenizer test binary and register it with ctest" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()

# Header-only (SRS FR-3), same INTERFACE-library shape as the other
# sqlite-cpp libraries.
add_library(sqlite-compiler-tokenizer INTERFACE)
add_library(sqlite::compiler::tokenizer ALIAS sqlite-compiler-tokenizer)

target_compile_features(sqlite-compiler-tokenizer INTERFACE cxx_std_20)
target_include_directories(sqlite-compiler-tokenizer
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
# Linked for the shared test harness (sqlite::utils::testing) this library's
# own tests use, mirroring sqlite-core-virtual-machine's CMakeLists.txt --
# no header under include/sqlite/compiler/tokenizer/ (excluding testing/)
# actually includes anything from sqlite::utils.
target_link_libraries(sqlite-compiler-tokenizer INTERFACE sqlite::utils)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-compiler-tokenizer EXPORT sqlite-compiler-tokenizerTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-compiler-tokenizerTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-tokenizer)
configure_package_config_file(
    cmake/sqlite-compiler-tokenizerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-tokenizerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-tokenizer
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-tokenizerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-tokenizerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-tokenizerConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-tokenizer
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
