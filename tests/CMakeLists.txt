cmake_minimum_required(VERSION 3.24)
project(sqlite3-tests LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/SqliteCodegen.cmake)

find_package(TCL REQUIRED)

# main.mk's $(TESTSRC): core src/test*.c harness files plus the "statically linked
# extensions" block (ext/misc/*, fts5 test helpers, expert/intck/recover/rbu/session
# test glue) that testfixture registers as Tcl test commands.
set(SQLITE_TESTFIXTURE_SRC_FILES
    src/test1.c src/test2.c src/test3.c src/test4.c src/test5.c src/test6.c
    src/test8.c src/test9.c src/test_autoext.c src/test_backup.c
    src/test_bestindex.c src/test_blob.c src/test_btree.c src/test_config.c
    src/test_delete.c src/test_demovfs.c src/test_devsym.c src/test_fs.c
    src/test_func.c src/test_hexio.c src/test_init.c src/test_intarray.c
    src/test_journal.c src/test_malloc.c src/test_md5.c src/test_multiplex.c
    src/test_mutex.c src/test_onefile.c src/test_osinst.c src/test_pcache.c
    src/test_quota.c src/test_rtree.c src/test_schema.c src/test_superlock.c
    src/test_syscall.c src/test_tclsh.c src/test_tclvar.c src/test_thread.c
    src/test_vdbecov.c src/test_vfs.c src/test_window.c src/test_wsd.c
    ext/fts3/fts3_term.c ext/fts3/fts3_test.c
    ext/session/test_session.c
    ext/recover/sqlite3recover.c ext/recover/dbdata.c ext/recover/test_recover.c
    ext/intck/test_intck.c ext/intck/sqlite3intck.c
    ext/rbu/test_rbu.c
    ext/expert/sqlite3expert.c ext/expert/test_expert.c
    ext/misc/amatch.c ext/misc/appendvfs.c ext/misc/basexx.c ext/misc/cksumvfs.c
    ext/misc/closure.c ext/misc/csv.c ext/misc/decimal.c ext/misc/eval.c
    ext/misc/explain.c ext/misc/fileio.c ext/misc/fuzzer.c
    ext/fts5/fts5_tcl.c ext/fts5/fts5_test_mi.c ext/fts5/fts5_test_tok.c
    ext/misc/ieee754.c ext/misc/mmapwarm.c ext/misc/nextchar.c
    ext/misc/normalize.c ext/misc/prefixes.c ext/misc/qpvtab.c
    ext/misc/randomjson.c ext/misc/regexp.c ext/misc/remember.c ext/misc/series.c
    ext/misc/spellfix.c ext/misc/stmtrand.c ext/misc/totype.c
    ext/misc/unionvtab.c ext/misc/wholenumber.c ext/misc/zipfile.c
    ext/rtree/test_rtreedoc.c
)
list(TRANSFORM SQLITE_TESTFIXTURE_SRC_FILES PREPEND ${SQLITE_TOP}/ OUTPUT_VARIABLE SQLITE_TESTFIXTURE_SRC_PATHS)

add_executable(testfixture
    ${SQLITE_TESTFIXTURE_SRC_PATHS}
    ${SQLITE_TCLSQLITE_EX_C}
    ${SQLITE_AMALGAMATION_C}
)
add_dependencies(testfixture sqlite3_tclsqlite_ex_c sqlite3_amalgamation)

target_include_directories(testfixture PRIVATE
    ${SQLITE_GENDIR}
    ${TCL_INCLUDE_PATH}
    ${SQLITE_TOP}/src
    ${SQLITE_TOP}/ext/rtree
    ${SQLITE_TOP}/ext/icu
    ${SQLITE_TOP}/ext/fts3
    ${SQLITE_TOP}/ext/fts5
    ${SQLITE_TOP}/ext/session
    ${SQLITE_TOP}/ext/misc
    ${SQLITE_TOP}/ext/expert
    ${SQLITE_TOP}/ext/intck
    ${SQLITE_TOP}/ext/recover
    ${SQLITE_TOP}/ext/rbu
    ${SQLITE_TOP}/ext/qrf
)

