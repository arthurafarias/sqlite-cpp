cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

project(sqlite-core-virtual-machine VERSION ${SQLITE_CPP_VERSION_STRING} LANGUAGES CXX)

option(SQLITE_CPP_BUILD_TESTS "Build the sqlite-core-virtual-machine test binary and register it with ctest" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_utils()

# Header-only (SRS FR-3), same INTERFACE-library shape as sqlite-utils and
# sqlite-backend-os. Does NOT link sqlite::backend::os yet: this phase's
# opcode subset is self-contained (arithmetic/comparison/control-flow/row
# output registers only, per the SRS phase-3 scoping note in this library's
# docs) and has no database-file-touching opcodes (OP_OpenRead et al.) to
# justify the dependency yet. The dependency edge from S3.1's architecture
# diagram (sqlite-core depends on sqlite-backend) will be wired in when a
# later pass adds opcodes that actually call into it.
add_library(sqlite-core-virtual-machine INTERFACE)
add_library(sqlite::core::virtual_machine ALIAS sqlite-core-virtual-machine)

target_compile_features(sqlite-core-virtual-machine INTERFACE cxx_std_20)
target_include_directories(sqlite-core-virtual-machine
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(sqlite-core-virtual-machine INTERFACE sqlite::utils)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS sqlite-core-virtual-machine EXPORT sqlite-core-virtual-machineTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} PATTERN "testing" EXCLUDE)
install(EXPORT sqlite-core-virtual-machineTargets NAMESPACE sqlite:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-core-virtual-machine)
configure_package_config_file(
    cmake/sqlite-core-virtual-machineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-core-virtual-machineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-core-virtual-machine
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-core-virtual-machineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-core-virtual-machineConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite-core-virtual-machineConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite-core-virtual-machine
)

if(SQLITE_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
