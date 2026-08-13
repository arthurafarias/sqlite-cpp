#pragma once

#include "cell.hpp"
#include "page_layout.hpp"
#include "page_store.hpp"
#include "varint.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

// Legacy source: btree.c's insert/delete/balance machinery
// (sqlite3BtreeInsert, sqlite3BtreeDelete, balance_nonroot, balance_deeper;
// SRS S3.3.3). A real, working table B-tree keyed by 64-bit rowid, storing
// opaque byte-string payloads (record serialization -- turning SQL row
// values into those bytes -- is sqlite::core::virtual_machine::aux's job,
// consistent with SRS S3.4.1's Mem-cell-handling assignment; this library
// never interprets payload contents).
//
// Deliberate simplifications vs. legacy balance_nonroot(), each documented
// at the point it matters and summarized in this library's docs:
//   - Splits are a plain 2-way split (no 3-sibling redistribution), so this
//     tree can end up somewhat less full than one balance_nonroot() would
//     produce for the same insert sequence -- still correctly balanced
//     (every leaf at the same depth, O(log n) operations), just not
//     maximally space-efficient.
//   - No merge-on-delete: erase() removes a cell and rewrites its page, but
//     never merges an under-full page with a sibling or shrinks the tree's
//     height back down. A tree that has many rows deleted stays as tall as
//     its largest-ever size.
//   - The root page number is allowed to change when the tree grows a new
//     level (real SQLite keeps the root page number fixed forever, moving
//     the *old* root's content to a new page instead, because a schema
//     entry elsewhere stores that fixed number). There is no schema layer
//     yet to hold such a reference, so this simplification is currently
//     free; a caller must track btree::root_page() itself if it needs
//     stability across inserts.
//   - Index b-trees (arbitrary-key, not rowid-keyed) are not implemented --
//     table b-trees only.
namespace sqlite::backend::tree {

class btree_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class btree {
public:
    // Opens a B-tree rooted at `root_page` in `store` (page 1's freshly
    // formatted empty leaf, for a brand-new page_store).
    explicit btree(page_store& store, std::uint32_t root_page = 1) : store_(store), root_(root_page) {}

    [[nodiscard]] std::uint32_t root_page() const noexcept { return root_; }

    // sqlite3BtreeInsert(): inserts, or replaces if `rowid` already exists
    // (an upsert -- real SQLite's INSERT-vs-UPDATE distinction is a
    // command-processor-level decision this library doesn't make).
    void insert(std::int64_t rowid, const unsigned char* payload, std::uint64_t payload_size) {
        std::uint32_t first_overflow = write_overflow_chain(payload, payload_size);
        int cell_size = leaf_cell_size(rowid, payload_size, store_.usable_size());
        std::vector<unsigned char> cell_bytes(static_cast<std::size_t>(cell_size));
        write_leaf_cell(cell_bytes.data(), rowid, payload, payload_size, store_.usable_size(), first_overflow);

        std::vector<std::uint32_t> path = descend_path(rowid);
        std::uint32_t leaf_pgno = path.back();
        decoded_leaf leaf = read_leaf_page(leaf_pgno);
        upsert_cell(leaf.cells, rowid, std::move(cell_bytes));

        if (leaf_fits(leaf)) {
            write_leaf_page(leaf_pgno, leaf);
        } else {
            split_leaf(path, leaf);
        }
    }

    // sqlite3BtreeDelete(): removes the row with this rowid, if present.
    // Returns whether it was found. Does not merge/rebalance (see the
    // class-level comment).
    bool erase(std::int64_t rowid) {
        std::vector<std::uint32_t> path = descend_path(rowid);
        std::uint32_t leaf_pgno = path.back();
        decoded_leaf leaf = read_leaf_page(leaf_pgno);
        auto it = std::find_if(leaf.cells.begin(), leaf.cells.end(),
                                [&](const auto& c) { return c.first == rowid; });
        if (it == leaf.cells.end()) return false;
        leaf.cells.erase(it);
        write_leaf_page(leaf_pgno, leaf);
        return true;
    }

