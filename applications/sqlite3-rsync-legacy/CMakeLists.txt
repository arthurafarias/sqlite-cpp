cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-2: renamed from `sqlite3-rsync`. Its unsuffixed name is
# reserved (RR-3) for the eventual sqlite-cpp-linked replacement.
project(sqlite3-rsync-legacy LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppLibrary.cmake)

# SRS 001 FR-5: previously embedded its own compile of the amalgamation
# (single-threaded, load-extension/deprecated APIs stripped) instead of
# linking libsqlite3-legacy. Now links against the shared libraries instead.
# The single-threaded build (SQLITE_THREADSAFE=0) is dropped, not carried
# forward: it directly conflicts with the workspace default every other
# consumer of the shared libraries needs, and a single .so can't be both --
# see cmake/SqliteCppLibrary.cmake's SQLITE_CPP_EXTRA_FEATURE_DEFS comment
# for the tradeoff this accepts (an explicit choice, not an oversight).
add_executable(sqlite3-rsync-legacy ${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/tool/sqlite3_rsync.c)
add_dependencies(sqlite3-rsync-legacy sqlite3_amalgamation)
target_include_directories(sqlite3-rsync-legacy PRIVATE ${SQLITE_GENDIR})
target_compile_definitions(sqlite3-rsync-legacy PRIVATE ${SQLITE_FEATURE_DEFS} ${SQLITE_CPP_EXTRA_FEATURE_DEFS})
target_link_libraries(sqlite3-rsync-legacy PRIVATE m ${CMAKE_DL_LIBS} ${SQLITE_FEATURE_LIBS})
sqlite_cpp_link_full_split(sqlite3-rsync-legacy)

include(GNUInstallDirs)
install(TARGETS sqlite3-rsync-legacy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
