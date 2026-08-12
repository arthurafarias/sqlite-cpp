#pragma once

#include "../../testing/test_group.hpp"
#include "../utf.hpp"

namespace sqlite::utils::text::testing {

using sqlite::utils::testing::test_group;

inline const test_group utf_tests("utf", {
    {"encode_utf8/decode_utf8 round-trip ASCII", [](auto& ctx) {
        auto decoded = decode_utf8("hello");
        ctx.equal(decoded.size(), std::size_t{5});
        ctx.equal(encode_utf8(decoded), std::string{"hello"});
    }},
    {"encode_utf8/decode_utf8 round-trip multi-byte characters", [](auto& ctx) {
        // U+00E9 (e-acute, 2 bytes), U+4E2D (CJK, 3 bytes), U+1F600 (emoji, 4 bytes)
        std::u32string codepoints = {0x00E9, 0x4E2D, 0x1F600};
        std::string encoded = encode_utf8(codepoints);
        auto decoded = decode_utf8(encoded);
        ctx.check(decoded == codepoints);
    }},
    {"read_utf8 decodes a null-terminated multi-byte string", [](auto& ctx) {
        std::string encoded = encode_utf8(static_cast<std::uint32_t>(0x4E2D));
        encoded.push_back('\0');
        const auto* p = reinterpret_cast<const unsigned char*>(encoded.data());
        std::uint32_t c = read_utf8(&p);
        ctx.equal(c, static_cast<std::uint32_t>(0x4E2D));
    }},
    {"append_utf8 returns the correct byte length per codepoint range", [](auto& ctx) {
        char buf[4];
        ctx.equal(append_utf8(buf, 0x41), 1);
        ctx.equal(append_utf8(buf, 0x00E9), 2);
        ctx.equal(append_utf8(buf, 0x4E2D), 3);
        ctx.equal(append_utf8(buf, 0x1F600), 4);
    }},
    {"a UTF-16 surrogate value decodes to the replacement character", [](auto& ctx) {
        // 0xED 0xA0 0x80 is an over-length 3-byte encoding of 0xD800 (a
        // surrogate), which utf.c documents as folding to U+FFFD.
        unsigned char bytes[] = {0xED, 0xA0, 0x80, 0x00};
        const unsigned char* p = bytes;
        std::uint32_t c = read_utf8(&p);
        ctx.equal(c, static_cast<std::uint32_t>(0xFFFD));
    }},
});

} // namespace sqlite::utils::text::testing
