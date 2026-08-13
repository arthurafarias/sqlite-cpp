cmake_minimum_required(VERSION 3.24)
# C is needed here (not just in the per-target subdirectories) because
# cmake/SqliteCodegen.cmake's host tools (lemon, mksourceid, mkkeywordhash) are
# add_executable()'d from this scope, before any subdirectory's own project() runs.
# CXX is needed for the sqlite-cpp libraries under libraries/libsqlite-utils
# and its future siblings (SRS S3), which live alongside the original C
# sources/build rather than replacing them (SRS S2.5).
project(sqlite-workspace LANGUAGES C CXX)

option(SQLITE_BUILD_SHELL "Build the sqlite3 CLI shell" ON)
option(SQLITE_BUILD_TESTS "Build testfixture and register the TCL test suite with ctest" ON)
option(SQLITE_BUILD_BENCHMARKS "Build speedtest1" ON)
option(SQLITE_BUILD_APPLICATIONS "Build sqldiff and sqlite3-rsync" ON)
option(SQLITE_CPP_BUILD_UTILS "Build the sqlite-cpp sqlite::utils library (SRS phase 1)" ON)
option(SQLITE_CPP_BUILD_BACKEND_OS "Build the sqlite-cpp sqlite::backend::os library (SRS phase 2)" ON)
option(SQLITE_CPP_BUILD_CORE_VIRTUAL_MACHINE "Build the sqlite-cpp sqlite::core::virtual_machine library (SRS phase 3)" ON)
option(SQLITE_CPP_BUILD_COMPILER_TOKENIZER "Build the sqlite-cpp sqlite::compiler::tokenizer library (SRS phase 4, part 1)" ON)
option(SQLITE_CPP_BUILD_COMPILER_PARSER "Build the sqlite-cpp sqlite::compiler::parser library (SRS phase 4, part 1)" ON)
option(SQLITE_CPP_BUILD_COMPILER_CODE_GENERATOR "Build the sqlite-cpp sqlite::compiler::code_generator library (SRS phase 4, code-generator slice)" ON)
option(SQLITE_CPP_BUILD_BACKEND_TREE "Build the sqlite-cpp sqlite::backend::tree library (SRS phase 2, tree slice)" ON)

# Called unconditionally (rather than only under SQLITE_BUILD_TESTS below) so
# that libraries/libsqlite-utils's own tests/CMakeLists.txt -- added via
# sqlite_cpp_require_utils() before the SQLITE_BUILD_TESTS block runs -- gets
# a root CTestTestfile.cmake to register into; enable_testing() is otherwise
# a no-op declaration and calling it twice is harmless.
enable_testing()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteFeatures.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteCodegen.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqliteCppDependency.cmake)

add_subdirectory(libraries/libsqlite3)

# SRS phase 1 (S12): sqlite-utils is the lowest-dependency sqlite-cpp
# library, added alongside (not replacing) the legacy libsqlite3 target above.
if(SQLITE_CPP_BUILD_UTILS)
    sqlite_cpp_require_utils()
endif()

# SRS phase 2 (S12): sqlite-backend-os, the FR-6 paradigm case (unix/Windows
# selected structurally -- see libraries/libsqlite-backend-os/docs/index.md
# for how that adapts to a header-only library).
if(SQLITE_CPP_BUILD_BACKEND_OS)
    sqlite_cpp_require_backend_os()
endif()

# SRS phase 3 (S12): sqlite-core-virtual-machine, the runtime half of the VDBE
# only -- see libraries/libsqlite-core-virtual-machine/docs/index.md for this
# phase's scoping (a self-contained VM exercised by hand-assembled programs,
# since the compiler that would otherwise generate its bytecode doesn't exist
# yet).
if(SQLITE_CPP_BUILD_CORE_VIRTUAL_MACHINE)
    sqlite_cpp_require_core_virtual_machine()
endif()

# SRS phase 4, part 1 (S12): sqlite-compiler-tokenizer -- "tokenizer and
# parser first (lower risk)", per the SRS's own note that phase 4's
# code-generator/command-processor half needs its own separate design
# review before starting (S11.2). Parser, code-generator, and
# command-processor remain unstarted.
if(SQLITE_CPP_BUILD_COMPILER_TOKENIZER)
    sqlite_cpp_require_compiler_tokenizer()
endif()

# SRS phase 4, part 1 (S12): sqlite-compiler-parser -- a hand-written
# recursive-descent/precedence-climbing parser (not a port of the generated
# LALR tables) for expressions and single SELECT statements. See
# libraries/libsqlite-compiler-parser/docs/index.md for what's deferred
# (INSERT/UPDATE/DELETE, CREATE TABLE/INDEX/VIEW/TRIGGER, ALTER, compound
# SELECT, CTEs, subqueries, window functions) and why.
if(SQLITE_CPP_BUILD_COMPILER_PARSER)
    sqlite_cpp_require_compiler_parser()
endif()

# SRS phase 4, code-generator slice (S12, S11.2): sqlite-compiler-code-generator
# -- the AST-to-bytecode seam S11.2 flags as the refactor's highest-risk
# piece. Scoped to compiling expressions (not full statements -- see
# libraries/libsqlite-compiler-code-generator/docs/index.md) using the
# opcode set sqlite-core-virtual-machine currently implements.
if(SQLITE_CPP_BUILD_COMPILER_CODE_GENERATOR)
    sqlite_cpp_require_compiler_code_generator()
endif()

# SRS phase 2, tree slice (S12): sqlite-backend-tree -- a real table B-tree
# over the actual SQLite on-disk page format, with real multi-page
# split-based balancing (see libraries/libsqlite-backend-tree/docs/index.md
# for the deliberate simplifications: 2-way splits rather than
# balance_nonroot()'s 3-sibling redistribution, no merge-on-delete, and a
# minimal page_store standing in for the not-yet-built sqlite-backend-pager).
if(SQLITE_CPP_BUILD_BACKEND_TREE)
    sqlite_cpp_require_backend_tree()
endif()

if(SQLITE_BUILD_SHELL)
    add_subdirectory(applications/sqlite3-shell)
endif()
if(SQLITE_BUILD_APPLICATIONS)
    add_subdirectory(applications/sqldiff)
    add_subdirectory(applications/sqlite3-rsync)
endif()
if(SQLITE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(SQLITE_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
