#pragma once

#include "../varint.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <cstdint>

namespace sqlite::backend::tree::testing {

using sqlite::utils::testing::test_group;

inline const test_group varint_tests("varint", {
    {"round-trips values spanning every encoded length (1 through 9 bytes)", [](auto& ctx) {
        std::uint64_t values[] = {
            0, 1, 0x7f, 0x80, 0x3fff, 0x4000,
            0xfffffu, 0x1000000u, 0xffffffffu,
            std::uint64_t{0x100000000}, ~std::uint64_t{0},
        };
        for (std::uint64_t v : values) {
            unsigned char buf[9];
            int written = put_varint(buf, v);
            ctx.check(written >= 1 && written <= 9);
            ctx.equal(written, varint_length(v));

            std::uint64_t decoded = 0;
            int read = get_varint(buf, &decoded);
            ctx.equal(read, written);
            ctx.equal(decoded, v);
        }
    }},
    {"smaller values encode to fewer bytes", [](auto& ctx) {
        ctx.equal(varint_length(0), 1);
        ctx.equal(varint_length(127), 1);
        ctx.equal(varint_length(128), 2);
        ctx.equal(varint_length(16383), 2);
        ctx.equal(varint_length(16384), 3);
        ctx.equal(varint_length(~std::uint64_t{0}), 9);
    }},
});

} // namespace sqlite::backend::tree::testing
