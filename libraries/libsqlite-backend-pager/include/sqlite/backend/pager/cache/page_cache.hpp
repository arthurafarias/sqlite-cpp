#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Legacy source: pcache.c/pcache1.c -- the in-memory page cache every
// Pager read/write goes through, so a page touched repeatedly within a
// transaction (extremely common -- e.g. a B-tree root touched by nearly
// every operation) costs one syscall total, not one per access. That gap
// is exactly what sqlite-backend-tree's own benchmark flagged when it
// measured this workspace's `page_store` (no cache at all) against
// `libsqlite3` -- see that library's docs/index.md, "The pager stand-in".
//
// Simplified vs. legacy: unconditional retention, no eviction. Real
// pcache1.c bounds memory use (SQLITE_DEFAULT_CACHE_SIZE) and evicts
// clean (non-dirty) pages under pressure using an LRU-ish policy. This
// port's test/benchmark workloads are far too small for unbounded growth
// to matter, and skipping eviction is what actually closes the
// syscall-per-operation gap -- adding a real eviction policy is a natural
// follow-up once a workload exists that would actually exhaust memory
// without one.
namespace sqlite::backend::pager::cache {

class page_cache {
public:
    [[nodiscard]] bool contains(std::uint32_t pgno) const noexcept { return pages_.contains(pgno); }

    [[nodiscard]] unsigned char* get(std::uint32_t pgno) {
        auto it = pages_.find(pgno);
        return it == pages_.end() ? nullptr : it->second.data();
    }
    [[nodiscard]] const unsigned char* get(std::uint32_t pgno) const {
        auto it = pages_.find(pgno);
        return it == pages_.end() ? nullptr : it->second.data();
    }

    // Inserts (or resets, if already present) a page_size-byte slot for
    // `pgno` and returns a pointer to it, for the caller to fill in.
    unsigned char* reserve(std::uint32_t pgno, int page_size) {
        auto [it, inserted] = pages_.try_emplace(pgno);
        it->second.resize(static_cast<std::size_t>(page_size));
        return it->second.data();
    }

    // Drops a single cached page outright (not just its dirty flag), so
    // the next get()/reserve() for it starts fresh. Used by pager::rollback()
    // to force re-reading a page's on-disk content after the file itself
    // has been restored to a pre-transaction state via journal replay --
    // otherwise the cache would keep handing out the in-memory mutation
    // rollback() is supposed to undo.
    void evict(std::uint32_t pgno) {
        pages_.erase(pgno);
        dirty_.erase(pgno);
    }

    void mark_dirty(std::uint32_t pgno) { dirty_.insert(pgno); }
    [[nodiscard]] bool is_dirty(std::uint32_t pgno) const noexcept { return dirty_.contains(pgno); }
    [[nodiscard]] const std::unordered_set<std::uint32_t>& dirty_pages() const noexcept { return dirty_; }
    void clear_dirty() { dirty_.clear(); }

    // Drops every cached page beyond `page_count` -- used after a
    // rollback truncates the underlying file back to its pre-transaction
    // size, so a stale cached copy of a page that no longer exists on
    // disk can never be handed back out.
    void truncate(std::uint32_t page_count) {
        for (auto it = pages_.begin(); it != pages_.end();) {
            if (it->first > page_count) it = pages_.erase(it); else ++it;
        }
    }

private:
    std::unordered_map<std::uint32_t, std::vector<unsigned char>> pages_;
    std::unordered_set<std::uint32_t> dirty_;
};

} // namespace sqlite::backend::pager::cache