    // sqlite3BtreeMovetoUnpacked() + payload read: exact-match lookup,
    // reconstructing the full payload across any overflow chain.
    [[nodiscard]] std::optional<std::vector<unsigned char>> find(std::int64_t rowid) {
        std::uint32_t leaf_pgno = descend_path(rowid).back();
        decoded_leaf leaf = read_leaf_page(leaf_pgno);
        for (const auto& [r, bytes] : leaf.cells) {
            if (r == rowid) return read_full_payload(read_leaf_cell(bytes.data(), store_.usable_size()));
        }
        return std::nullopt;
    }

    // sqlite3BtreeMovetoUnpacked() with a bias toward the smallest key >=
    // target: the primitive testing/cursor.hpp's cursor is built on for
    // in-order iteration.
    [[nodiscard]] std::optional<std::pair<std::int64_t, std::vector<unsigned char>>> seek_ge(std::int64_t target) {
        std::uint32_t pgno = root_;
        while (true) {
            std::vector<unsigned char> buf(static_cast<std::size_t>(store_.page_size()));
            store_.read_page(pgno, buf.data());
            int hdr_offset = pgno == 1 ? database_header_size : 0;
            page_header h = page_header::parse(buf.data(), hdr_offset);
            if (h.is_leaf()) {
                decoded_leaf leaf = decode_leaf(buf.data(), h, hdr_offset);
                for (const auto& [r, bytes] : leaf.cells) {
                    if (r >= target) return std::make_pair(r, read_full_payload(read_leaf_cell(bytes.data(), store_.usable_size())));
                }
                return std::nullopt;
            }
            decoded_interior node = decode_interior(buf.data(), h, hdr_offset);
            std::uint32_t child = h.right_most_pointer;
            for (const auto& [c, r] : node.cells) {
                if (r >= target) { child = c; break; }
            }
            pgno = child;
        }
    }

private:
    // A leaf page's cells, decoded to (rowid, raw encoded cell bytes) pairs
    // sorted by rowid -- kept as raw bytes (rather than further decomposed)
    // since re-encoding a leaf cell after an edit would otherwise need to
    // reallocate overflow pages for content that didn't change.
    struct decoded_leaf {
        std::vector<std::pair<std::int64_t, std::vector<unsigned char>>> cells;
    };
    // An interior page's cells, decoded to (child_page, rowid) pairs sorted
    // by rowid, plus the right-most pointer.
    struct decoded_interior {
        std::vector<std::pair<std::uint32_t, std::int64_t>> cells;
        std::uint32_t right_most_pointer;
    };

    static void upsert_cell(std::vector<std::pair<std::int64_t, std::vector<unsigned char>>>& cells,
                             std::int64_t rowid, std::vector<unsigned char> bytes) {
        auto it = std::find_if(cells.begin(), cells.end(), [&](const auto& c) { return c.first == rowid; });
        if (it != cells.end()) { it->second = std::move(bytes); return; }
        auto pos = std::upper_bound(cells.begin(), cells.end(), rowid,
                                     [](std::int64_t r, const auto& c) { return r < c.first; });
        cells.insert(pos, {rowid, std::move(bytes)});
    }

    [[nodiscard]] decoded_leaf decode_leaf(const unsigned char* buf, const page_header& h, int hdr_offset) const {
        decoded_leaf leaf;
        const unsigned char* ptr_array = buf + hdr_offset + h.size_in_bytes();
        for (int i = 0; i < h.cell_count; ++i) {
            std::uint16_t offset = page_header::read_u16(ptr_array + 2 * i);
            leaf_cell_view v = read_leaf_cell(buf + offset, store_.usable_size());
            leaf.cells.emplace_back(v.rowid, std::vector<unsigned char>(buf + offset, buf + offset + v.cell_size));
        }
        return leaf;
    }
    [[nodiscard]] decoded_interior decode_interior(const unsigned char* buf, const page_header& h, int hdr_offset) const {
        decoded_interior node;
        node.right_most_pointer = h.right_most_pointer;
        const unsigned char* ptr_array = buf + hdr_offset + h.size_in_bytes();
        for (int i = 0; i < h.cell_count; ++i) {
            std::uint16_t offset = page_header::read_u16(ptr_array + 2 * i);
            interior_cell_view v = read_interior_cell(buf + offset);
            node.cells.emplace_back(v.child_page, v.rowid);
        }
        return node;
    }

