#pragma once

#include <type_traits>

// Legacy source: sqlite3.h's SQLITE_OPEN_* #defines (SRS S3.3.1). FR-5: a
// bitmask constant set replaced by a scoped enumeration plus the standard
// C++ scoped-bitmask-enum operator trio (|, &, ~), so callers keep
// bitwise-flag ergonomics without reverting to raw ints or unscoped enums.
namespace sqlite::backend::os {

enum class open_flags : unsigned int {
    none = 0,
    readonly = 0x00000001,
    readwrite = 0x00000002,
    create = 0x00000004,
    deleteonclose = 0x00000008,
    exclusive = 0x00000010,
    main_db = 0x00000100,
    temp_db = 0x00000200,
    transient_db = 0x00000400,
    main_journal = 0x00000800,
    temp_journal = 0x00001000,
    subjournal = 0x00002000,
    super_journal = 0x00004000,
    wal = 0x00080000,
    nofollow = 0x01000000,
};

constexpr open_flags operator|(open_flags a, open_flags b) noexcept {
    return static_cast<open_flags>(
        static_cast<std::underlying_type_t<open_flags>>(a) |
        static_cast<std::underlying_type_t<open_flags>>(b));
}
constexpr open_flags operator&(open_flags a, open_flags b) noexcept {
    return static_cast<open_flags>(
        static_cast<std::underlying_type_t<open_flags>>(a) &
        static_cast<std::underlying_type_t<open_flags>>(b));
}
constexpr open_flags operator~(open_flags a) noexcept {
    return static_cast<open_flags>(~static_cast<std::underlying_type_t<open_flags>>(a));
}
constexpr bool any(open_flags a) noexcept {
    return static_cast<std::underlying_type_t<open_flags>>(a) != 0;
}

} // namespace sqlite::backend::os
