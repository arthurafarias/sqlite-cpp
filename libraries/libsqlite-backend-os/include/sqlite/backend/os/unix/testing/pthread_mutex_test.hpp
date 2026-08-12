#pragma once

#include "../pthread_mutex.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <atomic>
#include <thread>
#include <vector>

namespace sqlite::backend::os::unix::testing {

using sqlite::utils::testing::test_group;

inline const test_group pthread_mutex_tests("pthread_mutex", {
    {"a fresh fast mutex is not held", [](auto& ctx) {
        pthread_mutex m;
        ctx.check(m.not_held());
    }},
    {"enter/leave toggles held/not_held", [](auto& ctx) {
        pthread_mutex m;
        m.enter();
        ctx.check(m.held());
        m.leave();
        ctx.check(m.not_held());
    }},
    {"a recursive mutex can be entered more than once by the owning thread", [](auto& ctx) {
        pthread_mutex m(sqlite::utils::mutex::mutex_id::recursive);
        m.enter();
        m.enter();
        ctx.check(m.held());
        m.leave();
        ctx.check(m.held());
        m.leave();
        ctx.check(m.not_held());
    }},
    {"try_enter succeeds when unheld and fails while held by another thread", [](auto& ctx) {
        pthread_mutex m;
        ctx.check(m.try_enter());
        std::atomic<bool> other_saw_busy{false};
        std::thread other([&] { other_saw_busy = !m.try_enter(); });
        other.join();
        ctx.check(other_saw_busy.load());
        m.leave();
    }},
    {"actually serializes concurrent increments of a shared counter", [](auto& ctx) {
        pthread_mutex m;
        long counter = 0;
        std::vector<std::thread> threads;
        for (int i = 0; i < 8; ++i) {
            threads.emplace_back([&] {
                for (int j = 0; j < 10000; ++j) {
                    m.enter();
                    ++counter;
                    m.leave();
                }
            });
        }
        for (auto& t : threads) t.join();
        ctx.equal(counter, 80000L);
    }},
});

} // namespace sqlite::backend::os::unix::testing
