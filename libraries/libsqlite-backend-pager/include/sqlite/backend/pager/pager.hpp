#pragma once

#include "cache/page_cache.hpp"
#include "journal_format.hpp"

#include <sqlite/backend/os/unix/unix_file.hpp>
#include <sqlite/backend/os/unix/unix_vfs.hpp>

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

// Legacy source: pager.c's Pager object -- sqlite-backend-tree's own docs
// (page_store.hpp's header comment) name this exact gap: "Real SQLite's
// Btree never touches the VFS directly; it goes through Pager for the
// page cache, rollback-journal durability, and multi-writer locking those
// provide." This library is that Pager, covering the two pieces the user
// scoped for this pass -- an in-memory page cache and a real rollback
// journal -- deliberately excluding multi-writer locking refinements
// (RESERVED/PENDING/EXCLUSIVE lock escalation across *other* connections;
// sqlite::backend::os's byte-range locks exist and could back this later)
// and WAL (a separate, newer durability mechanism).
//
// Transaction model: begin() starts a transaction; write_page()s within
// it are cached in memory (not written to the main file yet) and, the
// first time each touched page is written, its pre-transaction content is
// appended to a rollback journal file before the in-memory copy changes
// (pagerAddPageToRollbackJournal()). commit() flushes every dirty page to
// the main file, syncs it, and deletes the journal. rollback() -- or,
// automatically, opening a pager over a database whose journal is still
// present because a previous session never reached commit()/rollback()
// (pager.c's "hot journal" case) -- replays the journal's pre-images back
// onto the main file and truncates it back to its pre-transaction page
// count, undoing the transaction entirely.
namespace sqlite::backend::pager {

namespace os = sqlite::backend::os;

class pager_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class pager {
public:
    // Opens (creating if missing) `path` as a fixed-page-size paged file.
    // `page_size` only applies to a newly created, empty file; an
    // existing file's page count is derived from its actual size. If a
    // stale "<path>-journal" file is already present -- a previous
    // session began a transaction and was never able to call commit() or
    // rollback() on it, e.g. it crashed -- this constructor replays that
    // journal onto `path` and deletes it before returning, exactly like
    // legacy's hot-journal rollback on the next open.
    explicit pager(std::string path, int page_size = 4096) : path_(std::move(path)), page_size_(page_size) {
        journal_path_ = path_ + "-journal";

        bool db_exists = false;
        vfs_.access(path_, db_exists);

        os::open_flags flags = os::open_flags::readwrite | os::open_flags::main_db;
        if (!db_exists) flags = flags | os::open_flags::create;
        if (vfs_.open(path_, flags, file_) != os::result::ok) {
            throw pager_error("pager: failed to open '" + path_ + "'");
        }

        bool journal_exists = false;
        vfs_.access(journal_path_, journal_exists);
        if (journal_exists) recover_hot_journal();

        std::int64_t size = 0;
        file_.file_size(size);
        page_count_ = size > 0 ? static_cast<std::uint32_t>(size / page_size_) : 0;
    }

    pager(const pager&) = delete;
    pager& operator=(const pager&) = delete;

    [[nodiscard]] int page_size() const noexcept { return page_size_; }
    [[nodiscard]] std::uint32_t page_count() const noexcept { return page_count_; }
    [[nodiscard]] bool in_transaction() const noexcept { return in_transaction_; }

    // Returns page_size()-byte content for `pgno`, from the cache if
    // already resident, otherwise read from the main file and cached.
    [[nodiscard]] const unsigned char* read_page(std::uint32_t pgno) {
        if (pgno == 0 || pgno > page_count_) throw pager_error("pager: read_page: invalid page number");
        unsigned char* cached = cache_.get(pgno);
        if (cached) return cached;
        unsigned char* slot = cache_.reserve(pgno, page_size_);
        if (file_.read(slot, page_size_, page_offset(pgno)) != os::result::ok) {
            throw pager_error("pager: failed to read page");
        }
        return slot;
    }

