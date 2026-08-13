# SRS 001 FR-4: shared setup for every libraries/<name>/csrc-based library.
#
# Each library compiles its own csrc/*.c into a SHARED object. No
# target_link_libraries() is declared between sibling sqlite-cpp libraries --
# CMake refuses genuine target-level cycles between SHARED_LIBRARY targets
# ("Cyclic dependencies are allowed only among static libraries"), and the
# real C symbol graph *is* cyclic across the SRS 001 S1.3 boundaries (see
# that document's FR-4 status note): sqlite-utils itself reaches into
# Btree/Pager/Vdbe symbols nominally "above" it. Each .so is therefore built
# with unresolved symbols by design -- normal for a Linux .so, resolved at
# process load time once something (a test binary, an application) links
# against the whole set together. Real, encapsulated per-library link
# boundaries are SRS 002's job, once each library has its own namespaced
# C++ API instead of free functions sharing one global sqliteInt.h.
include_guard(GLOBAL)

# Extra arguments are additional generated source files (e.g. compiler-parser's
# ${SQLITE_GENDIR}/parse.c, produced by lemon -- there's no static parse.c to
# glob, only the parse.y grammar source csrc/ actually holds).
function(sqlite_cpp_add_library name)
    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
    string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteFeatures.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)

    file(GLOB _sqlite_cpp_sources ${CMAKE_CURRENT_SOURCE_DIR}/csrc/*.c)
    list(APPEND _sqlite_cpp_sources ${ARGN})

    add_library(sqlite-${name} SHARED ${_sqlite_cpp_sources})
    add_library(sqlite::${name} ALIAS sqlite-${name})

    set_target_properties(sqlite-${name} PROPERTIES
        OUTPUT_NAME sqlite-${name}
        C_STANDARD 11
        POSITION_INDEPENDENT_CODE ON
        VERSION ${SQLITE_CPP_VERSION_STRING}
    )
    target_compile_definitions(sqlite-${name} PRIVATE ${SQLITE_FEATURE_DEFS})
    target_include_directories(sqlite-${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/src
        ${SQLITE_GENDIR}
    )
    # Build-order only (generated headers like parse.h/opcodes.h/keywordhash.h
    # must exist before csrc/*.c can be compiled) -- not a link dependency.
    add_dependencies(sqlite-${name} sqlite3_amalgamation)

    target_link_libraries(sqlite-${name} PRIVATE m ${CMAKE_DL_LIBS})
    if(CMAKE_USE_PTHREADS_INIT OR UNIX)
        find_package(Threads REQUIRED)
        target_link_libraries(sqlite-${name} PRIVATE Threads::Threads)
    endif()

    install(TARGETS sqlite-${name})
endfunction()
