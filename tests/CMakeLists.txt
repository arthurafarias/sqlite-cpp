cmake_minimum_required(VERSION 3.24)
project(sqlite3-tests LANGUAGES C CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/SqliteCodegen.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/SqliteCppLibrary.cmake)

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
list(TRANSFORM SQLITE_TESTFIXTURE_SRC_FILES PREPEND ${SQLITE_TOP}/legacy/ OUTPUT_VARIABLE SQLITE_TESTFIXTURE_SRC_PATHS)

add_executable(testfixture
    ${SQLITE_TESTFIXTURE_SRC_PATHS}
    ${SQLITE_TCLSQLITE_EX_C}
    ${SQLITE_AMALGAMATION_C}
)
add_dependencies(testfixture sqlite3_tclsqlite_ex_c sqlite3_amalgamation)

target_include_directories(testfixture PRIVATE
    ${SQLITE_GENDIR}
    ${TCL_INCLUDE_PATH}
    ${SQLITE_TOP}/legacy/src
    ${SQLITE_TOP}/legacy/ext/rtree
    ${SQLITE_TOP}/legacy/ext/icu
    ${SQLITE_TOP}/legacy/ext/fts3
    ${SQLITE_TOP}/legacy/ext/fts5
    ${SQLITE_TOP}/legacy/ext/session
    ${SQLITE_TOP}/legacy/ext/misc
    ${SQLITE_TOP}/legacy/ext/expert
    ${SQLITE_TOP}/legacy/ext/intck
    ${SQLITE_TOP}/legacy/ext/recover
    ${SQLITE_TOP}/legacy/ext/rbu
    ${SQLITE_TOP}/legacy/ext/qrf
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

add_executable(srs002-utils-smoke srs002_utils_smoke.cpp)
set_target_properties(srs002-utils-smoke PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(srs002-utils-smoke PRIVATE sqlite::utils)
sqlite_cpp_link_full_split(srs002-utils-smoke)
add_test(NAME srs002_utils_namespaced_api COMMAND srs002-utils-smoke)

add_executable(srs002-backend-os-smoke srs002_backend_os_smoke.cpp)
set_target_properties(srs002-backend-os-smoke PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(srs002-backend-os-smoke PRIVATE sqlite::backend-os)
sqlite_cpp_link_full_split(srs002-backend-os-smoke)
add_test(NAME srs002_backend_os_namespaced_api COMMAND srs002-backend-os-smoke)

add_executable(srs002-backend-pager-smoke srs002_backend_pager_smoke.cpp)
set_target_properties(srs002-backend-pager-smoke PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(srs002-backend-pager-smoke PRIVATE sqlite::backend-pager)
sqlite_cpp_link_full_split(srs002-backend-pager-smoke)
add_test(NAME srs002_backend_pager_namespaced_api COMMAND srs002-backend-pager-smoke)

add_executable(srs002-backend-tree-smoke srs002_backend_tree_smoke.cpp)
set_target_properties(srs002-backend-tree-smoke PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(srs002-backend-tree-smoke PRIVATE sqlite::backend-tree)
sqlite_cpp_link_full_split(srs002-backend-tree-smoke)
add_test(NAME srs002_backend_tree_namespaced_api COMMAND srs002-backend-tree-smoke)

add_executable(srs002-core-virtual-machine-smoke srs002_core_virtual_machine_smoke.cpp)
set_target_properties(srs002-core-virtual-machine-smoke PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(srs002-core-virtual-machine-smoke PRIVATE sqlite::core-virtual-machine)
sqlite_cpp_link_full_split(srs002-core-virtual-machine-smoke)
add_test(NAME srs002_core_virtual_machine_namespaced_api COMMAND srs002-core-virtual-machine-smoke)

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
