#pragma once

#include "../result.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

// Test-only support (not a legacy port): an in-memory model of the kernel's
// advisory-lock table, shared by two or more fake_locker instances
// representing distinct connections/processes contending for the same file.
// Lets byte_range_lock_test.hpp exercise real cross-connection lock conflicts
// (SHARED vs RESERVED vs PENDING vs EXCLUSIVE) deterministically and without
// touching a real filesystem, the way unix_file_test.hpp's real posix_locker
// tests do for the single-connection, real-fd case.
namespace sqlite::backend::os::testing {

class fake_lock_table {
public:
    result try_lock(int owner, std::int64_t start, std::int64_t len, bool exclusive) {
        std::int64_t end = len == 0 ? std::numeric_limits<std::int64_t>::max() : start + len;
        for (const auto& e : entries_) {
            if (e.owner == owner) continue;
            if (!overlaps(e.start, e.end, start, end)) continue;
            if (exclusive || e.exclusive) return result::busy;
        }
        remove_owner_overlaps(owner, start, end);
        entries_.push_back({owner, start, end, exclusive});
        return result::ok;
    }

    result try_unlock(int owner, std::int64_t start, std::int64_t len) {
        std::int64_t end = len == 0 ? std::numeric_limits<std::int64_t>::max() : start + len;
        remove_owner_overlaps(owner, start, end);
        return result::ok;
    }

    [[nodiscard]] bool owner_holds_exclusive(int owner, std::int64_t at) const {
        for (const auto& e : entries_) {
            if (e.owner == owner && e.exclusive && at >= e.start && at < e.end) return true;
        }
        return false;
    }

private:
    struct entry {
        int owner;
        std::int64_t start;
        std::int64_t end;
        bool exclusive;
    };

    static bool overlaps(std::int64_t a_start, std::int64_t a_end, std::int64_t b_start, std::int64_t b_end) {
        return a_start < b_end && b_start < a_end;
    }

    void remove_owner_overlaps(int owner, std::int64_t start, std::int64_t end) {
        entries_.erase(
            std::remove_if(entries_.begin(), entries_.end(),
                [&](const entry& e) { return e.owner == owner && overlaps(e.start, e.end, start, end); }),
            entries_.end());
    }

    std::vector<entry> entries_;
};

class fake_locker {
public:
    fake_locker(fake_lock_table& table, int owner) : table_(&table), owner_(owner) {}

    result lock_range(std::int64_t start, std::int64_t len, bool exclusive) const {
        return table_->try_lock(owner_, start, len, exclusive);
    }
    result unlock_range(std::int64_t start, std::int64_t len) const {
        return table_->try_unlock(owner_, start, len);
    }

private:
    fake_lock_table* table_;
    int owner_;
};

} // namespace sqlite::backend::os::testing