    // Returns a mutable pointer to `pgno`'s cached content for the
    // caller to write into, marking it dirty. Must be called inside a
    // transaction. The first time a page that existed before this
    // transaction began is touched, its pre-transaction content is
    // journaled before this function returns.
    [[nodiscard]] unsigned char* write_page(std::uint32_t pgno) {
        if (!in_transaction_) throw pager_error("pager: write_page: not in a transaction");
        if (pgno == 0 || pgno > page_count_) throw pager_error("pager: write_page: invalid page number");

        if (pgno <= orig_page_count_ && !journaled_this_txn_.contains(pgno)) {
            const unsigned char* pre_image = read_page(pgno);
            append_journal_record(pgno, pre_image);
            journaled_this_txn_.insert(pgno);
        }

        unsigned char* slot = cache_.get(pgno);
        if (!slot) slot = read_page_into_cache(pgno);
        cache_.mark_dirty(pgno);
        return slot;
    }

    // Extends the file by one zero-filled page and returns its number.
    // Not journaled: rollback discards it simply by truncating the file
    // back to its pre-transaction page count.
    [[nodiscard]] std::uint32_t allocate_page() {
        if (!in_transaction_) throw pager_error("pager: allocate_page: not in a transaction");
        std::uint32_t pgno = ++page_count_;
        unsigned char* slot = cache_.reserve(pgno, page_size_);
        std::memset(slot, 0, static_cast<std::size_t>(page_size_));
        cache_.mark_dirty(pgno);
        return pgno;
    }

    void begin() {
        if (in_transaction_) throw pager_error("pager: begin: already in a transaction");
        orig_page_count_ = page_count_;
        journaled_this_txn_.clear();

        os::open_flags jflags = os::open_flags::readwrite | os::open_flags::create | os::open_flags::main_journal;
        if (vfs_.open(journal_path_, jflags, journal_file_) != os::result::ok) {
            throw pager_error("pager: failed to open journal '" + journal_path_ + "'");
        }

        unsigned char seed_bytes[4];
        vfs_.randomness(4, seed_bytes);
        cksum_init_ = jf_read_u32(seed_bytes);

        journal_header hdr;
        hdr.nrec = 0;
        hdr.cksum_init = cksum_init_;
        hdr.orig_page_count = orig_page_count_;
        hdr.sector_size = static_cast<std::uint32_t>(journal_header_size);
        hdr.page_size = static_cast<std::uint32_t>(page_size_);
        unsigned char header_buf[journal_header_size];
        hdr.write(header_buf);
        if (journal_file_.write(header_buf, journal_header_size, 0) != os::result::ok) {
            throw pager_error("pager: failed to write journal header");
        }
        journal_offset_ = journal_header_size;
        nrec_ = 0;
        in_transaction_ = true;
    }

    // Writes every currently-dirty page's in-memory content to the main
    // file, without ending the transaction -- the journal is untouched,
    // and commit()/rollback() must still be called afterward. Mirrors
    // legacy's mid-transaction dirty-page spilling (pager.c's
    // pagerStress(), triggered by cache-size pressure): real SQLite does
    // not keep every dirty page resident for an entire transaction, so a
    // crash before commit() genuinely can leave the main file holding
    // pages this transaction already changed -- that's the actual reason
    // a rollback journal is load-bearing, not just a theoretical one.
    // This method lets a caller (and this library's own crash-recovery
    // test) reproduce that scenario deliberately.
    void flush() {
        if (!in_transaction_) throw pager_error("pager: flush: not in a transaction");
        flush_dirty_pages();
    }

