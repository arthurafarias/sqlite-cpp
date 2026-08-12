#pragma once

#include "../lock_level.hpp"
#include "../result.hpp"

#include <concepts>
#include <cstdint>
#include <utility>

// Legacy source: os_unix.c's unixLock/posixUnlock/unixCheckReservedLock and
// os_win.c's winLock/winUnlock (SRS S3.3.1). os.h documents that "the same
// locking strategy and byte ranges are used for Unix [and Windows]" -- both
// implement the identical SHARED/RESERVED/PENDING/EXCLUSIVE state machine
// over the same three well-known byte ranges (pending_byte, reserved_byte,
// [shared_first, shared_first+shared_size)), differing only in the OS
// primitive used to lock/unlock a byte range (POSIX fcntl() advisory locks
// vs Win32 LockFile()/LockFileEx()). This header ports that shared state
// machine once, as a template over a `byte_range_locker`, so the unix and
// windows bodies (unix/posix_locker.hpp, windows/win32_locker.hpp) only need
// to implement the OS-specific primitive -- eliminating the risk of the two
// ports' lock protocols silently diverging.
//
// Known, deliberate scope reduction vs. the legacy code: the legacy
// unixInodeInfo/unixShmNode machinery additionally tracks every open
// unixFile* on the *same inode within the same process*, because POSIX
// advisory locks (fcntl) are scoped to the process, not the file
// descriptor -- two fds in one process locking the same file would
// otherwise silently clobber each other's lock state. This port tracks lock
// state per byte_range_lock_state_machine instance (i.e. per open file
// handle) only, which is correct for the common case of at most one open
// handle per file per process, but does not reproduce that same-process,
// multiple-handle interlocking. That interlocking (plus the NFS-specific
// two-phase downgrade workaround, WAL shared-memory locks, and Windows'
// probabilistic-shared-lock-byte fallback for pre-NT systems) is deferred,
// exactly as mem2.c/mem3.c/mem5.c were deferred in sqlite-utils (SRS S3.2).
namespace sqlite::backend::os::detail {

template<class L>
concept byte_range_locker = requires(L l, std::int64_t start, std::int64_t len) {
    { l.lock_range(start, len, /*exclusive=*/true) } -> std::same_as<result>;
    { l.unlock_range(start, len) } -> std::same_as<result>;
};

template<byte_range_locker Locker>
class byte_range_lock_state_machine {
public:
    explicit byte_range_lock_state_machine(Locker locker) : locker_(std::move(locker)) {}

    [[nodiscard]] lock_level current() const noexcept { return level_; }

    // Mirrors unixLock()/winLock(): raise the lock to at least `requested`.
    // A no-op if already at or above that level.
    result lock(lock_level requested) {
        if (level_ >= requested) return result::ok;

        if (requested == lock_level::shared ||
            (requested == lock_level::exclusive && level_ == lock_level::reserved)) {
            // A PENDING lock is taken first: a read-lock when upgrading to
            // SHARED (so it can co-exist with other readers' own PENDING
            // probes), a write-lock when upgrading RESERVED -> EXCLUSIVE (so
            // no new SHARED locks can start while we wait for existing ones
            // to drain).
            result r = locker_.lock_range(pending_byte, 1, requested != lock_level::shared);
            if (r != result::ok) return r;
            if (requested == lock_level::exclusive) level_ = lock_level::pending;
        }

        if (requested == lock_level::shared) {
            result r = locker_.lock_range(shared_first, shared_size, /*exclusive=*/false);
            result unlock_pending = locker_.unlock_range(pending_byte, 1);
            if (r != result::ok) return r;
            if (unlock_pending != result::ok) return result::ioerr_unlock;
            level_ = lock_level::shared;
        } else if (requested == lock_level::reserved) {
            result r = locker_.lock_range(reserved_byte, 1, /*exclusive=*/true);
            if (r != result::ok) return r;
            level_ = requested;
        } else {
            // EXCLUSIVE: takes a write-lock over the same [shared_first,
            // shared_first+shared_size) range already held (per the
            // SHARED-or-RESERVED precondition, os.h's locking notes) as a
            // read lock. POSIX fcntl can convert a held lock to a stronger
            // mode on the same range atomically, but Win32 LockFileEx
            // cannot -- winLock() (os_win.c) explicitly unlocks the read
            // lock first and, if the exclusive attempt then fails, restores
            // it. Doing the same unconditionally here (harmless extra
            // syscalls on unix, required correctness on Windows) is what
            // lets this one state machine drive both lockers.
            result unlock_read = locker_.unlock_range(shared_first, shared_size);
            if (unlock_read != result::ok) return result::ioerr_unlock;
            result r = locker_.lock_range(shared_first, shared_size, /*exclusive=*/true);
            if (r != result::ok) {
                // Restore the read lock so the connection still holds at
                // least what it held before this attempt (SHARED or
                // RESERVED), matching winLock()'s fallback.
                locker_.lock_range(shared_first, shared_size, /*exclusive=*/false);
                return r;
            }
            level_ = requested;
        }
        return result::ok;
    }

