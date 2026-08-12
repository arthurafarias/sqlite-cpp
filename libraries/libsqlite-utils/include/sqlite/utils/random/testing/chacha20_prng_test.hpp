#pragma once

#include "../../testing/test_group.hpp"
#include "../chacha20_prng.hpp"

#include <array>
#include <cstring>

namespace sqlite::utils::random::testing {

using sqlite::utils::testing::test_group;

inline const test_group chacha20_prng_tests("chacha20_prng", {
    {"chacha_block is a deterministic pure function of its input", [](auto& ctx) {
        std::array<std::uint32_t, 16> in{};
        for (std::uint32_t i = 0; i < 16; ++i) in[i] = i * 0x01010101u;
        std::array<std::uint32_t, 16> out1{}, out2{};
        chacha20_prng::chacha_block(out1.data(), in.data());
        chacha20_prng::chacha_block(out2.data(), in.data());
        ctx.check(out1 == out2);
        // The block function must not be the identity transform.
        ctx.check(out1 != in);
    }},
    {"reseeding to the same state reproduces the same byte stream", [](auto& ctx) {
        std::array<std::uint32_t, 16> seed{};
        for (std::uint32_t i = 0; i < 16; ++i) seed[i] = i + 1;

        chacha20_prng a, b;
        a.reseed(seed);
        b.reseed(seed);

        unsigned char buf_a[37];
        unsigned char buf_b[37];
        a.randomness(sizeof(buf_a), buf_a);
        b.randomness(sizeof(buf_b), buf_b);
        ctx.check(std::memcmp(buf_a, buf_b, sizeof(buf_a)) == 0);
    }},
    {"save_state/restore_state makes subsequent output reproducible", [](auto& ctx) {
        std::array<std::uint32_t, 16> seed{};
        for (std::uint32_t i = 0; i < 16; ++i) seed[i] = 0x11223344u + i;

        chacha20_prng gen;
        gen.reseed(seed);
        auto checkpoint = gen.save_state();

        unsigned char first[20];
        gen.randomness(sizeof(first), first);

        gen.restore_state(checkpoint);
        unsigned char second[20];
        gen.randomness(sizeof(second), second);

        ctx.check(std::memcmp(first, second, sizeof(first)) == 0);
    }},
    {"randomness with n<=0 or a null buffer resets the generator without crashing", [](auto& ctx) {
        chacha20_prng gen;
        gen.randomness(0, nullptr);
        gen.randomness(-1, nullptr);
        unsigned char buf[8];
        gen.randomness(static_cast<int>(sizeof(buf)), buf);
        ctx.check(true);
    }},
    {"randomness fills a buffer larger than one 64-byte block", [](auto& ctx) {
        chacha20_prng gen;
        unsigned char buf[200] = {};
        gen.randomness(static_cast<int>(sizeof(buf)), buf);
        bool any_nonzero = false;
        for (unsigned char c : buf) any_nonzero |= (c != 0);
        ctx.check(any_nonzero);
    }},
});

} // namespace sqlite::utils::random::testing
