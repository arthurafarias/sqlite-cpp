#pragma once

#include <cstddef>
#include <cstdint>

// Legacy source: util.c's sqlite3GetVarint()/sqlite3PutVarint() (SRS
// S3.3.3). SQLite's on-disk variable-length integer encoding: 1 to 9 bytes,
// big-endian base-128 with a continuation bit (0x80) set on every byte
// except the last, except that the 9th byte (when present) uses all 8 bits
// with no continuation bit, since 8*8+8=64 bits is exactly enough for any
// u64. This is part of the real on-disk format -- used by every cell's
// payload-length and rowid fields (cell.hpp) -- so it's ported bit-for-bit,
// not reinterpreted.
namespace sqlite::backend::tree {

// Encodes v into out[0..9), returns the number of bytes written (1-9).
inline int put_varint(unsigned char* out, std::uint64_t v) {
    if (v <= 0x7f) {
        out[0] = static_cast<unsigned char>(v);
        return 1;
    }
    if (v <= 0x3fff) {
        out[0] = static_cast<unsigned char>(((v >> 7) & 0x7f) | 0x80);
        out[1] = static_cast<unsigned char>(v & 0x7f);
        return 2;
    }
    if (v & (std::uint64_t{0xff000000} << 32)) {
        // Needs the full 9-byte form: 8 base-128 digits (56 bits) plus one
        // raw byte for the low 8 bits.
        out[8] = static_cast<unsigned char>(v);
        v >>= 8;
        for (int i = 7; i >= 0; --i) {
            out[i] = static_cast<unsigned char>((v & 0x7f) | 0x80);
            v >>= 7;
        }
        return 9;
    }
    unsigned char buf[9];
    int n = 0;
    do {
        buf[n++] = static_cast<unsigned char>((v & 0x7f) | 0x80);
        v >>= 7;
    } while (v != 0);
    buf[0] &= 0x7f;
    for (int i = 0, j = n - 1; j >= 0; --j, ++i) out[i] = buf[j];
    return n;
}

// Decodes a varint starting at p, writes the value to *out, and returns the
// number of bytes consumed (1-9). Caller must ensure at least 9 bytes are
// readable at p (SQLite's own format guarantee: a varint is never split
// across a page boundary in a way that would read past a 9-byte max).
inline int get_varint(const unsigned char* p, std::uint64_t* out) {
    if (p[0] < 0x80) {
        *out = p[0];
        return 1;
    }
    std::uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        v = (v << 7) | (p[i] & 0x7f);
        if ((p[i] & 0x80) == 0) {
            *out = v;
            return i + 1;
        }
    }
    // 9th byte: all 8 bits, no continuation semantics.
    v = (v << 8) | p[8];
    *out = v;
    return 9;
}

// Returns the number of bytes put_varint(v) would write, without writing
// anything -- used to size cells before allocating/copying into a page.
inline int varint_length(std::uint64_t v) {
    if (v <= 0x7f) return 1;
    if (v <= 0x3fff) return 2;
    int n = 0;
    for (std::uint64_t x = v; x != 0; x >>= 7) ++n;
    return n < 9 ? n : 9;
}

} // namespace sqlite::backend::tree