    [[nodiscard]] decoded_leaf read_leaf_page(std::uint32_t pgno) {
        std::vector<unsigned char> buf(static_cast<std::size_t>(store_.page_size()));
        store_.read_page(pgno, buf.data());
        int hdr_offset = pgno == 1 ? database_header_size : 0;
        page_header h = page_header::parse(buf.data(), hdr_offset);
        if (!h.is_leaf()) throw btree_error("expected a leaf page");
        return decode_leaf(buf.data(), h, hdr_offset);
    }
    [[nodiscard]] decoded_interior read_interior_page(std::uint32_t pgno) {
        std::vector<unsigned char> buf(static_cast<std::size_t>(store_.page_size()));
        store_.read_page(pgno, buf.data());
        int hdr_offset = pgno == 1 ? database_header_size : 0;
        page_header h = page_header::parse(buf.data(), hdr_offset);
        if (h.is_leaf()) throw btree_error("expected an interior page");
        return decode_interior(buf.data(), h, hdr_offset);
    }

    // Both fit checks conservatively assume the worst-case header offset
    // (page 1's, which loses database_header_size bytes to the database
    // header) regardless of which page number is actually being written --
    // wasting a few bytes of capacity on every other page, but avoiding a
    // page-1-specific fit bug where a page that fits by a page-1-unaware
    // check could overrun into its own cell-pointer array once actually
    // written at hdr_offset=100.
    [[nodiscard]] bool leaf_fits(const decoded_leaf& leaf) const {
        int total = database_header_size + 8 + 2 * static_cast<int>(leaf.cells.size());
        for (const auto& [r, bytes] : leaf.cells) { (void)r; total += static_cast<int>(bytes.size()); }
        return total <= store_.usable_size();
    }
    [[nodiscard]] bool interior_fits(const decoded_interior& node) const {
        int total = database_header_size + 12 + 2 * static_cast<int>(node.cells.size());
        for (const auto& [c, r] : node.cells) {
            (void)c;
            total += interior_cell_size(r);
        }
        return total <= store_.usable_size();
    }

    void write_leaf_page(std::uint32_t pgno, const decoded_leaf& leaf) {
        std::vector<unsigned char> buf(static_cast<std::size_t>(store_.page_size()), 0);
        int hdr_offset = pgno == 1 ? database_header_size : 0;
        // This buffer is built from scratch every time (no partial-page
        // in-place editing -- see this file's header comment), so page 1's
        // database header (bytes [0, database_header_size)) must be
        // re-populated here or it would be silently zeroed on every write.
        if (pgno == 1) store_.format_header(buf.data());
        page_header h{page_type::leaf_table, 0, static_cast<std::uint16_t>(leaf.cells.size()), 0, 0, 0};
        int content_start = store_.page_size();
        for (const auto& [r, bytes] : leaf.cells) { (void)r; content_start -= static_cast<int>(bytes.size()); }
        h.cell_content_start = content_start >= 65536 ? 0 : static_cast<std::uint16_t>(content_start);
        h.write(buf.data(), hdr_offset);

        unsigned char* ptr_array = buf.data() + hdr_offset + h.size_in_bytes();
        int cursor = content_start;
        for (std::size_t i = 0; i < leaf.cells.size(); ++i) {
            const auto& bytes = leaf.cells[i].second;
            std::memcpy(buf.data() + cursor, bytes.data(), bytes.size());
            page_header::write_u16(ptr_array + 2 * i, static_cast<std::uint16_t>(cursor));
            cursor += static_cast<int>(bytes.size());
        }
        store_.write_page(pgno, buf.data());
    }

