#pragma once

// Legacy source: sqlite3.h's SQLITE_OK/SQLITE_BUSY/SQLITE_IOERR* result-code
// #defines (SRS S3.3.1). FR-5: scoped enumeration replacing the subset of
// SQLITE_* result codes this library's file/VFS operations can return.
// Values match the legacy numeric codes exactly (including the "primary code
// | (extended<<8)" extended-error-code encoding) so that a later phase's
// extern "C" FR-7 façade can forward these directly as the public API's
// `int` result codes with no translation table.
namespace sqlite::backend::os {

enum class result : int {
    ok = 0,
    perm = 3,
    busy = 5,
    nomem = 7,
    readonly = 8,
    ioerr = 10,
    full = 13,
    notfound = 12,
    cantopen = 14,

    ioerr_read = 10 | (1 << 8),
    ioerr_short_read = 10 | (2 << 8),
    ioerr_write = 10 | (3 << 8),
    ioerr_fsync = 10 | (4 << 8),
    ioerr_truncate = 10 | (6 << 8),
    ioerr_fstat = 10 | (7 << 8),
    ioerr_unlock = 10 | (8 << 8),
    ioerr_rdlock = 10 | (9 << 8),
    ioerr_delete = 10 | (10 << 8),
    ioerr_access = 10 | (13 << 8),
    ioerr_lock = 10 | (15 << 8),
};

constexpr bool ok(result r) noexcept { return r == result::ok; }

} // namespace sqlite::backend::os
