# Reproduces main.mk's code-generation pipeline (lemon-legacy -> parse.c/h,
# mkkeywordhash-legacy -> keywordhash.h, mkopcodeh-legacy/mkopcodec-legacy ->
# opcodes.h/c, mksqlite3h.tcl/mksqlite3c.tcl -> sqlite3.h/c, mkfts5c.tcl ->
# fts5.c, mkshellc.tcl -> shell.c) so that libraries/libsqlite3-legacy,
# applications/sqlite3-shell-legacy and tests/ can all consume the same
# generated sqlite3.{h,c}/shell.c without regenerating them.
#
# SRS 002 RR-4: the four generator applications (lemon, mkkeywordhash,
# mkopcodeh, mkopcodec) live under applications/*-legacy now, relocated from
# tool/ -- see the "Host tools" section below for how they're pulled in.
#
# Included once from the root CMakeLists.txt. Defines, in the caller's scope:
#   SQLITE_GENDIR             - directory holding every generated file below
#   SQLITE_AMALGAMATION_H     - path to generated sqlite3.h
#   SQLITE_AMALGAMATION_C     - path to generated sqlite3.c
#   SQLITE_AMALGAMATION_EXT_H - path to generated sqlite3ext.h
#   SQLITE_SHELL_C            - path to generated shell.c

# Target names created below (lemon-legacy, sqlite3_amalgamation, ...) are
# global to the whole build, so this guard must survive across
# add_subdirectory() scopes (each of which otherwise gets its own copy of a
# plain variable) - hence CACHE INTERNAL.
if(SQLITE_CODEGEN_INCLUDED)
    return()
endif()
set(SQLITE_CODEGEN_INCLUDED TRUE CACHE INTERNAL "")

find_program(TCLSH_EXECUTABLE NAMES tclsh tclsh8.6 tclsh8.7)
if(NOT TCLSH_EXECUTABLE)
    message(FATAL_ERROR "tclsh is required to generate sqlite3.c/sqlite3.h/shell.c (main.mk's own build requires it too)")
endif()

# Resolved relative to this file's own location (cmake/SqliteCodegen.cmake), not
# CMAKE_SOURCE_DIR, so it is correct whether this is include()'d from the workspace
# root or, via sqlite_require_core()'s add_subdirectory() fallback, from a standalone
# application build.
get_filename_component(SQLITE_TOP "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(SQLITE_GENDIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${SQLITE_GENDIR})

# ---------------------------------------------------------------------------
# Host tools: lemon-legacy (parser generator), mksourceid, mkkeywordhash-legacy.
# Built straight into SQLITE_GENDIR so the scripts below (which look for e.g.
# "$PWD/mksourceid") find them, exactly mirroring main.mk running everything
# from a single object directory.
#
# SRS 002 RR-4: lemon.c/mkkeywordhash.c themselves live under
# applications/lemon-legacy and applications/mkkeywordhash-legacy now (moved
# from tool/), each with its own standalone-buildable CMakeLists.txt. They're
# pulled in here via the same target-reuse-then-add_subdirectory-fallback
# idiom cmake/Sqlite(Cpp)Dependency.cmake uses elsewhere, so this file keeps
# working correctly regardless of which scope include()s it first (workspace
# root, or a standalone application build via sqlite_require_core()'s own
# fallback).
# ---------------------------------------------------------------------------
if(NOT TARGET lemon-legacy)
    add_subdirectory(${SQLITE_TOP}/applications/lemon-legacy ${CMAKE_BINARY_DIR}/lemon-legacy)
endif()
if(NOT TARGET mkkeywordhash-legacy)
    add_subdirectory(${SQLITE_TOP}/applications/mkkeywordhash-legacy ${CMAKE_BINARY_DIR}/mkkeywordhash-legacy)
endif()
add_executable(mksourceid ${SQLITE_TOP}/legacy/tool/mksourceid.c)
set_target_properties(lemon-legacy mksourceid mkkeywordhash-legacy PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${SQLITE_GENDIR}
)

add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/lempar.c
    COMMAND ${CMAKE_COMMAND} -E copy ${SQLITE_TOP}/applications/lemon-legacy/lempar.c ${SQLITE_GENDIR}/lempar.c
    DEPENDS ${SQLITE_TOP}/applications/lemon-legacy/lempar.c
)