# Mirrors main.mk's $(TESTFIXTURE_FLAGS): testfixture inherits the same
# $(OPT_FEATURE_FLAGS)/SQLITE_FEATURE_DEFS as libsqlite3 (via the shared $(T.cc)
# base in the real build), plus this fixed set on top. -DSQLITE_PRIVATE="" is the
# significant one: it un-static's every "SQLITE_PRIVATE"-tagged symbol in the
# amalgamation so the TESTSRC files above can call directly into sqlite3 internals.
target_compile_definitions(testfixture PRIVATE
    ${SQLITE_FEATURE_DEFS}
    SQLITE_NO_SYNC=1
    SQLITE_TEST=1
    SQLITE_CRASH_TEST=1
    TCLSH_INIT_PROC=sqlite3TestInit
    SQLITE_SERVER=1
    SQLITE_PRIVATE=
    SQLITE_CORE
    SQLITE_SERIES_CONSTRAINT_VERIFY=1
    SQLITE_DEFAULT_PAGE_SIZE=1024
    SQLITE_ENABLE_STMTVTAB
    SQLITE_ENABLE_DBPAGE_VTAB
    SQLITE_ENABLE_BYTECODE_VTAB
    SQLITE_ENABLE_CARRAY
    SQLITE_ENABLE_PERCENTILE
    SQLITE_CKSUMVFS_STATIC
    SQLITE_STATIC_RANDOMJSON
    SQLITE_STRICT_SUBTYPE=1
)
target_link_libraries(testfixture PRIVATE ${TCL_LIBRARY} m ${CMAKE_DL_LIBS} ${SQLITE_FEATURE_LIBS})
find_package(Threads REQUIRED)
target_link_libraries(testfixture PRIVATE Threads::Threads)

find_program(TCLSH_TEST_EXECUTABLE NAMES tclsh tclsh8.6 tclsh8.7)

# The fast default gate: main.mk's own `tcltest` target is exactly
# `testfixture test/veryquick.test`.
add_test(
    NAME sqlite_veryquick
    COMMAND testfixture ${SQLITE_TOP}/test/veryquick.test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# The full suite (main.mk's `testrunner`/`fulltest` targets): not registered as a
# ctest test because, like in the pre-existing build, it is slow (1000+ .test
# files) - run manually via `cmake --build <dir> --target sqlite_fulltest`.
if(TCLSH_TEST_EXECUTABLE)
    add_custom_target(sqlite_fulltest
        COMMAND ${TCLSH_TEST_EXECUTABLE} ${SQLITE_TOP}/test/testrunner.tcl
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS testfixture
        COMMENT "Running the full SQLite TCL test suite via test/testrunner.tcl (slow)"
    )
endif()

# SRS 002 TR-4/BR-5: RR-6's freeze check, wired as its own CTest target
# (`ctest -R legacy_freeze_check` or `-L comparison`) separate from the
# ordinary oracle/unit tests above, so a freeze violation is distinguishable
# in CI output from a functional regression.
add_test(
    NAME legacy_freeze_check
    COMMAND ${SQLITE_TOP}/tool/check-legacy-freeze.sh
    WORKING_DIRECTORY ${SQLITE_TOP}
)
set_tests_properties(legacy_freeze_check PROPERTIES LABELS "comparison")

# SRS 002 RR-7/TR-1: generator-output parity between each *-legacy
# generator and its complete C++ replacement (RR-5), via
# tool/check-generator-parity.sh. mkopcodec-legacy/mkopcodeh-legacy are TCL
# scripts (no compiled binary, see their CMakeLists.txt), invoked directly
# via TCLSH_EXECUTABLE; mkkeywordhash-legacy and the three new-track
# executables are real targets, referenced via $<TARGET_FILE:...>.
# mkopcodeh/mkopcodec are fed the same already-generated parse.h/vdbe.c and
# opcodes.h the main codegen pipeline (cmake/SqliteCodegen.cmake) already
# produces for libsqlite3-legacy/testfixture -- no separate generation step
# needed.
add_test(
    NAME generator_parity_mkkeywordhash
    COMMAND ${SQLITE_TOP}/tool/check-generator-parity.sh
            "$<TARGET_FILE:mkkeywordhash-legacy>"
            "$<TARGET_FILE:mkkeywordhash>"
)
set_tests_properties(generator_parity_mkkeywordhash PROPERTIES LABELS "comparison")

add_test(
    NAME generator_parity_mkopcodec
    COMMAND ${SQLITE_TOP}/tool/check-generator-parity.sh
            "${TCLSH_EXECUTABLE} ${SQLITE_TOP}/applications/mkopcodec-legacy/mkopcodec.tcl ${SQLITE_GENDIR}/opcodes.h"
            "$<TARGET_FILE:mkopcodec> ${SQLITE_GENDIR}/opcodes.h"
)
set_tests_properties(generator_parity_mkopcodec PROPERTIES LABELS "comparison")

add_test(
    NAME generator_parity_mkopcodeh
    COMMAND ${SQLITE_TOP}/tool/check-generator-parity.sh
            "cat ${SQLITE_GENDIR}/parse.h ${SQLITE_TOP}/src/vdbe.c | ${TCLSH_EXECUTABLE} ${SQLITE_TOP}/applications/mkopcodeh-legacy/mkopcodeh.tcl"
            "cat ${SQLITE_GENDIR}/parse.h ${SQLITE_TOP}/src/vdbe.c | $<TARGET_FILE:mkopcodeh>"
)
set_tests_properties(generator_parity_mkopcodeh PROPERTIES LABELS "comparison")
