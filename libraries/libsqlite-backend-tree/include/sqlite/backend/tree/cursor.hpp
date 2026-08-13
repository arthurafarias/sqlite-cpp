#pragma once

#include "btree.hpp"

#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

// Legacy source: btree.c's BtCursor / sqlite3BtreeNext() (SRS S3.3.3).
// In-order iteration over a btree's (rowid, payload) pairs.
//
// Deliberate simplification: real BtCursor holds a live root-to-leaf page
// stack and moves through it directly, so advancing to the next row is
// O(1) amortized. This cursor instead re-seeks from the root on every
// next() call (seek_ge(current_rowid + 1)) -- still O(log n) per step, the
// same asymptotic complexity a balanced tree guarantees either way, just
// with a larger constant factor from re-descending each time rather than
// reusing the previous descent's path.
namespace sqlite::backend::tree {

class cursor {
public:
    explicit cursor(btree& tree) : tree_(tree) { current_ = tree_.seek_ge(std::numeric_limits<std::int64_t>::min()); }

    [[nodiscard]] bool eof() const noexcept { return !current_.has_value(); }
    [[nodiscard]] std::int64_t rowid() const { return current_->first; }
    [[nodiscard]] const std::vector<unsigned char>& payload() const { return current_->second; }

    void next() {
        if (!current_) return;
        if (current_->first == std::numeric_limits<std::int64_t>::max()) {
            current_.reset();
            return;
        }
        current_ = tree_.seek_ge(current_->first + 1);
    }

    // Repositions to the smallest rowid >= target (sqlite3BtreeMovetoUnpacked()).
    void seek(std::int64_t target) { current_ = tree_.seek_ge(target); }

private:
    btree& tree_;
    std::optional<std::pair<std::int64_t, std::vector<unsigned char>>> current_;
};

} // namespace sqlite::backend::tree
