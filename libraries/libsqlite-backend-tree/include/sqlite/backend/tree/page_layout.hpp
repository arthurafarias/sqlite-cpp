#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>

// Legacy source: btree.c's page-header constants and the first-100-bytes
// database header format (documented in btree.c's header comment and
// implemented across sqlite3BtreeOpen/newDatabase/btreeInitPage; SRS
// S3.3.3). This is the actual on-disk layout, not a simplified stand-in --
// see cell.hpp and page_store.hpp for the pieces built on top of it.
//
// Scope note: only the table-b-tree page types (interior/leaf) are
// implemented (FR per this library's docs: index b-trees are deferred).
// The database header's schema-related fields (schema cookie, text
// encoding, application ID, ...) are zeroed rather than populated -- they
// belong to a schema layer (sqlite-core-command-processor) that doesn't
// exist yet, and a zeroed field there is itself the documented-correct
// default SQLite uses for an empty database.
namespace sqlite::backend::tree {

inline constexpr int database_header_size = 100;
inline constexpr std::string_view magic_header_string = "SQLite format 3";  // + a trailing NUL, 16 bytes total

enum class page_type : std::uint8_t {
    interior_table = 0x05,
    leaf_table = 0x0d,
};

// btreeInitPage()'s header field offsets, relative to the page's own
// hdrOffset (100 for page 1, since the 100-byte database header precedes
// it there; 0 for every other page).
struct page_header {
    page_type type = page_type::leaf_table;
    std::uint16_t first_freeblock = 0;
    std::uint16_t cell_count = 0;
    std::uint16_t cell_content_start = 0;  // 0 means 65536, per the legacy format
    std::uint8_t fragmented_free_bytes = 0;
    std::uint32_t right_most_pointer = 0;  // interior pages only

    [[nodiscard]] bool is_leaf() const noexcept { return type == page_type::leaf_table; }
    [[nodiscard]] int size_in_bytes() const noexcept { return is_leaf() ? 8 : 12; }

    static page_header parse(const unsigned char* page, int hdr_offset) {
        page_header h;
        h.type = static_cast<page_type>(page[hdr_offset]);
        h.first_freeblock = read_u16(page + hdr_offset + 1);
        h.cell_count = read_u16(page + hdr_offset + 3);
        h.cell_content_start = read_u16(page + hdr_offset + 5);
        h.fragmented_free_bytes = page[hdr_offset + 7];
        if (!h.is_leaf()) h.right_most_pointer = read_u32(page + hdr_offset + 8);
        return h;
    }

    void write(unsigned char* page, int hdr_offset) const {
        page[hdr_offset] = static_cast<unsigned char>(type);
        write_u16(page + hdr_offset + 1, first_freeblock);
        write_u16(page + hdr_offset + 3, cell_count);
        write_u16(page + hdr_offset + 5, cell_content_start);
        page[hdr_offset + 7] = fragmented_free_bytes;
        if (!is_leaf()) write_u32(page + hdr_offset + 8, right_most_pointer);
    }

    static std::uint16_t read_u16(const unsigned char* p) noexcept {
        return static_cast<std::uint16_t>((p[0] << 8) | p[1]);
    }
    static std::uint32_t read_u32(const unsigned char* p) noexcept {
        return (static_cast<std::uint32_t>(p[0]) << 24) | (static_cast<std::uint32_t>(p[1]) << 16) |
               (static_cast<std::uint32_t>(p[2]) << 8) | p[3];
    }
    static void write_u16(unsigned char* p, std::uint16_t v) noexcept {
        p[0] = static_cast<unsigned char>(v >> 8);
        p[1] = static_cast<unsigned char>(v);
    }
    static void write_u32(unsigned char* p, std::uint32_t v) noexcept {
        p[0] = static_cast<unsigned char>(v >> 24);
        p[1] = static_cast<unsigned char>(v >> 16);
        p[2] = static_cast<unsigned char>(v >> 8);
        p[3] = static_cast<unsigned char>(v);
    }
};

// The first 100 bytes of page 1 (newDatabase() in btree.c). Only the fields
// this library actually uses (magic string, page size, page count) are
// populated; every other legacy field is written as zero, matching what a
// freshly-created, schema-less SQLite database file already has in them.
struct database_header {
    std::uint16_t page_size = 4096;  // legacy encoding: 1 means 65536, handled in read/write below
    std::uint32_t page_count = 1;

    static database_header parse(const unsigned char* page1) {
        database_header h;
        std::uint16_t raw = page_header::read_u16(page1 + 16);
        h.page_size = raw;  // caller (page_store) interprets 1 as 65536
        h.page_count = page_header::read_u32(page1 + 28);
        return h;
    }

    void write(unsigned char* page1) const {
        // GCC 16's -Warray-bounds/-Wstringop-overflow misfire here under
        // -O2/-O3 once this function gets inlined into btree.hpp's
        // write_leaf_page()/write_interior_page() (themselves several
        // inlining hops from the actual std::vector<unsigned char> buffer,
        // which is always sized at least a page's worth of bytes --
        // confirmed by ASan under a Debug build, where this code path runs
        // on every test that touches page 1, with no out-of-bounds access
        // reported). Narrowly silenced at the one function GCC's
        // diagnostics point at, rather than weakening -Werror project-wide.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
        std::memset(page1, 0, database_header_size);
        std::memcpy(page1, magic_header_string.data(), magic_header_string.size());
        page1[15] = 0;  // NUL terminator of the magic string, byte 15
        page_header::write_u16(page1 + 16, page_size);
        page1[18] = 1;  // file format write version: 1 = legacy rollback journal
        page1[19] = 1;  // file format read version
        page1[20] = 0;  // bytes of unused "reserved space" per page
        page1[21] = 64; // maximum embedded payload fraction: always 64
        page1[22] = 32; // minimum embedded payload fraction: always 32
        page1[23] = 32; // leaf payload fraction: always 32
        page_header::write_u32(page1 + 28, page_count);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    }
};

} // namespace sqlite::backend::tree