# ---------------------------------------------------------------------------
# parse.c / parse.h  (lemon-legacy over src/parse.y)
# ---------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/parse.c ${SQLITE_GENDIR}/parse.h
    COMMAND ${CMAKE_COMMAND} -E copy ${SQLITE_TOP}/legacy/src/parse.y ${SQLITE_GENDIR}/parse.y
    COMMAND lemon-legacy -S parse.y
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS lemon-legacy ${SQLITE_GENDIR}/lempar.c ${SQLITE_TOP}/legacy/src/parse.y
    VERBATIM
)

# ---------------------------------------------------------------------------
# opcodes.h / opcodes.c
# ---------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/opcodes.h
    COMMAND /bin/sh -c "cat parse.h '${SQLITE_TOP}/legacy/src/vdbe.c' | '${TCLSH_EXECUTABLE}' '${SQLITE_TOP}/applications/mkopcodeh-legacy/mkopcodeh.tcl' > opcodes.h"
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS ${SQLITE_GENDIR}/parse.h ${SQLITE_TOP}/legacy/src/vdbe.c ${SQLITE_TOP}/applications/mkopcodeh-legacy/mkopcodeh.tcl
    VERBATIM
)
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/opcodes.c
    COMMAND /bin/sh -c "'${TCLSH_EXECUTABLE}' '${SQLITE_TOP}/applications/mkopcodec-legacy/mkopcodec.tcl' opcodes.h > opcodes.c"
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS ${SQLITE_GENDIR}/opcodes.h ${SQLITE_TOP}/applications/mkopcodec-legacy/mkopcodec.tcl
    VERBATIM
)

# ---------------------------------------------------------------------------
# keywordhash.h
# ---------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/keywordhash.h
    COMMAND /bin/sh -c "./mkkeywordhash-legacy > keywordhash.h"
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS mkkeywordhash-legacy
    VERBATIM
)

# ---------------------------------------------------------------------------
# pragma.h, ctime.c - each script self-locates $TOP from its own script path
# and writes its output file directly into the current working directory.
# ---------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/pragma.h
    COMMAND ${TCLSH_EXECUTABLE} ${SQLITE_TOP}/legacy/tool/mkpragmatab.tcl
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS ${SQLITE_TOP}/legacy/tool/mkpragmatab.tcl ${SQLITE_TOP}/legacy/src/pragma.c
    VERBATIM
)
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/ctime.c
    COMMAND ${TCLSH_EXECUTABLE} ${SQLITE_TOP}/legacy/tool/mkctimec.tcl
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS ${SQLITE_TOP}/legacy/tool/mkctimec.tcl
    VERBATIM
)

# ---------------------------------------------------------------------------
# sqlite3.h - mksqlite3h.tcl cd's into $TOP to run "$PWD/mksourceid manifest",
# where $PWD is captured before the cd, so mksourceid must already live in
# the working directory this command runs from (SQLITE_GENDIR). $TOP is
# passed as legacy/ (not the workspace root) because the script expects
# VERSION/manifest/manifest.tags colocated with src/ext -- SRS 001 FR-2
# copies those three small metadata files into legacy/ alongside src/ext/tool
# for exactly this reason; the workspace-root copies remain the ones every
# other CMakeLists.txt reads by relative path.
# ---------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/sqlite3.h
    COMMAND ${TCLSH_EXECUTABLE} ${SQLITE_TOP}/legacy/tool/mksqlite3h.tcl ${SQLITE_TOP}/legacy -o sqlite3.h
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS mksourceid ${SQLITE_TOP}/legacy/src/sqlite.h.in ${SQLITE_TOP}/legacy/manifest
            ${SQLITE_TOP}/legacy/VERSION ${SQLITE_TOP}/legacy/tool/mksqlite3h.tcl
    VERBATIM
)

