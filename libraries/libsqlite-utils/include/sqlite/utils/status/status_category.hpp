#pragma once

// Legacy source: status.c, sqlite3.h's SQLITE_STATUS_* #defines (SRS S3.2).
// FR-5: preprocessor constants are replaced by a scoped enumeration. These
// are not a bitmask (each sqlite3_status() call names exactly one counter),
// so no operator|/operator&/operator~ overloads are defined for it, unlike
// the SQLITE_OPEN_*-style flags FR-5 also covers.
namespace sqlite::utils::status {

enum class category {
    memory_used = 0,
    pagecache_used = 1,
    pagecache_overflow = 2,
    scratch_used = 3,        // legacy: not used since SQLite 3.8.5
    scratch_overflow = 4,    // legacy: not used since SQLite 3.8.5
    malloc_size = 5,
    parser_stack = 6,
    pagecache_size = 7,
    scratch_size = 8,        // legacy: not used since SQLite 3.8.5
    malloc_count = 9,
};

inline constexpr int category_count = 10;

} // namespace sqlite::utils::status