    void write_interior_page(std::uint32_t pgno, const decoded_interior& node) {
        std::vector<unsigned char> buf(static_cast<std::size_t>(store_.page_size()), 0);
        int hdr_offset = pgno == 1 ? database_header_size : 0;
        if (pgno == 1) store_.format_header(buf.data());  // see write_leaf_page()'s comment
        page_header h{page_type::interior_table, 0, static_cast<std::uint16_t>(node.cells.size()), 0, 0, node.right_most_pointer};

        std::vector<std::vector<unsigned char>> encoded;
        encoded.reserve(node.cells.size());
        int content_size = 0;
        for (const auto& [c, r] : node.cells) {
            std::vector<unsigned char> bytes(static_cast<std::size_t>(interior_cell_size(r)));
            write_interior_cell(bytes.data(), c, r);
            content_size += static_cast<int>(bytes.size());
            encoded.push_back(std::move(bytes));
        }
        int content_start = store_.page_size() - content_size;
        h.cell_content_start = content_start >= 65536 ? 0 : static_cast<std::uint16_t>(content_start);
        h.write(buf.data(), hdr_offset);

        unsigned char* ptr_array = buf.data() + hdr_offset + h.size_in_bytes();
        int cursor = content_start;
        for (std::size_t i = 0; i < encoded.size(); ++i) {
            std::memcpy(buf.data() + cursor, encoded[i].data(), encoded[i].size());
            page_header::write_u16(ptr_array + 2 * i, static_cast<std::uint16_t>(cursor));
            cursor += static_cast<int>(encoded[i].size());
        }
        store_.write_page(pgno, buf.data());
    }

    // Root-to-leaf descent for `rowid`, recording every page number visited
    // (path.back() is always the leaf).
    [[nodiscard]] std::vector<std::uint32_t> descend_path(std::int64_t rowid) {
        std::vector<std::uint32_t> path;
        std::uint32_t pgno = root_;
        while (true) {
            path.push_back(pgno);
            std::vector<unsigned char> buf(static_cast<std::size_t>(store_.page_size()));
            store_.read_page(pgno, buf.data());
            int hdr_offset = pgno == 1 ? database_header_size : 0;
            page_header h = page_header::parse(buf.data(), hdr_offset);
            if (h.is_leaf()) break;
            decoded_interior node = decode_interior(buf.data(), h, hdr_offset);
            std::uint32_t child = h.right_most_pointer;
            for (const auto& [c, r] : node.cells) {
                if (r >= rowid) { child = c; break; }
            }
            pgno = child;
        }
        return path;
    }

    void split_leaf(std::vector<std::uint32_t>& path, decoded_leaf& leaf) {
        std::size_t k = leaf.cells.size() / 2;
        decoded_leaf left, right;
        left.cells.assign(leaf.cells.begin(), leaf.cells.begin() + static_cast<long>(k));
        right.cells.assign(leaf.cells.begin() + static_cast<long>(k), leaf.cells.end());

        std::uint32_t left_pgno = path.back();
        write_leaf_page(left_pgno, left);
        std::uint32_t right_pgno = store_.allocate_page();
        write_leaf_page(right_pgno, right);

        std::int64_t divider = left.cells.back().first;
        path.pop_back();
        propagate_split(path, left_pgno, right_pgno, divider);
    }

    void split_interior(std::vector<std::uint32_t>& path, decoded_interior& node) {
        std::size_t k = node.cells.size() / 2;
        // node.cells[k] is the boundary cell: its child becomes the left
        // page's right-most pointer (everything up to and including its
        // rowid belongs to the left subtree, but an interior page's cells
        // only ever express "strictly less than the top of this page's
        // range" -- the top itself is always the right-most pointer) and
        // its rowid is the divider propagated up to the parent.
        // node.cells[0..k) stay as the left page's own cells;
        // node.cells[k+1..end) become the right page's cells.
        decoded_interior left, right;
        left.cells.assign(node.cells.begin(), node.cells.begin() + static_cast<long>(k));
        left.right_most_pointer = node.cells[k].first;
        std::int64_t divider = node.cells[k].second;
        right.cells.assign(node.cells.begin() + static_cast<long>(k) + 1, node.cells.end());
        right.right_most_pointer = node.right_most_pointer;

        std::uint32_t left_pgno = path.back();
        write_interior_page(left_pgno, left);
        std::uint32_t right_pgno = store_.allocate_page();
        write_interior_page(right_pgno, right);

        path.pop_back();
        propagate_split(path, left_pgno, right_pgno, divider);
    }

