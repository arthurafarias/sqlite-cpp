cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-compiler-code-generator VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-compiler-code-generator test binary and register it with ctest" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()
sqlite_cpp_require_compiler_parser()
sqlite_cpp_require_core_virtual_machine()

# Header-only (SRS FR-3), same INTERFACE-library shape as the other
# sqlite-cpp libraries. SRS S3.1's dependency graph: depends on
# sqlite-core-virtual-machine (it emits VDBE opcodes) and on
# sqlite-compiler-parser's AST types -- this is the one library in the
# workspace so far that sits at the actual "compiler meets core" seam the
# graph describes.
add_library(sqlite-compiler-code-generator INTERFACE)
add_library(sqlite::compiler::code_generator ALIAS sqlite-compiler-code-generator)

target_compile_features(sqlite-compiler-code-generator INTERFACE cxx_std_20)
target_include_directories(sqlite-compiler-code-generator
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(sqlite-compiler-code-generator
    INTERFACE
        sqlite::utils
        sqlite::compiler::parser
        sqlite::core::virtual_machine
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-compiler-code-generator EXPORT sqlite-compiler-code-generatorTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-compiler-code-generatorTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-code-generator)
configure_package_config_file(
    cmake/sqlite-compiler-code-generatorConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-code-generatorConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-code-generator
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-code-generatorConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-code-generatorConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-compiler-code-generatorConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-compiler-code-generator
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
