cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-compiler-parser VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-compiler-parser test binary and register it with ctest" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()
sqlite_cpp_require_compiler_tokenizer()

# Header-only (SRS FR-3), same INTERFACE-library shape as the other
# sqlite-cpp libraries.
add_library(sqlite-compiler-parser INTERFACE)
add_library(sqlite::compiler::parser ALIAS sqlite-compiler-parser)

target_compile_features(sqlite-compiler-parser INTERFACE cxx_std_20)
target_include_directories(sqlite-compiler-parser
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(sqlite-compiler-parser INTERFACE sqlite::utils sqlite::compiler::tokenizer)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-compiler-parser EXPORT sqlite-compiler-parserTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-compiler-parserTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-parser)
configure_package_config_file(
    cmake/sqlite-compiler-parserConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-parserConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-parser
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-parserConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-parserConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-parserConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-parser
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
