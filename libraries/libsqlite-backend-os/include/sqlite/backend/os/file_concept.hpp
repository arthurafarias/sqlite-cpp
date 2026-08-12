#pragma once

#include "device_characteristics.hpp"
#include "lock_level.hpp"
#include "result.hpp"

#include <concepts>
#include <cstdint>

// Legacy source: os.h's sqlite3OsClose/Read/Write/Truncate/Sync/FileSize/
// Lock/Unlock/CheckReservedLock/SectorSize/DeviceCharacteristics wrapper
// functions, which forward through the sqlite3_io_methods vtable (SRS
// S3.3.1). This concept is the C++ replacement for that vtable: a
// conforming file type satisfies it structurally, no virtual dispatch or
// registration required.
//
// Deferred (not part of this concept, and not ported this phase): xFetch/
// xUnfetch (mmap-backed I/O), xShmMap/xShmLock/xShmBarrier/xShmUnmap (WAL
// shared memory), and xFileControl's opcode-specific hint channel. All are
// real legacy sqlite3_io_methods entries but are separable extensions of the
// core file contract below, which is what the rollback-journal path (and a
// correct, if not WAL-capable, database) actually needs.
namespace sqlite::backend::os {

template<class F>
concept file = requires(F f, const F cf, void* buf, const void* cbuf, std::int64_t amt, bool& out_bool, std::int64_t& out_i64) {
    { f.close() } -> std::same_as<result>;
    { f.read(buf, static_cast<int>(amt), amt) } -> std::same_as<result>;
    { f.write(cbuf, static_cast<int>(amt), amt) } -> std::same_as<result>;
    { f.truncate(amt) } -> std::same_as<result>;
    { f.sync(sync_flags::normal) } -> std::same_as<result>;
    { cf.file_size(out_i64) } -> std::same_as<result>;
    { f.lock(lock_level::shared) } -> std::same_as<result>;
    { f.unlock(lock_level::shared) } -> std::same_as<result>;
    { cf.check_reserved_lock(out_bool) } -> std::same_as<result>;
    { cf.sector_size() } -> std::same_as<int>;
    // Named `characteristics()`, not `device_characteristics()`: GCC/Clang
    // reject a member function whose name exactly matches its own return
    // type ("declaration ... changes meaning of 'device_characteristics'"),
    // since the identifier would denote both the enum type and a class
    // member within the same scope.
    { cf.characteristics() } -> std::same_as<sqlite::backend::os::device_characteristics>;
};

} // namespace sqlite::backend::os
