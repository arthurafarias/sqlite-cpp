#pragma once

#include <cstdint>

// Legacy source: os.h's NO_LOCK/SHARED_LOCK/RESERVED_LOCK/PENDING_LOCK/
// EXCLUSIVE_LOCK #defines, and the byte-range locking scheme documented
// alongside them (SRS S3.3.1). FR-5: replaced by a scoped enumeration. Not a
// bitmask -- lock_level values form a strict order used to compare "at least
// as strong a lock", not to combine with |/& -- so no bitmask operators are
// defined for it (contrast open_flags.hpp/device_characteristics.hpp, which
// are bitmasks).
//
// The five levels and their semantics (unchanged from the legacy comment):
//   none:      no lock held.
//   shared:    any number of processes may hold a shared lock simultaneously.
//   reserved:  at most one process may hold reserved; others may still hold shared.
//   pending:   at most one process may hold pending; blocks new shared locks
//              but lets existing shared locks persist. Never requested
//              directly -- a file implementation may pass through it on the
//              way to exclusive.
//   exclusive: precludes all other locks.
namespace sqlite::backend::os {

enum class lock_level {
    none = 0,
    shared = 1,
    reserved = 2,
    pending = 3,
    exclusive = 4,
};

// The byte-range locking scheme shared by the unix and windows bodies
// (os.h's PENDING_BYTE/RESERVED_BYTE/SHARED_FIRST/SHARED_SIZE): a
// process/file lock state is expressed as advisory locks over these
// well-known byte offsets, near the 1GiB mark so it doesn't collide with
// real page data for any page size SQLite supports.
inline constexpr std::int64_t pending_byte = 0x40000000;
inline constexpr std::int64_t reserved_byte = pending_byte + 1;
inline constexpr std::int64_t shared_first = pending_byte + 2;
inline constexpr std::int64_t shared_size = 510;

} // namespace sqlite::backend::os
