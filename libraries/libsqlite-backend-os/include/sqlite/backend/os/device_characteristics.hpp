#pragma once

#include <type_traits>

// Legacy source: sqlite3.h's SQLITE_IOCAP_* #defines and SQLITE_SYNC_*
// #defines (SRS S3.3.1). FR-5: bitmask constant sets replaced by scoped
// enumerations plus the standard C++ scoped-bitmask-enum operator trio.
namespace sqlite::backend::os {

enum class device_characteristics : unsigned int {
    none = 0,
    atomic = 0x00000001,
    atomic512 = 0x00000002,
    atomic1k = 0x00000004,
    atomic2k = 0x00000008,
    atomic4k = 0x00000010,
    atomic8k = 0x00000020,
    atomic16k = 0x00000040,
    atomic32k = 0x00000080,
    atomic64k = 0x00000100,
    safe_append = 0x00000200,
    sequential = 0x00000400,
    undeletable_when_open = 0x00000800,
    powersafe_overwrite = 0x00001000,
    immutable = 0x00002000,
    batch_atomic = 0x00004000,
};

constexpr device_characteristics operator|(device_characteristics a, device_characteristics b) noexcept {
    return static_cast<device_characteristics>(
        static_cast<std::underlying_type_t<device_characteristics>>(a) |
        static_cast<std::underlying_type_t<device_characteristics>>(b));
}
constexpr device_characteristics operator&(device_characteristics a, device_characteristics b) noexcept {
    return static_cast<device_characteristics>(
        static_cast<std::underlying_type_t<device_characteristics>>(a) &
        static_cast<std::underlying_type_t<device_characteristics>>(b));
}
constexpr device_characteristics operator~(device_characteristics a) noexcept {
    return static_cast<device_characteristics>(~static_cast<std::underlying_type_t<device_characteristics>>(a));
}

// sqlite3OsSync()'s flags argument: SQLITE_SYNC_NORMAL/FULL, optionally
// combined with SQLITE_SYNC_DATAONLY -- a separate, smaller bitmask from
// device_characteristics, kept as its own type rather than folded in since
// the legacy code never combines the two constant sets.
enum class sync_flags : unsigned int {
    normal = 0x00002,
    full = 0x00003,
    dataonly = 0x00010,
};

constexpr sync_flags operator|(sync_flags a, sync_flags b) noexcept {
    return static_cast<sync_flags>(
        static_cast<std::underlying_type_t<sync_flags>>(a) |
        static_cast<std::underlying_type_t<sync_flags>>(b));
}
constexpr sync_flags operator&(sync_flags a, sync_flags b) noexcept {
    return static_cast<sync_flags>(
        static_cast<std::underlying_type_t<sync_flags>>(a) &
        static_cast<std::underlying_type_t<sync_flags>>(b));
}

} // namespace sqlite::backend::os
