cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-core-command-processor VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-core-command-processor test binary and register it with ctest" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()
sqlite_cpp_require_compiler_tokenizer()
sqlite_cpp_require_compiler_parser()
sqlite_cpp_require_compiler_code_generator()
sqlite_cpp_require_core_virtual_machine()
sqlite_cpp_require_backend_tree()

# Header-only (SRS FR-3), same INTERFACE-library shape as the other
# sqlite-cpp libraries. SRS S3.1's dependency graph:
# sqlite-core-command-processor depends on sqlite-compiler (it drives the
# parser's AST and the code generator) and on sqlite-backend ("it drives
# execution") -- this phase covers only the `query` namespace (single-table
# SELECT orchestration, against either an in-memory table_descriptor or a
# real sqlite-backend-tree-backed btree_table_descriptor); see docs/index.md
# for exactly what's in scope and what's deferred.
add_library(sqlite-core-command-processor INTERFACE)
add_library(sqlite::core::command_processor ALIAS sqlite-core-command-processor)

target_compile_features(sqlite-core-command-processor INTERFACE cxx_std_20)
target_include_directories(sqlite-core-command-processor
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(sqlite-core-command-processor
    INTERFACE
        sqlite::utils
        sqlite::compiler::tokenizer
        sqlite::compiler::parser
        sqlite::compiler::code_generator
        sqlite::core::virtual_machine
        sqlite::backend::tree
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-core-command-processor EXPORT sqlite-core-command-processorTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-core-command-processorTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-core-command-processor)
configure_package_config_file(
    cmake/sqlite-core-command-processorConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-core-command-processorConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-core-command-processor
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-core-command-processorConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-core-command-processorConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-core-command-processorConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-core-command-processor
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
