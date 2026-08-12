#pragma once

#include "../../testing/test_group.hpp"
#include "../status_registry.hpp"

namespace sqlite::utils::status::testing {

using sqlite::utils::testing::test_group;

inline const test_group status_registry_tests("status_registry", {
    {"a fresh registry reports zero for every category", [](auto& ctx) {
        status_registry registry;
        ctx.equal(registry.value(category::memory_used), std::int64_t{0});
        auto r = registry.read(category::memory_used);
        ctx.equal(r.current, std::int64_t{0});
        ctx.equal(r.highwater, std::int64_t{0});
    }},
    {"up increases the current value and raises the high-water mark", [](auto& ctx) {
        status_registry registry;
        registry.up(category::memory_used, 100);
        ctx.equal(registry.value(category::memory_used), std::int64_t{100});
        registry.up(category::memory_used, 50);
        auto r = registry.read(category::memory_used);
        ctx.equal(r.current, std::int64_t{150});
        ctx.equal(r.highwater, std::int64_t{150});
    }},
    {"down decreases the current value but not the high-water mark", [](auto& ctx) {
        status_registry registry;
        registry.up(category::memory_used, 200);
        registry.down(category::memory_used, 80);
        auto r = registry.read(category::memory_used);
        ctx.equal(r.current, std::int64_t{120});
        ctx.equal(r.highwater, std::int64_t{200});
    }},
    {"read with reset_highwater clamps the high-water mark to the current value", [](auto& ctx) {
        status_registry registry;
        registry.up(category::pagecache_used, 300);
        registry.down(category::pagecache_used, 250);
        auto r = registry.read(category::pagecache_used, /*reset_highwater=*/true);
        ctx.equal(r.current, std::int64_t{50});
        ctx.equal(r.highwater, std::int64_t{300});
        auto after = registry.read(category::pagecache_used);
        ctx.equal(after.highwater, std::int64_t{50});
    }},
    {"highwater raises the mark without touching the current value", [](auto& ctx) {
        status_registry registry;
        registry.up(category::parser_stack, 5);
        registry.highwater(category::parser_stack, 42);
        auto r = registry.read(category::parser_stack);
        ctx.equal(r.current, std::int64_t{5});
        ctx.equal(r.highwater, std::int64_t{42});
    }},
    {"categories are independent of one another", [](auto& ctx) {
        status_registry registry;
        registry.up(category::malloc_size, 10);
        registry.up(category::malloc_count, 3);
        ctx.equal(registry.value(category::malloc_size), std::int64_t{10});
        ctx.equal(registry.value(category::malloc_count), std::int64_t{3});
    }},
    {"global() returns the same instance across calls", [](auto& ctx) {
        ctx.check(&global() == &global());
    }},
});

} // namespace sqlite::utils::status::testing