    // Flushes every dirty page to the main file, syncs it, then deletes
    // the journal -- once this returns, the transaction is durable and no
    // longer recoverable-by-rollback (matches sqlite3PagerCommitPhaseTwo()
    // + the journal deletion that follows it).
    void commit() {
        if (!in_transaction_) throw pager_error("pager: commit: not in a transaction");

        flush_dirty_pages();
        file_.sync(os::sync_flags::normal);

        finalize_journal();
        cache_.clear_dirty();
        journaled_this_txn_.clear();
        in_transaction_ = false;
    }

    // Replays the journal's pre-images back onto the main file, truncates
    // it back to the pre-transaction page count, discards every in-memory
    // dirty/newly-allocated page, and deletes the journal.
    void rollback() {
        if (!in_transaction_) throw pager_error("pager: rollback: not in a transaction");

        std::int64_t journal_size = 0;
        journal_file_.file_size(journal_size);
        std::uint32_t replayed = 0;
        replay_journal(file_, journal_file_, journal_size, page_size_, cksum_init_, replayed);

        file_.truncate(static_cast<std::int64_t>(orig_page_count_) * page_size_);
        file_.sync(os::sync_flags::normal);
        page_count_ = orig_page_count_;

        cache_.truncate(orig_page_count_);  // drops this transaction's newly-allocated pages
        for (std::uint32_t pgno : journaled_this_txn_) {
            cache_.evict(pgno);  // drops this transaction's in-place mutations of pre-existing pages
        }
        cache_.clear_dirty();

        finalize_journal();
        journaled_this_txn_.clear();
        in_transaction_ = false;
    }

private:
    [[nodiscard]] std::int64_t page_offset(std::uint32_t pgno) const noexcept {
        return static_cast<std::int64_t>(pgno - 1) * page_size_;
    }

    void flush_dirty_pages() {
        for (std::uint32_t pgno : cache_.dirty_pages()) {
            const unsigned char* content = cache_.get(pgno);
            if (file_.write(content, page_size_, page_offset(pgno)) != os::result::ok) {
                throw pager_error("pager: failed to flush dirty page");
            }
        }
    }

    unsigned char* read_page_into_cache(std::uint32_t pgno) {
        unsigned char* slot = cache_.reserve(pgno, page_size_);
        if (file_.read(slot, page_size_, page_offset(pgno)) != os::result::ok) {
            throw pager_error("pager: failed to read page");
        }
        return slot;
    }

    void append_journal_record(std::uint32_t pgno, const unsigned char* content) {
        std::vector<unsigned char> record(static_cast<std::size_t>(page_record_overhead + page_size_));
        jf_write_u32(record.data(), pgno);
        std::memcpy(record.data() + 4, content, static_cast<std::size_t>(page_size_));
        std::uint32_t cksum = page_checksum(cksum_init_, content, page_size_);
        jf_write_u32(record.data() + 4 + page_size_, cksum);
        if (journal_file_.write(record.data(), static_cast<int>(record.size()), journal_offset_) != os::result::ok) {
            throw pager_error("pager: failed to append journal record");
        }
        journal_offset_ += static_cast<std::int64_t>(record.size());
        ++nrec_;
    }

    // Rewrites the journal header with the final record count and syncs
    // it, then closes and deletes the journal file. The record count
    // isn't load-bearing for recovery (replay_journal() below reads
    // sequential records until EOF or a bad checksum, exactly like
    // legacy's "no-sync mode" journal reader, since a real crash by
    // definition means this finalize step never gets to run) -- it's
    // written for on-disk fidelity to the documented format.
    void finalize_journal() {
        unsigned char header_buf[journal_header_size];
        journal_header hdr;
        hdr.nrec = nrec_;
        hdr.cksum_init = cksum_init_;
        hdr.orig_page_count = orig_page_count_;
        hdr.sector_size = static_cast<std::uint32_t>(journal_header_size);
        hdr.page_size = static_cast<std::uint32_t>(page_size_);
        hdr.write(header_buf);
        journal_file_.write(header_buf, journal_header_size, 0);
        journal_file_.close();
        vfs_.delete_file(journal_path_, true);
    }

