cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-2: renamed from `sqldiff`. Its unsuffixed name is reserved
# (RR-3) for the eventual sqlite-cpp-linked replacement.
project(sqldiff-legacy LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppLibrary.cmake)

# SRS 001 FR-5: previously linked libsqlite3-legacy (cmake/SqliteDependency.cmake's
# sqlite_require_core()); now links against the shared libraries instead.
add_executable(sqldiff-legacy ${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/tool/sqldiff.c)
add_dependencies(sqldiff-legacy sqlite3_amalgamation)
target_include_directories(sqldiff-legacy PRIVATE
    ${SQLITE_GENDIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/ext/misc
)
sqlite_cpp_link_full_split(sqldiff-legacy)

include(GNUInstallDirs)
install(TARGETS sqldiff-legacy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
