# SRS 001 FR-4/FR-5: shared setup for every libraries/<name>/csrc-based library,
# and for linking an application against the full split.
#
# SQLITE_CPP_EXTRA_FEATURE_DEFS is the union of every FR-5 consumer's
# compile-time feature needs beyond the base SQLITE_FEATURE_DEFS: previously
# sqlite3-shell-legacy and sqlite3-rsync-legacy each got these by compiling
# their own private copy of the amalgamation with their own extra defines
# (see their CMakeLists.txt history); now that they link against the shared
# libraries instead (FR-5), those defines have to be baked into the shared
# libraries themselves, since a .so's behavior is fixed at its own compile
# time, not per-consumer. Applied only here, not to SQLITE_FEATURE_DEFS
# itself, so libsqlite3-legacy/testfixture (and the TCL suite's baseline)
# are unaffected. sqlite3-rsync-legacy's SQLITE_THREADSAFE=0 override does
# NOT appear here -- it directly conflicts with the workspace default
# (SQLITE_THREADSAFE=1) that every other consumer needs, and a single .so
# can't be both; per the requester's explicit choice (SRS 001 S6's FR-5
# note), rsync loses that single-threaded optimization rather than the
# shared libraries losing threading for everyone else. Its
# SQLITE_OMIT_LOAD_EXTENSION/SQLITE_OMIT_DEPRECATED overrides are dropped
# too, for the same reason, but harmlessly -- they only ever restricted
# what rsync's own translation unit could call, and the workspace default
# already permits both.
#
# SQLITE_ENABLE_FTS4 (which implies FTS3), SQLITE_ENABLE_RTREE, and
# SQLITE_ENABLE_STMTVTAB are deliberately NOT included, unlike the shell's
# original flag set: main.c references sqlite3Fts3Init/sqlite3RtreeInit/
# sqlite3StmtVtabInit unconditionally once those macros are defined (a
# static per-connection init-function array), and those functions live in
# ext/fts3/fts3.c, ext/rtree/rtree.c, and ext/misc/stmt.c respectively --
# extensions SRS 001 S3.6 explicitly defers, not part of any of the 9
# libraries. Defining the macro without the implementation is an unresolved
# symbol, not a missing feature toggle; shell/rsync lose FTS4/RTree/stmt-vtab
# support until extensions get their own pass, on top of the SQLITE_THREADSAFE
# tradeoff above.
set(SQLITE_CPP_EXTRA_FEATURE_DEFS
    SQLITE_DQS=0
    SQLITE_ENABLE_EXPLAIN_COMMENTS
    SQLITE_ENABLE_UNKNOWN_SQL_FUNCTION
    SQLITE_ENABLE_DBPAGE_VTAB
    SQLITE_ENABLE_DBSTAT_VTAB
    SQLITE_ENABLE_BYTECODE_VTAB
    SQLITE_ENABLE_OFFSET_SQL_FUNC
    SQLITE_STRICT_SUBTYPE=1
)

set(SQLITE_CPP_LIBRARY_NAMES
    utils backend-os backend-pager backend-tree
    core-virtual-machine core-command-processor core-interface
    compiler-tokenizer compiler-parser compiler-code-generator
)

# Each library compiles the C or C++ sources currently present in csrc/ into a
# SHARED object. This mixed-source discovery is what allows SRS 002 to advance
# one dependency leaf at a time. No
# target_link_libraries() is declared between sibling sqlite-cpp libraries --
# CMake refuses genuine target-level cycles between SHARED_LIBRARY targets
# ("Cyclic dependencies are allowed only among static libraries"), and the
# real C symbol graph *is* cyclic across the SRS 001 S1.3 boundaries (see
# that document's FR-4 status note): sqlite-utils itself reaches into
# Btree/Pager/Vdbe symbols nominally "above" it. Each .so is therefore built
# with unresolved symbols by design -- normal for a Linux .so, resolved at
# process load time once something (a test binary, an application) links
# against the whole set together. Real, encapsulated per-library link
# boundaries remain a private implementation constraint during the conversion.
include_guard(GLOBAL)

