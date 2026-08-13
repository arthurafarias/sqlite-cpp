# SRS FR-15: one sqlite_cpp_require_<library>() function per sqlite-cpp
# library, mirroring cmake/SqliteDependency.cmake's sqlite_require_core() and
# opheap's OpheapDependency.cmake. Any consumer (an application, a test
# binary, another library) can depend on a library whether it's built
# in-workspace, resolved via an installed find_package(), or -- if neither
# applies, e.g. this library is being used completely standalone -- pulled in
# via an add_subdirectory() fallback.

# Resolves the sqlite::utils target.
#
# Resolution order:
#   1. reuse sqlite::utils if an enclosing workspace build already defined it;
#   2. otherwise find_package() whatever sqlite-utils is installed on the
#      system, so a consumer can be configured and built entirely on its own;
#   3. otherwise fall back to building the in-tree libraries/libsqlite-utils,
#      so the consumer still builds standalone on a machine with nothing
#      installed.
function(sqlite_cpp_require_utils)
    if(TARGET sqlite::utils)
        return()
    endif()

    find_package(sqlite-utils CONFIG QUIET)
    if(TARGET sqlite::utils)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite-utils
        ${CMAKE_BINARY_DIR}/libsqlite-utils
    )
endfunction()

# Resolves the sqlite::backend::os target. Mirrors sqlite_cpp_require_utils().
function(sqlite_cpp_require_backend_os)
    if(TARGET sqlite::backend::os)
        return()
    endif()

    find_package(sqlite-backend-os CONFIG QUIET)
    if(TARGET sqlite::backend::os)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite-backend-os
        ${CMAKE_BINARY_DIR}/libsqlite-backend-os
    )
endfunction()

# Resolves the sqlite::core::virtual_machine target. Mirrors sqlite_cpp_require_utils().
function(sqlite_cpp_require_core_virtual_machine)
    if(TARGET sqlite::core::virtual_machine)
        return()
    endif()

    find_package(sqlite-core-virtual-machine CONFIG QUIET)
    if(TARGET sqlite::core::virtual_machine)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite-core-virtual-machine
        ${CMAKE_BINARY_DIR}/libsqlite-core-virtual-machine
    )
endfunction()

# Resolves the sqlite::compiler::tokenizer target. Mirrors sqlite_cpp_require_utils().
function(sqlite_cpp_require_compiler_tokenizer)
    if(TARGET sqlite::compiler::tokenizer)
        return()
    endif()

    find_package(sqlite-compiler-tokenizer CONFIG QUIET)
    if(TARGET sqlite::compiler::tokenizer)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite-compiler-tokenizer
        ${CMAKE_BINARY_DIR}/libsqlite-compiler-tokenizer
    )
endfunction()

# Resolves the sqlite::compiler::parser target. Mirrors sqlite_cpp_require_utils().
function(sqlite_cpp_require_compiler_parser)
    if(TARGET sqlite::compiler::parser)
        return()
    endif()

    find_package(sqlite-compiler-parser CONFIG QUIET)
    if(TARGET sqlite::compiler::parser)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite-compiler-parser
        ${CMAKE_BINARY_DIR}/libsqlite-compiler-parser
    )
endfunction()

# Resolves the sqlite::compiler::code_generator target. Mirrors sqlite_cpp_require_utils().
function(sqlite_cpp_require_compiler_code_generator)
    if(TARGET sqlite::compiler::code_generator)
        return()
    endif()

    find_package(sqlite-compiler-code-generator CONFIG QUIET)
    if(TARGET sqlite::compiler::code_generator)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite-compiler-code-generator
        ${CMAKE_BINARY_DIR}/libsqlite-compiler-code-generator
    )
endfunction()

# Resolves the sqlite::backend::tree target. Mirrors sqlite_cpp_require_utils().
function(sqlite_cpp_require_backend_tree)
    if(TARGET sqlite::backend::tree)
        return()
    endif()

    find_package(sqlite-backend-tree CONFIG QUIET)
    if(TARGET sqlite::backend::tree)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite-backend-tree
        ${CMAKE_BINARY_DIR}/libsqlite-backend-tree
    )
endfunction()

# Resolves the sqlite::backend::pager target. Mirrors sqlite_cpp_require_utils().
function(sqlite_cpp_require_backend_pager)
    if(TARGET sqlite::backend::pager)
        return()
    endif()

    find_package(sqlite-backend-pager CONFIG QUIET)
    if(TARGET sqlite::backend::pager)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite-backend-pager
        ${CMAKE_BINARY_DIR}/libsqlite-backend-pager
    )
endfunction()
