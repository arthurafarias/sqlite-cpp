cmake_minimum_required(VERSION 3.24)
# SRS 002 RR-2: renamed from `sqlite3-rsync`. Its unsuffixed name is
# reserved (RR-3) for the eventual sqlite-cpp-linked replacement.
project(sqlite3-rsync-legacy LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)

# Mirrors main.mk's RSYNC_SRC/RSYNC_OPT: sqlite3_rsync embeds its own compile of the
# amalgamation (like the shell) rather than linking libsqlite3-legacy, built
# single-threaded with load-extension/deprecated APIs stripped. The
# THREADSAFE=0 override must come after SQLITE_FEATURE_DEFS in the
# definitions list below - for a macro defined multiple times via -D, the
# compiler uses the last one, matching main.mk's -USQLITE_THREADSAFE
# -DSQLITE_THREADSAFE=0 undef-then-redefine.
add_executable(sqlite3-rsync-legacy ${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/tool/sqlite3_rsync.c ${SQLITE_AMALGAMATION_C})
add_dependencies(sqlite3-rsync-legacy sqlite3_amalgamation)
target_include_directories(sqlite3-rsync-legacy PRIVATE ${SQLITE_GENDIR})
target_compile_definitions(sqlite3-rsync-legacy PRIVATE
    ${SQLITE_FEATURE_DEFS}
    SQLITE_ENABLE_DBPAGE_VTAB
    SQLITE_THREADSAFE=0
    SQLITE_OMIT_LOAD_EXTENSION
    SQLITE_OMIT_DEPRECATED
)
target_link_libraries(sqlite3-rsync-legacy PRIVATE m ${CMAKE_DL_LIBS} ${SQLITE_FEATURE_LIBS})

include(GNUInstallDirs)
install(TARGETS sqlite3-rsync-legacy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