# Extra arguments are generated sources (for example compiler-parser's parse.c).
function(sqlite_cpp_add_library name)
    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION SQLITE_CPP_VERSION_STRING LIMIT_COUNT 1)
    string(STRIP "${SQLITE_CPP_VERSION_STRING}" SQLITE_CPP_VERSION_STRING)

    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteFeatures.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteCodegen.cmake)

    file(GLOB _sqlite_c_sources CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/csrc/*.c
    )
    file(GLOB _sqlite_cxx_sources CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/csrc/*.cpp
    )
    set(_sqlite_cpp_sources ${_sqlite_c_sources} ${_sqlite_cxx_sources} ${ARGN})

    add_library(sqlite-${name} SHARED ${_sqlite_cpp_sources})
    add_library(sqlite::${name} ALIAS sqlite-${name})

    set_target_properties(sqlite-${name} PROPERTIES
        OUTPUT_NAME sqlite-${name}
        POSITION_INDEPENDENT_CODE ON
        VERSION ${SQLITE_CPP_VERSION_STRING}
    )
    if(_sqlite_cxx_sources)
        set_target_properties(sqlite-${name} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS ON
        )
    else()
        set_target_properties(sqlite-${name} PROPERTIES C_STANDARD 11)
    endif()
    target_compile_definitions(sqlite-${name} PRIVATE ${SQLITE_FEATURE_DEFS} ${SQLITE_CPP_EXTRA_FEATURE_DEFS})
    if(_sqlite_cxx_sources)
        # The SRS 003 pass replaces SQLite's C allocation/data-structure
        # idioms. Until then, GNU C++ needs its permissive conversion mode.
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(sqlite-${name} PRIVATE -fpermissive)
        endif()
        # Unconverted C dependents still consume converted leaves' legacy
        # symbols. Keep that transitional linkage in one façade outside the
        # converted libraries; no converted source declares extern "C" itself.
        target_compile_options(sqlite-${name} PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/SqliteConvertedCFacade.hpp"
        )
    endif()
    target_include_directories(sqlite-${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/src
        ${SQLITE_GENDIR}
    )
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_include_directories(sqlite-${name} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        )
    endif()
    # Build-order only (generated headers like parse.h/opcodes.h/keywordhash.h
    # must exist before csrc files can be compiled) -- not a link dependency.
    add_dependencies(sqlite-${name} sqlite3_amalgamation)

    target_link_libraries(sqlite-${name} PRIVATE m ${CMAKE_DL_LIBS})
    if(CMAKE_USE_PTHREADS_INIT OR UNIX)
        find_package(Threads REQUIRED)
        target_link_libraries(sqlite-${name} PRIVATE Threads::Threads)
    endif()

    install(TARGETS sqlite-${name})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
            DESTINATION include
        )
    endif()
endfunction()

# SRS 001 FR-5: links an application against the full 10-library split instead
# of embedding the amalgamation or linking libsqlite3-legacy. -Wl,--no-as-needed
# is required, not cosmetic: an application typically only calls directly into
# sqlite-core-interface (e.g. sqlite3_open/sqlite3_exec), never referencing
# symbols from e.g. sqlite-compiler-tokenizer directly -- without this flag the
# linker would drop that .so's DT_NEEDED entry as unreferenced, and the
# process would fail to resolve it at load time even though core-interface's
# own code calls into it transitively (same load-time resolution FR-4 relies
# on, see cmake/SqliteCppLibrary.cmake's top comment).
function(sqlite_cpp_link_full_split target)
    foreach(_lib IN LISTS SQLITE_CPP_LIBRARY_NAMES)
        target_link_libraries(${target} PRIVATE sqlite::${_lib})
    endforeach()
    target_link_options(${target} PRIVATE "-Wl,--no-as-needed")
endfunction()
