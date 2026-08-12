cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_VERSION_STRING}" SQLITE_VERSION_STRING)

project(sqlite3 VERSION ${SQLITE_VERSION_STRING} LANGUAGES C)

option(SQLITE_BUILD_SHARED_LIBS "Build libsqlite3 as a shared library" ON)
option(SQLITE_BUILD_STATIC_LIBS "Build libsqlite3 as a static library" ON)

if(NOT SQLITE_BUILD_SHARED_LIBS AND NOT SQLITE_BUILD_STATIC_LIBS)
    message(FATAL_ERROR "At least one of SQLITE_BUILD_SHARED_LIBS / SQLITE_BUILD_STATIC_LIBS must be ON")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)

set(SQLITE_TARGETS)

if(SQLITE_BUILD_SHARED_LIBS)
    add_library(sqlite3 SHARED ${SQLITE_AMALGAMATION_C})
    add_library(sqlite3::sqlite3 ALIAS sqlite3)
    list(APPEND SQLITE_TARGETS sqlite3)
endif()
if(SQLITE_BUILD_STATIC_LIBS)
    add_library(sqlite3-static STATIC ${SQLITE_AMALGAMATION_C})
    add_library(sqlite3::sqlite3-static ALIAS sqlite3-static)
    list(APPEND SQLITE_TARGETS sqlite3-static)
    if(NOT SQLITE_BUILD_SHARED_LIBS)
        add_library(sqlite3::sqlite3 ALIAS sqlite3-static)
    endif()
endif()

foreach(_target IN LISTS SQLITE_TARGETS)
    add_dependencies(${_target} sqlite3_amalgamation)
    set_target_properties(${_target} PROPERTIES
        OUTPUT_NAME sqlite3
        C_STANDARD 11
        POSITION_INDEPENDENT_CODE ON
    )
    target_compile_definitions(${_target} PRIVATE ${SQLITE_FEATURE_DEFS})
    target_include_directories(${_target}
        PUBLIC
            $<BUILD_INTERFACE:${SQLITE_GENDIR}>
            $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(${_target} PRIVATE m ${CMAKE_DL_LIBS} ${SQLITE_FEATURE_LIBS})
    if(CMAKE_USE_PTHREADS_INIT OR UNIX)
        find_package(Threads REQUIRED)
        target_link_libraries(${_target} PRIVATE Threads::Threads)
    endif()
endforeach()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS ${SQLITE_TARGETS} EXPORT sqlite3Targets)
install(FILES ${SQLITE_GENDIR}/sqlite3.h ${SQLITE_GENDIR}/sqlite3ext.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT sqlite3Targets NAMESPACE sqlite3:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite3)
configure_package_config_file(
    cmake/sqlite3Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite3Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite3
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite3Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite3ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sqlite3
)

# pkg-config file: mirrors sqlite3.pc.in, but built from CMake's own install
# layout/link-library knowledge instead of autoconf substitution variables.
set(SQLITE_PC_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR})
set(SQLITE_PC_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})
set(SQLITE_PC_LIBS_PRIVATE "-lm")
if(SQLITE_FEATURE_PC_LIBS)
    string(REPLACE ";" " " _feature_libs_str "${SQLITE_FEATURE_PC_LIBS}")
    string(APPEND SQLITE_PC_LIBS_PRIVATE " ${_feature_libs_str}")
endif()
if(CMAKE_DL_LIBS)
    string(APPEND SQLITE_PC_LIBS_PRIVATE " -l${CMAKE_DL_LIBS}")
endif()
string(APPEND SQLITE_PC_LIBS_PRIVATE " -lpthread")
configure_file(
    cmake/sqlite3.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/sqlite3.pc
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sqlite3.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../sqlite3.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)
