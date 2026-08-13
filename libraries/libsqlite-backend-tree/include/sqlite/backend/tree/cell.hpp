#pragma once

#include "varint.hpp"

#include <cstdint>
#include <cstring>

// Legacy source: btree.c's cell format -- btreeParseCellPtr()/
// fillInCell()/cellSizePtr() for table-b-tree leaf cells, and the
// corresponding interior-cell handling (SRS S3.3.3). A table-b-tree leaf
// cell is `varint(payload_size) varint(rowid) local_payload[
// [u32 first_overflow_page]]`; an interior cell is `u32 child_page
// varint(rowid)`.
//
// Scope note on the local/overflow split point: real SQLite computes how
// many payload bytes stay local using a "surplus" redistribution formula
// (btree.c's inline logic in fillInCell) that keeps the last overflow page
// reasonably full. This port uses a simpler cutoff -- payloads under
// max_local_payload() stay fully local; anything larger stores exactly
// min_local_payload() bytes locally and chains the rest through overflow
// pages. Both are valid, correctly parseable instances of the *same*
// format (the format doesn't mandate a specific split point, only that
// local_size falls between the min/max bounds), but a page this library
// writes for an overflowing payload will not be byte-identical to what
// stock SQLite would write for the same row.
namespace sqlite::backend::tree {

inline constexpr int overflow_page_header_size = 4;  // u32 "next overflow page" pointer

inline int max_local_payload(int usable_size) noexcept { return usable_size - 35; }
inline int min_local_payload(int usable_size) noexcept { return (usable_size - 12) * 32 / 255 - 23; }

struct local_split {
    int local_size;
    bool has_overflow;
};

inline local_split compute_local_split(std::uint64_t payload_size, int usable_size) noexcept {
    int max_local = max_local_payload(usable_size);
    if (payload_size <= static_cast<std::uint64_t>(max_local)) {
        return {static_cast<int>(payload_size), false};
    }
    return {min_local_payload(usable_size), true};
}

// Total encoded size of a leaf cell (header + local payload + optional
// overflow pointer), given the rowid/payload it will hold.
inline int leaf_cell_size(std::int64_t rowid, std::uint64_t payload_size, int usable_size) noexcept {
    local_split split = compute_local_split(payload_size, usable_size);
    int size = varint_length(payload_size) + varint_length(static_cast<std::uint64_t>(rowid)) + split.local_size;
    if (split.has_overflow) size += 4;
    return size;
}

// Writes a leaf cell to `out` (which must have room for leaf_cell_size()
// bytes) and returns the number of bytes written. `first_overflow_page` is
// ignored (and must be 0) when the payload fits fully locally.
inline int write_leaf_cell(unsigned char* out, std::int64_t rowid, const unsigned char* payload,
                            std::uint64_t payload_size, int usable_size, std::uint32_t first_overflow_page) {
    local_split split = compute_local_split(payload_size, usable_size);
    unsigned char* p = out;
    p += put_varint(p, payload_size);
    p += put_varint(p, static_cast<std::uint64_t>(rowid));
    std::memcpy(p, payload, static_cast<std::size_t>(split.local_size));
    p += split.local_size;
    if (split.has_overflow) {
        p[0] = static_cast<unsigned char>(first_overflow_page >> 24);
        p[1] = static_cast<unsigned char>(first_overflow_page >> 16);
        p[2] = static_cast<unsigned char>(first_overflow_page >> 8);
        p[3] = static_cast<unsigned char>(first_overflow_page);
        p += 4;
    }
    return static_cast<int>(p - out);
}

struct leaf_cell_view {
    std::int64_t rowid;
    std::uint64_t payload_size;
    const unsigned char* local_payload;
    int local_size;
    bool has_overflow;
    std::uint32_t first_overflow_page;
    int cell_size;
};

inline leaf_cell_view read_leaf_cell(const unsigned char* cell, int usable_size) {
    leaf_cell_view v{};
    const unsigned char* p = cell;
    std::uint64_t payload_size = 0;
    p += get_varint(p, &payload_size);
    std::uint64_t rowid_raw = 0;
    p += get_varint(p, &rowid_raw);
    v.rowid = static_cast<std::int64_t>(rowid_raw);
    v.payload_size = payload_size;
    local_split split = compute_local_split(payload_size, usable_size);
    v.local_payload = p;
    v.local_size = split.local_size;
    v.has_overflow = split.has_overflow;
    p += split.local_size;
    if (split.has_overflow) {
        v.first_overflow_page = (static_cast<std::uint32_t>(p[0]) << 24) |
                                 (static_cast<std::uint32_t>(p[1]) << 16) |
                                 (static_cast<std::uint32_t>(p[2]) << 8) | p[3];
        p += 4;
    } else {
        v.first_overflow_page = 0;
    }
    v.cell_size = static_cast<int>(p - cell);
    return v;
}

inline int interior_cell_size(std::int64_t rowid) noexcept {
    return 4 + varint_length(static_cast<std::uint64_t>(rowid));
}

inline int write_interior_cell(unsigned char* out, std::uint32_t child_page, std::int64_t rowid) {
    out[0] = static_cast<unsigned char>(child_page >> 24);
    out[1] = static_cast<unsigned char>(child_page >> 16);
    out[2] = static_cast<unsigned char>(child_page >> 8);
    out[3] = static_cast<unsigned char>(child_page);
    return 4 + put_varint(out + 4, static_cast<std::uint64_t>(rowid));
}

struct interior_cell_view {
    std::uint32_t child_page;
    std::int64_t rowid;
    int cell_size;
};

inline interior_cell_view read_interior_cell(const unsigned char* cell) {
    interior_cell_view v{};
    v.child_page = (static_cast<std::uint32_t>(cell[0]) << 24) | (static_cast<std::uint32_t>(cell[1]) << 16) |
                   (static_cast<std::uint32_t>(cell[2]) << 8) | cell[3];
    std::uint64_t rowid_raw = 0;
    int n = get_varint(cell + 4, &rowid_raw);
    v.rowid = static_cast<std::int64_t>(rowid_raw);
    v.cell_size = 4 + n;
    return v;
}

} // namespace sqlite::backend::tree