    // Reads sequential page records from `journal_file` (starting right
    // after its header) and writes each one's content back into
    // `main_file` at its recorded page number, stopping at the first
    // record whose checksum doesn't match (a partial record from a write
    // that was interrupted mid-append -- pager.c's own journal reader
    // stops the same way once it can no longer verify a record) or at the
    // physical end of the file.
    static void replay_journal(os::unix::unix_file& main_file, os::unix::unix_file& journal_file,
                                std::int64_t journal_size, int page_size, std::uint32_t cksum_init,
                                std::uint32_t& out_replayed) {
        std::int64_t offset = journal_header_size;
        std::vector<unsigned char> record(static_cast<std::size_t>(page_record_overhead + page_size));
        std::uint32_t count = 0;
        while (offset + static_cast<std::int64_t>(record.size()) <= journal_size) {
            if (journal_file.read(record.data(), static_cast<int>(record.size()), offset) != os::result::ok) break;
            std::uint32_t pgno = jf_read_u32(record.data());
            const unsigned char* content = record.data() + 4;
            std::uint32_t stored_cksum = jf_read_u32(record.data() + 4 + static_cast<std::size_t>(page_size));
            if (pgno == 0 || page_checksum(cksum_init, content, page_size) != stored_cksum) break;

            if (main_file.write(content, page_size, static_cast<std::int64_t>(pgno - 1) * page_size) != os::result::ok) {
                throw pager_error("pager: failed to write page during journal replay");
            }
            offset += static_cast<std::int64_t>(record.size());
            ++count;
        }
        out_replayed = count;
    }

    // Called from the constructor when a leftover journal is found before
    // this pager has ever called begin() itself -- pager.c's hot-journal
    // recovery path. Unlike rollback(), there is no live in-memory state
    // (cksum_init_/orig_page_count_/page_size_) to trust; everything
    // needed comes from the journal file's own header, which is exactly
    // what a genuinely different process recovering after a crash would
    // have to do too.
    void recover_hot_journal() {
        os::unix::unix_file journal_file;
        if (vfs_.open(journal_path_, os::open_flags::readwrite | os::open_flags::main_journal, journal_file) !=
            os::result::ok) {
            return;  // can't open it; nothing to recover
        }

        std::int64_t journal_size = 0;
        journal_file.file_size(journal_size);
        unsigned char header_buf[journal_header_size];
        if (journal_size >= journal_header_size &&
            journal_file.read(header_buf, journal_header_size, 0) == os::result::ok &&
            journal_header::has_valid_magic(header_buf)) {
            journal_header hdr = journal_header::parse(header_buf);
            std::uint32_t replayed = 0;
            replay_journal(file_, journal_file, journal_size, static_cast<int>(hdr.page_size), hdr.cksum_init,
                            replayed);
            file_.truncate(static_cast<std::int64_t>(hdr.orig_page_count) * static_cast<std::int64_t>(hdr.page_size));
            file_.sync(os::sync_flags::normal);
            page_size_ = static_cast<int>(hdr.page_size);
        }
        // Not a real journal (bad/absent magic, truncated header) --
        // nothing recoverable from it; still remove it below so it isn't
        // mistaken for a hot journal again on the next open.

        journal_file.close();
        vfs_.delete_file(journal_path_, true);
    }

    std::string path_;
    std::string journal_path_;
    int page_size_;
    std::uint32_t page_count_ = 0;

    os::unix::unix_vfs vfs_;
    os::unix::unix_file file_;
    cache::page_cache cache_;

    bool in_transaction_ = false;
    os::unix::unix_file journal_file_;
    std::int64_t journal_offset_ = 0;
    std::uint32_t cksum_init_ = 0;
    std::uint32_t orig_page_count_ = 0;
    std::uint32_t nrec_ = 0;
    std::unordered_set<std::uint32_t> journaled_this_txn_;
};

} // namespace sqlite::backend::pager
