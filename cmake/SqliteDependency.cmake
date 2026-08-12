# Resolves the sqlite3::sqlite3 target for a utility application under applications/.
#
# Resolution order:
#   1. reuse sqlite3::sqlite3 if an enclosing workspace build already defined it;
#   2. otherwise find_package() whatever sqlite3 is installed on the system, so a
#      utility application can be configured and built entirely on its own;
#   3. otherwise fall back to building the in-tree libraries/libsqlite3, so the
#      application still builds standalone on a machine with nothing installed.
function(sqlite_require_core)
    if(TARGET sqlite3::sqlite3)
        return()
    endif()

    find_package(sqlite3 CONFIG QUIET)
    if(TARGET sqlite3::sqlite3)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite3
        ${CMAKE_BINARY_DIR}/libsqlite3
    )
endfunction()
