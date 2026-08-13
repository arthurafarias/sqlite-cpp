# Resolves the libsqlite3-legacy::libsqlite3-legacy target for a utility
# application under applications/. SRS 002 RR-1: this used to resolve
# sqlite3::sqlite3 / libraries/libsqlite3 before the legacy rename.
#
# Resolution order:
#   1. reuse libsqlite3-legacy::libsqlite3-legacy if an enclosing workspace
#      build already defined it;
#   2. otherwise find_package() whatever libsqlite3-legacy is installed on the
#      system, so a utility application can be configured and built entirely
#      on its own;
#   3. otherwise fall back to building the in-tree libraries/libsqlite3-legacy,
#      so the application still builds standalone on a machine with nothing
#      installed.
function(sqlite_require_core)
    if(TARGET libsqlite3-legacy::libsqlite3-legacy)
        return()
    endif()

    find_package(libsqlite3-legacy CONFIG QUIET)
    if(TARGET libsqlite3-legacy::libsqlite3-legacy)
        return()
    endif()

    add_subdirectory(
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../libraries/libsqlite3-legacy
        ${CMAKE_BINARY_DIR}/libsqlite3-legacy
    )
endfunction()
