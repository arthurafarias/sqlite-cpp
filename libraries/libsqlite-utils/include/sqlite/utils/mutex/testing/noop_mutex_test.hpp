#pragma once

#include "../../testing/test_group.hpp"
#include "../noop_mutex.hpp"

namespace sqlite::utils::mutex::testing {

using sqlite::utils::testing::test_group;

inline const test_group noop_mutex_tests("noop_mutex", {
    {"a freshly constructed mutex is not held", [](auto& ctx) {
        noop_mutex m;
        ctx.check(m.not_held());
        ctx.check(!m.held());
    }},
    {"enter/leave toggles held/not_held", [](auto& ctx) {
        noop_mutex m;
        m.enter();
        ctx.check(m.held());
        m.leave();
        ctx.check(m.not_held());
    }},
    {"try_enter behaves like enter and always succeeds", [](auto& ctx) {
        noop_mutex m;
        ctx.check(m.try_enter());
        ctx.check(m.held());
        m.leave();
    }},
    {"a recursive mutex can be entered more than once", [](auto& ctx) {
        noop_mutex m(mutex_id::recursive);
        m.enter();
        m.enter();
        ctx.check(m.held());
        m.leave();
        ctx.check(m.held());
        m.leave();
        ctx.check(m.not_held());
    }},
});

} // namespace sqlite::utils::mutex::testing
