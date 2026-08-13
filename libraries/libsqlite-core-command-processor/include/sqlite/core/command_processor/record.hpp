#pragma once

#include <sqlite/backend/tree/varint.hpp>
#include <sqlite/core/virtual_machine/aux/mem.hpp>

#include <cstdint>
#include <cstring>
#include <vector>

// Legacy source: vdbeaux.c's sqlite3VdbeSerialType()/SerialTypeLen()/
// SerialPut()/SerialGet() -- SQLite's real on-disk row-record format: a
// header (a varint giving the header's own total length, then one varint
// per column giving its "serial type") followed by the column values'
// bytes back to back, in the same order. This is what
// sqlite::backend::tree::btree::insert()'s opaque payload actually holds
// once this library uses it -- real format, not a simplification, ported
// bit-for-bit (see this library's docs for why it lives here rather than
// in sqlite-core-virtual-machine::aux, vdbeaux.c's other legacy home).
//
// Reuses sqlite::backend::tree's varint encode/decode directly rather than
// re-implementing it -- it's the same on-disk integer encoding either way.
namespace sqlite::core::command_processor::record {

namespace tree = sqlite::backend::tree;
namespace vm = sqlite::core::virtual_machine;

namespace detail {

// sqlite3VdbeSerialType() for the integer case: the smallest of the six
// signed-integer widths that losslessly holds v, or the zero-byte 0/1
// special cases.
inline int integer_serial_type(std::int64_t v) {
    if (v == 0) return 8;
    if (v == 1) return 9;
    if (v >= -128 && v <= 127) return 1;
    if (v >= -32768 && v <= 32767) return 2;
    if (v >= -8388608 && v <= 8388607) return 3;
    if (v >= -2147483648LL && v <= 2147483647LL) return 4;
    if (v >= -(std::int64_t{1} << 47) && v < (std::int64_t{1} << 47)) return 5;
    return 6;
}

inline int serial_type_length(int serial_type) {
    switch (serial_type) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
        case 5: return 6;
        case 6: return 8;
        case 7: return 8;
        case 8: case 9: return 0;
        default:
            if (serial_type >= 12 && serial_type % 2 == 0) return (serial_type - 12) / 2; // blob
            return (serial_type - 13) / 2; // text (odd, >= 13)
    }
}

inline int serial_type_for(const vm::aux::mem& m) {
    switch (m.type()) {
        case vm::aux::value_type::null: return 0;
        case vm::aux::value_type::integer: return integer_serial_type(m.as_integer());
        case vm::aux::value_type::real: return 7;
        case vm::aux::value_type::text: return static_cast<int>(m.as_text().size()) * 2 + 13;
        case vm::aux::value_type::blob: return static_cast<int>(m.as_blob_if()->size()) * 2 + 12;
    }
    return 0;
}

inline void write_be_u64(unsigned char* out, std::uint64_t v, int n) {
    for (int i = 0; i < n; ++i) out[n - 1 - i] = static_cast<unsigned char>((v >> (8 * i)) & 0xff);
}

inline std::uint64_t read_be_u64(const unsigned char* p, int n) {
    std::uint64_t v = 0;
    for (int i = 0; i < n; ++i) v = (v << 8) | p[i];
    return v;
}

// Sign-extends an n-byte big-endian two's-complement integer. For n==8
// (serial type 6, the 64-bit case), read_be_u64() already reconstructs the
// exact bit pattern, so no extension is needed -- and `<<64` would be
// undefined behavior (shift amount >= the operand's width) if attempted.
inline std::int64_t read_be_signed(const unsigned char* p, int n) {
    std::uint64_t v = read_be_u64(p, n);
    int bits = n * 8;
    if (bits < 64) {
        std::uint64_t sign_bit = std::uint64_t{1} << (bits - 1);
        if (v & sign_bit) v |= (~std::uint64_t{0} << bits);
    }
    return static_cast<std::int64_t>(v);
}

inline void serialize_value(unsigned char* out, const vm::aux::mem& m, int serial_type) {
    switch (serial_type) {
        case 0: case 8: case 9:
            return; // no bytes stored
        case 1: case 2: case 3: case 4: case 5: case 6:
            write_be_u64(out, static_cast<std::uint64_t>(m.as_integer()), serial_type_length(serial_type));
            return;
        case 7: {
            double d = m.as_real();
            std::uint64_t bits;
            std::memcpy(&bits, &d, sizeof(bits));
            write_be_u64(out, bits, 8);
            return;
        }
        default:
            if (serial_type % 2 == 0) {
                const auto* b = m.as_blob_if();
                std::memcpy(out, b->data(), b->size());
            } else {
                std::string t = m.as_text();
                std::memcpy(out, t.data(), t.size());
            }
    }
}

inline vm::aux::mem deserialize_value(const unsigned char* p, int serial_type) {
    switch (serial_type) {
        case 0: return vm::aux::mem::make_null();
        case 8: return vm::aux::mem::make_integer(0);
        case 9: return vm::aux::mem::make_integer(1);
        case 1: case 2: case 3: case 4: case 5: case 6:
            return vm::aux::mem::make_integer(read_be_signed(p, serial_type_length(serial_type)));
        case 7: {
            std::uint64_t bits = read_be_u64(p, 8);
            double d;
            std::memcpy(&d, &bits, sizeof(d));
            return vm::aux::mem::make_real(d);
        }
        default: {
            int len = serial_type_length(serial_type);
            if (serial_type % 2 == 0) {
                std::vector<std::byte> b(static_cast<std::size_t>(len));
                std::memcpy(b.data(), p, static_cast<std::size_t>(len));
                return vm::aux::mem::make_blob(std::move(b));
            }
            return vm::aux::mem::make_text(std::string(reinterpret_cast<const char*>(p), static_cast<std::size_t>(len)));
        }
    }
}

// The header-length field is itself a varint whose encoded length
// contributes to the total it describes -- find the fixed point the same
// way legacy's sqlite3VdbeMakeReady-adjacent code effectively does.
inline int compute_header_length(int header_body_len) {
    int varint_size = 1;
    for (;;) {
        int candidate = varint_size + header_body_len;
        int actual = tree::varint_length(static_cast<std::uint64_t>(candidate));
        if (actual == varint_size) return candidate;
        varint_size = actual;
    }
}

} // namespace detail

