#pragma once

#include "status_category.hpp"

#include <array>
#include <cstdint>
#include <mutex>

// Legacy source: status.c (SRS S3.2, sqlite::utils::status). Ports
// sqlite3StatusValue/sqlite3StatusUp/sqlite3StatusDown/sqlite3StatusHighwater:
// a fixed bank of "current value" + "high-water mark" counters used to
// implement sqlite3_status()/sqlite3_status64(). The legacy code requires
// each caller to already be holding one of two specific external mutexes
// (the allocator mutex or the pcache1 mutex, chosen per-category via a
// static table) and only asserts that fact; this port instead makes the
// registry own its synchronization internally (a single std::mutex), which
// is a strictly stronger guarantee and needs no caller-side protocol.
namespace sqlite::utils::status {

class status_registry {
public:
    // Adds n to the current value of `cat`; raises the high-water mark if the
    // new current value exceeds it. n may be negative.
    void up(category cat, std::int64_t n) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto index = static_cast<std::size_t>(cat);
        now_[index] += n;
        if (now_[index] > high_[index]) high_[index] = now_[index];
    }

    // Lowers the current value of `cat` by n (n must be non-negative). The
    // high-water mark is unaffected, mirroring sqlite3StatusDown().
    void down(category cat, std::int64_t n) {
        std::lock_guard<std::mutex> lock(mutex_);
        now_[static_cast<std::size_t>(cat)] -= n;
    }

    // Directly raises the high-water mark to x if x exceeds the current
    // high-water mark, without touching the current value. Used for counters
    // (like SQLITE_STATUS_PARSER_STACK) whose "current value" concept doesn't
    // apply -- only a maximum observed value.
    void highwater(category cat, std::int64_t x) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto index = static_cast<std::size_t>(cat);
        if (x > high_[index]) high_[index] = x;
    }

    [[nodiscard]] std::int64_t value(category cat) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return now_[static_cast<std::size_t>(cat)];
    }

    // Returns {current, high-water}. If reset_highwater is true, the
    // high-water mark is reset to the current value after reading, mirroring
    // sqlite3_status64()'s resetFlag argument.
    struct reading {
        std::int64_t current;
        std::int64_t highwater;
    };

    reading read(category cat, bool reset_highwater = false) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto index = static_cast<std::size_t>(cat);
        reading result{now_[index], high_[index]};
        if (reset_highwater) high_[index] = now_[index];
        return result;
    }

private:
    mutable std::mutex mutex_;
    std::array<std::int64_t, category_count> now_{};
    std::array<std::int64_t, category_count> high_{};
};

// Legacy status.c keeps one process-wide sqlite3Stat instance; global()
// mirrors that default while still letting tests construct an independent
// status_registry when they need isolation (status_registry_test.hpp does).
inline status_registry& global() {
    static status_registry instance;
    return instance;
}

} // namespace sqlite::utils::status
