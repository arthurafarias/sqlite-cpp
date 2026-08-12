#pragma once

#include <cstdint>
#include <string>
#include <string_view>

// Legacy source: utf.c (SRS S3.2, sqlite::utils::text). Ports the UTF-8
// encode/decode primitives (sqlite3AppendOneUtf8Character, sqlite3Utf8Read
// and the READ_UTF8 macro's bounded variant) bit-for-bit, including their
// documented handling of invalid input: a multi-byte sequence that encodes a
// codepoint below 0x80, a UTF-16 surrogate (0xD800-0xDFFF), or a noncharacter
// (0xFFFE/0xFFFF) decodes to U+FFFD rather than being rejected, and an
// over-length encoding is accepted as-is. Downstream code that depends on
// this exact (lenient, not RFC-strict) behavior -- e.g. for byte-for-byte
// compatible column value decoding -- sees no change.
namespace sqlite::utils::text {

// utf.c's sqlite3Utf8Trans1[]: maps a lead byte in [0xc0, 0xff] (indexed by
// byte-0xc0) to the value bits it contributes before continuation bytes are
// folded in.
inline constexpr unsigned char utf8_trans1[64] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x00, 0x00,
};

// sqlite3AppendOneUtf8Character(): encodes codepoint v as UTF-8 into out
// (which must have room for at least 4 bytes) and returns the number of
// bytes written.
inline int append_utf8(char* out, std::uint32_t v) {
    if (v < 0x00080) {
        out[0] = static_cast<char>(v & 0xff);
        return 1;
    }
    if (v < 0x00800) {
        out[0] = static_cast<char>(0xc0 + ((v >> 6) & 0x1f));
        out[1] = static_cast<char>(0x80 + (v & 0x3f));
        return 2;
    }
    if (v < 0x10000) {
        out[0] = static_cast<char>(0xe0 + ((v >> 12) & 0x0f));
        out[1] = static_cast<char>(0x80 + ((v >> 6) & 0x3f));
        out[2] = static_cast<char>(0x80 + (v & 0x3f));
        return 3;
    }
    out[0] = static_cast<char>(0xf0 + ((v >> 18) & 0x07));
    out[1] = static_cast<char>(0x80 + ((v >> 12) & 0x3f));
    out[2] = static_cast<char>(0x80 + ((v >> 6) & 0x3f));
    out[3] = static_cast<char>(0x80 + (v & 0x3f));
    return 4;
}

inline std::string encode_utf8(std::uint32_t codepoint) {
    char buf[4];
    int n = append_utf8(buf, codepoint);
    return std::string(buf, static_cast<std::size_t>(n));
}

namespace detail {
inline std::uint32_t fold_invalid(std::uint32_t c) {
    if (c < 0x80 || (c & 0xFFFFF800u) == 0xD800u || (c & 0xFFFFFFFEu) == 0xFFFEu) {
        return 0xFFFD;
    }
    return c;
}
} // namespace detail

// sqlite3Utf8Read(): decodes one UTF-8 character starting at *pz, advancing
// *pz past it. Assumes the string is null-terminated (matches legacy
// behavior -- a truncated multi-byte sequence at the very end of the buffer
// stops consuming continuation bytes at the terminating 0x00, exactly as the
// legacy `while( (*(*pz) & 0xc0)==0x80 )` loop does with no upper bound
// other than the terminator).
inline std::uint32_t read_utf8(const unsigned char** pz) {
    unsigned int c = *((*pz)++);
    if (c >= 0xc0) {
        c = utf8_trans1[c - 0xc0];
        while ((**pz & 0xc0) == 0x80) {
            c = (c << 6) + (0x3f & *((*pz)++));
        }
        c = detail::fold_invalid(c);
    }
    return c;
}

// Bounded variant of READ_UTF8: like read_utf8(), but never reads past
// `term` (for buffers that are not null-terminated).
inline std::uint32_t read_utf8_bounded(const unsigned char** pz, const unsigned char* term) {
    unsigned int c = *((*pz)++);
    if (c >= 0xc0) {
        c = utf8_trans1[c - 0xc0];
        while (*pz < term && (**pz & 0xc0) == 0x80) {
            c = (c << 6) + (0x3f & *((*pz)++));
        }
        c = detail::fold_invalid(c);
    }
    return c;
}

// Decodes every character in a UTF-8 string_view to UTF-32, using the
// bounded reader above. Convenience built on read_utf8_bounded(), not a
// distinct legacy routine.
inline std::u32string decode_utf8(std::string_view s) {
    std::u32string result;
    const auto* p = reinterpret_cast<const unsigned char*>(s.data());
    const auto* term = p + s.size();
    while (p < term) result.push_back(read_utf8_bounded(&p, term));
    return result;
}

inline std::string encode_utf8(std::u32string_view s) {
    std::string result;
    char buf[4];
    for (char32_t c : s) {
        int n = append_utf8(buf, static_cast<std::uint32_t>(c));
        result.append(buf, static_cast<std::size_t>(n));
    }
    return result;
}

} // namespace sqlite::utils::text
