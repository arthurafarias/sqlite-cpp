#pragma once

#include "../../testing/test_group.hpp"
#include "../system_allocator.hpp"

#include <cstring>

namespace sqlite::utils::memory::testing {

using sqlite::utils::testing::test_group;

inline const test_group system_allocator_tests("system_allocator", {
    {"round8 rounds up to the next multiple of 8", [](auto& ctx) {
        ctx.equal(round8(1), 8);
        ctx.equal(round8(8), 8);
        ctx.equal(round8(9), 16);
        ctx.equal(round8(0), 0);
    }},
    {"malloc/free round-trips and preserves written bytes", [](auto& ctx) {
        system_allocator alloc;
        void* p = alloc.malloc(32);
        ctx.check(p != nullptr);
        std::memset(p, 0x5a, 32);
        ctx.equal(static_cast<unsigned char*>(p)[0], static_cast<unsigned char>(0x5a));
        alloc.free(p);
    }},
    {"size reports the requested allocation size", [](auto& ctx) {
        system_allocator alloc;
        void* p = alloc.malloc(100);
        ctx.equal(alloc.size(p), 100);
        alloc.free(p);
    }},
    {"realloc preserves prefix bytes and updates size", [](auto& ctx) {
        system_allocator alloc;
        void* p = alloc.malloc(round8(16));
        static_cast<char*>(p)[0] = 'x';
        void* q = alloc.realloc(p, round8(64));
        ctx.check(q != nullptr);
        ctx.equal(static_cast<char*>(q)[0], 'x');
        ctx.equal(alloc.size(q), round8(64));
        alloc.free(q);
    }},
    {"init and shutdown succeed and are idempotent", [](auto& ctx) {
        system_allocator alloc;
        ctx.check(alloc.init());
        ctx.check(alloc.init());
        alloc.shutdown();
        alloc.shutdown();
    }},
});

} // namespace sqlite::utils::memory::testing