    // Updates the parent (path.back(), or creates a new root if path is now
    // empty) so that `left_pgno` (already written, holding the smaller
    // half) and `right_pgno` (already written, holding the larger half,
    // including whatever `left_pgno` used to solely represent) are both
    // correctly reachable, with `divider` as the largest rowid in
    // `left_pgno`'s subtree.
    void propagate_split(std::vector<std::uint32_t>& path, std::uint32_t left_pgno,
                          std::uint32_t right_pgno, std::int64_t divider) {
        if (path.empty()) {
            decoded_interior new_root;
            new_root.cells.push_back({left_pgno, divider});
            new_root.right_most_pointer = right_pgno;
            std::uint32_t new_root_pgno = store_.allocate_page();
            write_interior_page(new_root_pgno, new_root);
            root_ = new_root_pgno;
            return;
        }
        std::uint32_t parent_pgno = path.back();
        decoded_interior parent = read_interior_page(parent_pgno);
        bool retargeted = false;
        for (auto& [c, r] : parent.cells) {
            if (c == left_pgno) { c = right_pgno; retargeted = true; break; }
        }
        if (!retargeted) {
            if (parent.right_most_pointer != left_pgno) {
                throw btree_error("propagate_split: parent does not reference the split child");
            }
            parent.right_most_pointer = right_pgno;
        }
        auto pos = std::upper_bound(parent.cells.begin(), parent.cells.end(), divider,
                                     [](std::int64_t r, const auto& c) { return r < c.second; });
        parent.cells.insert(pos, {left_pgno, divider});

        if (interior_fits(parent)) {
            write_interior_page(parent_pgno, parent);
        } else {
            split_interior(path, parent);
        }
    }

    [[nodiscard]] std::uint32_t write_overflow_chain(const unsigned char* payload, std::uint64_t payload_size) {
        local_split split = compute_local_split(payload_size, store_.usable_size());
        if (!split.has_overflow) return 0;

        std::uint64_t remaining = payload_size - static_cast<std::uint64_t>(split.local_size);
        const unsigned char* base = payload + split.local_size;
        int capacity = store_.usable_size() - overflow_page_header_size;

        // Chunks in forward order: chunk_ranges[0] is the first `capacity`
        // bytes right after the local portion, chunk_ranges[1] the next,
        // and so on.
        std::vector<std::pair<std::uint64_t, std::uint64_t>> chunk_ranges;  // (offset, length) into `base`
        std::uint64_t offset = 0;
        while (offset < remaining) {
            std::uint64_t len = (remaining - offset) < static_cast<std::uint64_t>(capacity)
                                     ? (remaining - offset) : static_cast<std::uint64_t>(capacity);
            chunk_ranges.emplace_back(offset, len);
            offset += len;
        }

        // Allocated/written in *reverse* order, so that by the time each
        // page is written, the page number of the chunk that follows it
        // (already processed, since we're going backwards) is known and can
        // go straight into that page's header -- there is no way to know a
        // forward-chain "next" pointer before the next page exists.
        std::uint32_t next_pgno = 0;
        for (auto it = chunk_ranges.rbegin(); it != chunk_ranges.rend(); ++it) {
            std::uint32_t pgno = store_.allocate_page();
            std::vector<unsigned char> page(static_cast<std::size_t>(store_.page_size()), 0);
            page_header::write_u32(page.data(), next_pgno);
            std::memcpy(page.data() + overflow_page_header_size, base + it->first, it->second);
            store_.write_page(pgno, page.data());
            next_pgno = pgno;
        }
        return next_pgno;  // the page holding chunk_ranges[0] -- the first chunk -- written last
    }

    [[nodiscard]] std::vector<unsigned char> read_full_payload(const leaf_cell_view& v) {
        std::vector<unsigned char> out(v.local_payload, v.local_payload + v.local_size);
        std::uint32_t pgno = v.first_overflow_page;
        int capacity = store_.usable_size() - overflow_page_header_size;
        while (pgno != 0) {
            std::vector<unsigned char> page(static_cast<std::size_t>(store_.page_size()));
            store_.read_page(pgno, page.data());
            std::uint32_t next = page_header::read_u32(page.data());
            std::uint64_t remaining = v.payload_size - out.size();
            std::size_t take = remaining < static_cast<std::uint64_t>(capacity) ? static_cast<std::size_t>(remaining)
                                                                                  : static_cast<std::size_t>(capacity);
            out.insert(out.end(), page.begin() + overflow_page_header_size,
                       page.begin() + overflow_page_header_size + static_cast<long>(take));
            pgno = next;
        }
        return out;
    }

    page_store& store_;
    std::uint32_t root_;
};

} // namespace sqlite::backend::tree