    // Mirrors posixUnlock()/winUnlock(): lower the lock to `requested`,
    // which must be none or shared. A no-op if already at or below that
    // level.
    result unlock(lock_level requested) {
        lock_level original = level_;
        if (original <= requested) return result::ok;

        // Tracks whether [shared_first, shared_first+shared_size) is
        // currently held (as either a read or a write lock) as this method
        // works its way down through the levels below, so the final
        // requested==none step below knows whether it still needs to
        // release that range or whether the exclusive-downgrade step
        // already did.
        bool holds_shared_range = original >= lock_level::shared;

        if (original == lock_level::exclusive) {
            // Must fully release the write lock before either downgrading
            // to a read lock or releasing entirely: POSIX fcntl can convert
            // a held lock to a weaker mode on the same range atomically,
            // but Win32 LockFileEx cannot -- winUnlock() (os_win.c)
            // explicitly unlocks first and then, only if downgrading to
            // SHARED (not releasing entirely), reacquires a read lock.
            result r = locker_.unlock_range(shared_first, shared_size);
            if (r != result::ok) return result::ioerr_unlock;
            holds_shared_range = false;
            if (requested == lock_level::shared) {
                result r2 = locker_.lock_range(shared_first, shared_size, /*exclusive=*/false);
                if (r2 != result::ok) return result::ioerr_rdlock;
                holds_shared_range = true;
            }
        }

        if (original >= lock_level::reserved) {
            // RESERVED_BYTE and PENDING_BYTE are always locked (and
            // released) as separate 1-byte regions, never combined into one
            // call spanning both: Win32's LockFileEx/UnlockFileEx require a
            // region to exactly match a prior lock call (unlike POSIX
            // fcntl, which would happily accept one call spanning both,
            // since fcntl recomputes the process's lock table by range
            // subtraction regardless of original call boundaries).
            result r = locker_.unlock_range(reserved_byte, 1);
            if (r != result::ok) return result::ioerr_unlock;
        }

        if (requested == lock_level::none && holds_shared_range) {
            result r = locker_.unlock_range(shared_first, shared_size);
            if (r != result::ok) return result::ioerr_unlock;
        }

        if (original >= lock_level::pending) {
            // pending_byte is only ever held as a residual of a RESERVED ->
            // EXCLUSIVE attempt (successful or not -- see lock()'s comment
            // on why a failed attempt leaves level_ at `pending`, per os.h's
            // documented PENDING-then-retry protocol).
            result r = locker_.unlock_range(pending_byte, 1);
            if (r != result::ok) return result::ioerr_unlock;
        }

        level_ = requested;
        return result::ok;
    }

    // Mirrors unixCheckReservedLock()/winCheckReservedLock(): probes whether
    // some other connection (this process or another) holds at least a
    // RESERVED lock, without acquiring or releasing anything on this handle.
    result check_reserved_lock(bool& reserved_out) const {
        if (level_ >= lock_level::reserved) {
            reserved_out = true;
            return result::ok;
        }
        result r = locker_.lock_range(reserved_byte, 1, /*exclusive=*/true);
        if (r == result::busy) {
            reserved_out = true;
            return result::ok;
        }
        if (r != result::ok) return r;
        result unlock_r = locker_.unlock_range(reserved_byte, 1);
        reserved_out = false;
        return unlock_r;
    }

    [[nodiscard]] Locker& locker() noexcept { return locker_; }

private:
    Locker locker_;
    lock_level level_ = lock_level::none;
};

} // namespace sqlite::backend::os::detail
