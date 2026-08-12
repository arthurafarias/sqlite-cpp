#pragma once

#include "../detail/byte_range_lock.hpp"
#include "fake_lock_table.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::backend::os::testing {

using sqlite::utils::testing::test_group;
using detail::byte_range_lock_state_machine;

inline const test_group byte_range_lock_tests("byte_range_lock_state_machine", {
    {"a fresh state machine holds no lock", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        ctx.check(a.current() == lock_level::none);
    }},
    {"two connections can both hold a shared lock", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        byte_range_lock_state_machine<fake_locker> b(fake_locker(table, 2));
        ctx.check(a.lock(lock_level::shared) == result::ok);
        ctx.check(b.lock(lock_level::shared) == result::ok);
        ctx.check(a.current() == lock_level::shared);
        ctx.check(b.current() == lock_level::shared);
    }},
    {"reserved is exclusive against another reserved but coexists with shared", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        byte_range_lock_state_machine<fake_locker> b(fake_locker(table, 2));
        ctx.check(a.lock(lock_level::shared) == result::ok);
        ctx.check(b.lock(lock_level::shared) == result::ok);
        ctx.check(a.lock(lock_level::reserved) == result::ok);
        // b still holds its shared lock; a second reserved attempt by a
        // third connection must fail while a holds reserved.
        byte_range_lock_state_machine<fake_locker> c(fake_locker(table, 3));
        ctx.check(c.lock(lock_level::shared) == result::ok);
        ctx.check(c.lock(lock_level::reserved) == result::busy);
    }},
    {"exclusive blocks a concurrent shared request", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        byte_range_lock_state_machine<fake_locker> b(fake_locker(table, 2));
        ctx.check(a.lock(lock_level::shared) == result::ok);
        ctx.check(a.lock(lock_level::exclusive) == result::ok);
        ctx.check(a.current() == lock_level::exclusive);
        ctx.check(b.lock(lock_level::shared) == result::busy);
    }},
    {"upgrading reserved to exclusive fails (returns busy) while another holds shared", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        byte_range_lock_state_machine<fake_locker> b(fake_locker(table, 2));
        ctx.check(a.lock(lock_level::shared) == result::ok);
        ctx.check(b.lock(lock_level::shared) == result::ok);
        ctx.check(a.lock(lock_level::reserved) == result::ok);
        ctx.check(a.lock(lock_level::exclusive) == result::busy);
        // a's own shared lock persists even though the exclusive attempt failed.
        ctx.check(a.current() >= lock_level::reserved);
    }},
    {"unlock releases the lock and lets another connection acquire exclusive", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        byte_range_lock_state_machine<fake_locker> b(fake_locker(table, 2));
        // The real sequence SQLite's pager drives a writer through: shared,
        // then reserved, then (after other readers drop off) exclusive.
        ctx.check(a.lock(lock_level::shared) == result::ok);
        ctx.check(a.lock(lock_level::reserved) == result::ok);
        ctx.check(a.lock(lock_level::exclusive) == result::ok);
        ctx.check(a.unlock(lock_level::none) == result::ok);
        ctx.check(a.current() == lock_level::none);
        // Once a has fully released, nothing contends with b: it can go
        // straight to exclusive (the RESERVED-first convention is a
        // decision the pager layer makes, not a hard OS-level requirement).
        ctx.check(b.lock(lock_level::exclusive) == result::ok);
    }},
    {"check_reserved_lock reports another connection's reserved lock", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        byte_range_lock_state_machine<fake_locker> b(fake_locker(table, 2));
        ctx.check(a.lock(lock_level::shared) == result::ok);
        ctx.check(a.lock(lock_level::reserved) == result::ok);

        ctx.check(b.lock(lock_level::shared) == result::ok);
        bool reserved = false;
        ctx.check(b.check_reserved_lock(reserved) == result::ok);
        ctx.check(reserved);
    }},
    {"check_reserved_lock reports false when no one holds reserved", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        ctx.check(a.lock(lock_level::shared) == result::ok);
        bool reserved = true;
        ctx.check(a.check_reserved_lock(reserved) == result::ok);
        ctx.check(!reserved);
    }},
    {"lock is a no-op when already at or above the requested level", [](auto& ctx) {
        fake_lock_table table;
        byte_range_lock_state_machine<fake_locker> a(fake_locker(table, 1));
        ctx.check(a.lock(lock_level::exclusive) == result::ok);
        ctx.check(a.lock(lock_level::shared) == result::ok);
        ctx.check(a.current() == lock_level::exclusive);
    }},
});

} // namespace sqlite::backend::os::testing