# ---------------------------------------------------------------------------
# fts5parse.c/h + fts5.c/fts5.h
# ---------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/fts5parse.c ${SQLITE_GENDIR}/fts5parse.h
    COMMAND ${CMAKE_COMMAND} -E copy ${SQLITE_TOP}/legacy/ext/fts5/fts5parse.y ${SQLITE_GENDIR}/fts5parse.y
    COMMAND ${CMAKE_COMMAND} -E remove -f fts5parse.h
    COMMAND lemon-legacy -S fts5parse.y
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS lemon-legacy ${SQLITE_GENDIR}/lempar.c ${SQLITE_TOP}/legacy/ext/fts5/fts5parse.y
    VERBATIM
)

file(GLOB SQLITE_FTS5_SRC_FILES
    ${SQLITE_TOP}/legacy/ext/fts5/*.c
    ${SQLITE_TOP}/legacy/ext/fts5/*.h
)
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/fts5.c ${SQLITE_GENDIR}/fts5.h
    COMMAND ${TCLSH_EXECUTABLE} ${SQLITE_TOP}/legacy/ext/fts5/tool/mkfts5c.tcl
    COMMAND ${CMAKE_COMMAND} -E copy ${SQLITE_TOP}/legacy/ext/fts5/fts5.h ${SQLITE_GENDIR}/fts5.h
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS ${SQLITE_GENDIR}/fts5parse.c ${SQLITE_GENDIR}/fts5parse.h
            ${SQLITE_TOP}/legacy/ext/fts5/tool/mkfts5c.tcl ${SQLITE_FTS5_SRC_FILES}
    VERBATIM
)

# ---------------------------------------------------------------------------
# tsrc/ staging: main.mk's ".target_source" target flattens every core +
# extension source/header (main.mk's $(SRC) list) plus the generated files
# above into tsrc/, running vdbe.c through vdbe-compress.tcl on the way, so
# that mksqlite3c.tcl (which reads everything from a single --srcdir) can
# find them all by basename.
# ---------------------------------------------------------------------------
set(SQLITE_CORE_SRC_FILES
    alter.c analyze.c attach.c auth.c backup.c bitvec.c btmutex.c btree.c
    btree.h btreeInt.h build.c callback.c carray.c complete.c date.c
    dbpage.c dbstat.c delete.c expr.c fault.c fkey.c func.c global.c hash.c
    hash.h hwtime.h insert.c json.c legacy.c loadext.c main.c malloc.c
    mem0.c mem1.c mem2.c mem3.c mem5.c memdb.c memjournal.c msvc.h mutex.c
    mutex.h mutex_noop.c mutex_unix.c mutex_w32.c notify.c os.c os.h
    os_common.h os_setup.h os_kv.c os_unix.c os_win.c os_win.h pager.c
    pager.h pcache.c pcache.h pcache1.c pragma.c prepare.c printf.c
    random.c resolve.c rowset.c select.c status.c sqlite3ext.h sqliteInt.h
    sqliteLimit.h table.c threads.c tokenize.c treeview.c trigger.c utf.c
    update.c upsert.c util.c vacuum.c vdbe.h vdbeapi.c vdbeaux.c vdbeblob.c
    vdbemem.c vdbesort.c vdbetrace.c vdbevtab.c vdbeInt.h vtab.c vxworks.h
    wal.c wal.h walker.c where.c wherecode.c whereexpr.c whereInt.h window.c
)
list(TRANSFORM SQLITE_CORE_SRC_FILES PREPEND ${SQLITE_TOP}/legacy/src/ OUTPUT_VARIABLE SQLITE_CORE_SRC_PATHS)

set(SQLITE_EXT_SRC_FILES
    fts3/fts3.c fts3/fts3.h fts3/fts3Int.h fts3/fts3_aux.c fts3/fts3_expr.c
    fts3/fts3_hash.c fts3/fts3_hash.h fts3/fts3_icu.c fts3/fts3_porter.c
    fts3/fts3_snippet.c fts3/fts3_tokenizer.h fts3/fts3_tokenizer.c
    fts3/fts3_tokenizer1.c fts3/fts3_tokenize_vtab.c fts3/fts3_unicode.c
    fts3/fts3_unicode2.c fts3/fts3_write.c
    icu/sqliteicu.h icu/icu.c
    rtree/rtree.h rtree/rtree.c rtree/geopoly.c
    session/sqlite3session.c session/sqlite3session.h
    rbu/sqlite3rbu.h rbu/sqlite3rbu.c
    misc/stmt.c
)
list(TRANSFORM SQLITE_EXT_SRC_FILES PREPEND ${SQLITE_TOP}/legacy/ext/ OUTPUT_VARIABLE SQLITE_EXT_SRC_PATHS)

set(SQLITE_GENERATED_FLAT_FILES
    ${SQLITE_GENDIR}/ctime.c
    ${SQLITE_GENDIR}/pragma.h
    ${SQLITE_GENDIR}/keywordhash.h
    ${SQLITE_GENDIR}/opcodes.c
    ${SQLITE_GENDIR}/opcodes.h
    ${SQLITE_GENDIR}/parse.c
    ${SQLITE_GENDIR}/parse.h
    ${SQLITE_GENDIR}/sqlite3.h
)

set(SQLITE_TSRC_COPY_COMMANDS)
foreach(_src_path IN LISTS SQLITE_CORE_SRC_PATHS SQLITE_EXT_SRC_PATHS SQLITE_GENERATED_FLAT_FILES)
    list(APPEND SQLITE_TSRC_COPY_COMMANDS COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_src_path} ${SQLITE_GENDIR}/tsrc/)
endforeach()

add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/tsrc/.stamp
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${SQLITE_GENDIR}/tsrc
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SQLITE_GENDIR}/tsrc
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SQLITE_TOP}/legacy/src/vdbe.c ${SQLITE_GENDIR}/tsrc/vdbe.c
    ${SQLITE_TSRC_COPY_COMMANDS}
    COMMAND /bin/sh -c "'${TCLSH_EXECUTABLE}' '${SQLITE_TOP}/legacy/tool/vdbe-compress.tcl' < '${SQLITE_GENDIR}/tsrc/vdbe.c' > '${SQLITE_GENDIR}/tsrc/vdbe.c.new'"
    COMMAND ${CMAKE_COMMAND} -E rename ${SQLITE_GENDIR}/tsrc/vdbe.c.new ${SQLITE_GENDIR}/tsrc/vdbe.c
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SQLITE_GENDIR}/fts5.c ${SQLITE_GENDIR}/tsrc/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SQLITE_GENDIR}/fts5.h ${SQLITE_GENDIR}/tsrc/
    COMMAND ${CMAKE_COMMAND} -E touch ${SQLITE_GENDIR}/tsrc/.stamp
    DEPENDS ${SQLITE_CORE_SRC_PATHS} ${SQLITE_EXT_SRC_PATHS} ${SQLITE_GENERATED_FLAT_FILES}
            ${SQLITE_GENDIR}/fts5.c ${SQLITE_GENDIR}/fts5.h
            ${SQLITE_TOP}/legacy/tool/vdbe-compress.tcl ${SQLITE_TOP}/legacy/src/vdbe.c
    VERBATIM
)

# ---------------------------------------------------------------------------
# sqlite3.c - the amalgamation itself.
# ---------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/sqlite3.c ${SQLITE_GENDIR}/sqlite3ext.h
    COMMAND ${TCLSH_EXECUTABLE} ${SQLITE_TOP}/legacy/tool/mksqlite3c.tcl --linemacros=0
    COMMAND ${CMAKE_COMMAND} -E copy ${SQLITE_GENDIR}/tsrc/sqlite3ext.h ${SQLITE_GENDIR}/sqlite3ext.h
    COMMAND ${CMAKE_COMMAND} -E copy ${SQLITE_TOP}/legacy/ext/session/sqlite3session.h ${SQLITE_GENDIR}/sqlite3session.h
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS ${SQLITE_GENDIR}/tsrc/.stamp ${SQLITE_TOP}/legacy/tool/mksqlite3c.tcl
    VERBATIM
)

add_custom_target(sqlite3_amalgamation DEPENDS ${SQLITE_GENDIR}/sqlite3.c ${SQLITE_GENDIR}/sqlite3.h)

# ---------------------------------------------------------------------------
# shell.c - self-locates $TOP from its own script path, no staging needed.
# ---------------------------------------------------------------------------
set(SQLITE_SHELL_DEP_FILES
    src/shell.c.in
    ext/qrf/qrf.c ext/qrf/qrf.h
    ext/expert/sqlite3expert.c ext/expert/sqlite3expert.h
    ext/intck/sqlite3intck.c ext/intck/sqlite3intck.h
    ext/misc/appendvfs.c ext/misc/base64.c ext/misc/base85.c
    ext/misc/completion.c ext/misc/decimal.c ext/misc/fileio.c
    ext/misc/ieee754.c ext/misc/memtrace.c ext/misc/pcachetrace.c
    ext/misc/regexp.c ext/misc/series.c ext/misc/sha1.c ext/misc/shathree.c
    ext/misc/sqlar.c ext/misc/uint.c ext/misc/vfstrace.c
    ext/misc/windirent.h ext/misc/zipfile.c
    ext/recover/dbdata.c ext/recover/sqlite3recover.c ext/recover/sqlite3recover.h
)
list(TRANSFORM SQLITE_SHELL_DEP_FILES PREPEND ${SQLITE_TOP}/legacy/ OUTPUT_VARIABLE SQLITE_SHELL_DEP_PATHS)

add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/shell.c
    COMMAND ${TCLSH_EXECUTABLE} ${SQLITE_TOP}/legacy/tool/mkshellc.tcl shell.c
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS ${SQLITE_TOP}/legacy/tool/mkshellc.tcl ${SQLITE_SHELL_DEP_PATHS}
    VERBATIM
)
add_custom_target(sqlite3_shell_c DEPENDS ${SQLITE_GENDIR}/shell.c)

# ---------------------------------------------------------------------------
# tclsqlite-ex.c - testfixture's Tcl interface, combining qrf (the "quoted
# request filter" wrapper) with src/tclsqlite.c via mkcombo.tcl.
# ---------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${SQLITE_GENDIR}/tclsqlite-ex.c
    COMMAND ${TCLSH_EXECUTABLE} ${SQLITE_TOP}/legacy/tool/mkcombo.tcl
            ${SQLITE_TOP}/legacy/ext/qrf/qrf.h ${SQLITE_TOP}/legacy/ext/qrf/qrf.c ${SQLITE_TOP}/legacy/src/tclsqlite.c
            -o tclsqlite-ex.c
    WORKING_DIRECTORY ${SQLITE_GENDIR}
    DEPENDS ${SQLITE_TOP}/legacy/tool/mkcombo.tcl ${SQLITE_TOP}/legacy/ext/qrf/qrf.h
            ${SQLITE_TOP}/legacy/ext/qrf/qrf.c ${SQLITE_TOP}/legacy/src/tclsqlite.c
    VERBATIM
)
add_custom_target(sqlite3_tclsqlite_ex_c DEPENDS ${SQLITE_GENDIR}/tclsqlite-ex.c)

set(SQLITE_AMALGAMATION_H ${SQLITE_GENDIR}/sqlite3.h)
set(SQLITE_AMALGAMATION_C ${SQLITE_GENDIR}/sqlite3.c)
set(SQLITE_AMALGAMATION_EXT_H ${SQLITE_GENDIR}/sqlite3ext.h)
set(SQLITE_SHELL_C ${SQLITE_GENDIR}/shell.c)
set(SQLITE_TCLSQLITE_EX_C ${SQLITE_GENDIR}/tclsqlite-ex.c)
