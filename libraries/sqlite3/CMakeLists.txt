cmake_minimum_required(VERSION 3.24)
project(sqlite3 LANGUAGES C)

# SRS 005: the umbrella library. Its own csrc/ is just the sqlite3_state
# forwarding shim (see include/sqlite/sqlite3_state.h) -- all real behavior
# comes from linking the ten SRS-001 sec.1.3 subsystem libraries together.
# Unlike those ten, this target *does* link its dependencies: it's the one
# place in the workspace where the whole, otherwise-cyclic symbol graph is
# resolved (SRS 001 FR-4's own smoke-tested pattern, applied here instead of
# a throwaway test executable). -Wl,--no-as-needed is required because the
# linker would otherwise drop any of the ten .so's this target's own code
# doesn't call into directly, even though something *inside* the set still
# needs it at load time.
sqlite3_cpp_add_split_library(sqlite3)

target_link_libraries(sqlite3 PUBLIC
    -Wl,--no-as-needed
    ${SQLITE_CPP_SPLIT_LIBRARIES}
    -Wl,--as-needed
)
