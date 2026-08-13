cmake_minimum_required(VERSION 3.24)

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_SHELL_VERSION_STRING LIMIT_COUNT 1)
string(STRIP "${SQLITE_SHELL_VERSION_STRING}" SQLITE_SHELL_VERSION_STRING)

# SRS 002 RR-2: renamed from `sqlite3-shell`. Its unsuffixed name is
# reserved (RR-3) for the eventual sqlite-cpp-linked replacement, gated on
# SRS 001's interface facade covering the shell's full dependency surface.
project(sqlite3-shell-legacy VERSION ${SQLITE_SHELL_VERSION_STRING} LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCppLibrary.cmake)

# SRS 001 FR-5: previously the shell embedded its own compile of sqlite3.c
# (matching upstream main.mk) to get compile-time-only features the
# canonical library build didn't carry. Those features are now baked into
# the shared libraries themselves (SQLITE_CPP_EXTRA_FEATURE_DEFS in
# cmake/SqliteCppLibrary.cmake) and the shell links against them (below)
# instead of compiling its own copy of the amalgamation.
option(SQLITE_SHELL_ENABLE_READLINE "Link the shell against GNU readline for line editing" ON)

add_executable(sqlite3-shell-legacy ${SQLITE_SHELL_C})
add_dependencies(sqlite3-shell-legacy sqlite3_shell_c sqlite3_amalgamation)
set_target_properties(sqlite3-shell-legacy PROPERTIES OUTPUT_NAME sqlite3-legacy C_STANDARD 11)
target_include_directories(sqlite3-shell-legacy PRIVATE ${SQLITE_GENDIR})
# shell.c itself branches on these macros (not just the library it calls into),
# so they still need to be defined for its own translation unit.
target_compile_definitions(sqlite3-shell-legacy PRIVATE
    ${SQLITE_FEATURE_DEFS}
    ${SQLITE_CPP_EXTRA_FEATURE_DEFS}
)
target_link_libraries(sqlite3-shell-legacy PRIVATE m ${CMAKE_DL_LIBS} ${SQLITE_FEATURE_LIBS})
sqlite_cpp_link_full_split(sqlite3-shell-legacy)
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
