cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_SHELL_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_SHELL_VERSION_STRING}" SQLITE_SHELL_VERSION_STRING)

# SRS 002 RR-2: renamed from `sqlite3-shell`. Its unsuffixed name is
# reserved (RR-3) for the eventual sqlite-cpp-linked replacement, gated on
# SRS 001's interface facade covering the shell's full dependency surface.
project(sqlite3-shell-legacy VERSION ${SQLITE_SHELL_VERSION_STRING} LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)

# The shell embeds sqlite3.c directly instead of linking libsqlite3-legacy
# (matching upstream main.mk, see the comment on its `sqlite3$(T.exe):` rule):
# this lets the CLI enable compile-time-only features, some with real
# performance cost, that the canonical library build does not carry. In the
# real build this compiles shell.c + sqlite3.c with $(T.link), which already
# carries the same $(OPT_FEATURE_FLAGS) as libsqlite3-legacy
# (SQLITE_FEATURE_DEFS here), plus $(SHELL_OPT) - the fixed extra list below
# - appended on top. So the shell gets both, not just the fixed list alone.
option(SQLITE_SHELL_ENABLE_READLINE "Link the shell against GNU readline for line editing" ON)

add_executable(sqlite3-shell-legacy ${SQLITE_SHELL_C} ${SQLITE_AMALGAMATION_C})
add_dependencies(sqlite3-shell-legacy sqlite3_shell_c sqlite3_amalgamation)
set_target_properties(sqlite3-shell-legacy PROPERTIES OUTPUT_NAME sqlite3-legacy C_STANDARD 11)
target_include_directories(sqlite3-shell-legacy PRIVATE ${SQLITE_GENDIR})
target_compile_definitions(sqlite3-shell-legacy PRIVATE
    ${SQLITE_FEATURE_DEFS}
    SQLITE_DQS=0
    SQLITE_ENABLE_FTS4
    SQLITE_ENABLE_RTREE
    SQLITE_ENABLE_EXPLAIN_COMMENTS
    SQLITE_ENABLE_UNKNOWN_SQL_FUNCTION
    SQLITE_ENABLE_STMTVTAB
    SQLITE_ENABLE_DBPAGE_VTAB
    SQLITE_ENABLE_DBSTAT_VTAB
    SQLITE_ENABLE_BYTECODE_VTAB
    SQLITE_ENABLE_OFFSET_SQL_FUNC
    SQLITE_ENABLE_PERCENTILE
    SQLITE_STRICT_SUBTYPE=1
)
target_link_libraries(sqlite3-shell-legacy PRIVATE m ${CMAKE_DL_LIBS} ${SQLITE_FEATURE_LIBS})
find_package(Threads REQUIRED)
target_link_libraries(sqlite3-shell-legacy PRIVATE Threads::Threads)

if(SQLITE_SHELL_ENABLE_READLINE)
    find_library(SQLITE_READLINE_LIBRARY readline)
    find_path(SQLITE_READLINE_INCLUDE_DIR readline/readline.h)
    if(SQLITE_READLINE_LIBRARY AND SQLITE_READLINE_INCLUDE_DIR)
        target_compile_definitions(sqlite3-shell-legacy PRIVATE HAVE_READLINE=1)
        target_include_directories(sqlite3-shell-legacy PRIVATE ${SQLITE_READLINE_INCLUDE_DIR})
        target_link_libraries(sqlite3-shell-legacy PRIVATE ${SQLITE_READLINE_LIBRARY})
    else()
        message(WARNING "SQLITE_SHELL_ENABLE_READLINE is ON but readline was not found; building the shell without line-editing support")
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS sqlite3-shell-legacy RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
