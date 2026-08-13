#pragma once

#include "../cache/page_cache.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <cstring>

namespace sqlite::backend::pager::testing {

using sqlite::utils::testing::test_group;

inline const test_group page_cache_tests("page_cache", {
    {"reserve then get returns the same content that was written", [](auto& ctx) {
        cache::page_cache c;
        ctx.check(!c.contains(3));

        unsigned char* slot = c.reserve(3, 16);
        std::memcpy(slot, "0123456789abcdef", 16);

        ctx.check(c.contains(3));
        const unsigned char* got = c.get(3);
        ctx.check(got != nullptr);
        ctx.check(std::memcmp(got, "0123456789abcdef", 16) == 0);
    }},
    {"get on a missing page returns nullptr", [](auto& ctx) {
        cache::page_cache c;
        ctx.check(c.get(99) == nullptr);
    }},
    {"mark_dirty/is_dirty/clear_dirty track exactly the pages marked", [](auto& ctx) {
        cache::page_cache c;
        c.reserve(1, 8);
        c.reserve(2, 8);
        ctx.check(!c.is_dirty(1));

        c.mark_dirty(1);
        ctx.check(c.is_dirty(1));
        ctx.check(!c.is_dirty(2));
        ctx.equal(c.dirty_pages().size(), std::size_t{1});

        c.clear_dirty();
        ctx.check(!c.is_dirty(1));
        ctx.equal(c.dirty_pages().size(), std::size_t{0});
    }},
    {"truncate drops every cached page beyond the given count", [](auto& ctx) {
        cache::page_cache c;
        c.reserve(1, 8);
        c.reserve(2, 8);
        c.reserve(5, 8);

        c.truncate(2);

        ctx.check(c.contains(1));
        ctx.check(c.contains(2));
        ctx.check(!c.contains(5));
    }},
});

} // namespace sqlite::backend::pager::testing