inline std::vector<unsigned char> encode_record(const std::vector<vm::aux::mem>& values) {
    std::vector<int> serial_types;
    serial_types.reserve(values.size());
    int header_body_len = 0;
    int data_len = 0;
    for (const auto& v : values) {
        int st = detail::serial_type_for(v);
        serial_types.push_back(st);
        header_body_len += tree::varint_length(static_cast<std::uint64_t>(st));
        data_len += detail::serial_type_length(st);
    }
    int header_len = detail::compute_header_length(header_body_len);

    std::vector<unsigned char> out(static_cast<std::size_t>(header_len + data_len));
    unsigned char* p = out.data();
    p += tree::put_varint(p, static_cast<std::uint64_t>(header_len));
    for (int st : serial_types) p += tree::put_varint(p, static_cast<std::uint64_t>(st));
    for (std::size_t i = 0; i < values.size(); ++i) {
        detail::serialize_value(p, values[i], serial_types[i]);
        p += detail::serial_type_length(serial_types[i]);
    }
    return out;
}

// Padded internally so get_varint()'s worst-case 9-byte read never runs
// past the buffer even when the last header varint sits at the very end.
inline std::vector<vm::aux::mem> decode_record(const std::vector<unsigned char>& payload) {
    std::vector<unsigned char> padded = payload;
    padded.resize(payload.size() + 8, 0);
    const unsigned char* data = padded.data();

    std::uint64_t header_len_u = 0;
    std::size_t pos = static_cast<std::size_t>(tree::get_varint(data, &header_len_u));
    std::size_t header_len = static_cast<std::size_t>(header_len_u);

    std::vector<int> serial_types;
    while (pos < header_len) {
        std::uint64_t st = 0;
        pos += static_cast<std::size_t>(tree::get_varint(data + pos, &st));
        serial_types.push_back(static_cast<int>(st));
    }

    std::vector<vm::aux::mem> out;
    out.reserve(serial_types.size());
    std::size_t body_pos = header_len;
    for (int st : serial_types) {
        out.push_back(detail::deserialize_value(data + body_pos, st));
        body_pos += static_cast<std::size_t>(detail::serial_type_length(st));
    }
    return out;
}

} // namespace sqlite::core::command_processor::record
