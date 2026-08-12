#pragma once

// Legacy source: sqlite3.h's SQLITE_MUTEX_* #defines (SRS S3.2). FR-5:
// replaces the preprocessor constants sqlite3_mutex_alloc() takes with a
// scoped enumeration. Not a bitmask -- each allocation names exactly one
// mutex identity -- so no operator|/&/~ overloads are defined here.
namespace sqlite::utils::mutex {

enum class mutex_id {
    fast = 0,
    recursive = 1,
    static_main = 2,
    static_mem = 3,       // sqlite3_malloc()
    static_open = 4,      // sqlite3BtreeOpen()  (legacy alias: STATIC_MEM2, unused)
    static_prng = 5,       // sqlite3_randomness()
    static_lru = 6,         // lru page list
    static_pmem = 7,        // sqlite3PageMalloc()  (legacy alias: STATIC_LRU2, unused)
    static_app1 = 8,
    static_app2 = 9,
    static_app3 = 10,
    static_vfs1 = 11,
    static_vfs2 = 12,
    static_vfs3 = 13,
};

} // namespace sqlite::utils::mutex
