#pragma once

#include "page_layout.hpp"

#include <sqlite/backend/os/unix/unix_file.hpp>
#include <sqlite/backend/os/unix/unix_vfs.hpp>
#include <sqlite/backend/pager/pager.hpp>

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

// Legacy source: none directly -- this is the adapter between
// sqlite::backend::tree::btree (which calls page_size()/read_page()/
// write_page()/allocate_page()/format_header(), with no notion of
// transactions of its own) and sqlite::backend::pager::pager, which now
// provides the real page cache and rollback journal SRS S3.3.2 calls for
// (pager.c/pcache.c). This used to be a deliberately minimal stand-in
// (real file I/O, no cache, no journal, a crash mid-write left the file
// however the completed writes left it) -- see this library's docs for
// that history. It is not a stand-in anymore: every read/write now goes
// through a real page cache, and every mutation is journaled.
//
// Because btree.hpp has no concept of "begin a transaction" / "commit"
// itself (it is a raw B-tree, not a SQL engine with statement/transaction
// boundaries), this class keeps exactly one sqlite::backend::pager
// transaction open for its entire lifetime, started right after
// construction (or after formatting a brand-new database) and committed
// in the destructor. This preserves this class's previous observable
// contract exactly -- every write made through a page_store is visible
// the moment the page_store is destroyed (e.g. "insert some rows, close
// the file, reopen it, the rows are still there" -- this library's own
// persistence test) -- while gaining real crash safety it never had
// before: if the *process* is killed before a page_store is destroyed,
// the next page_store opened over the same path finds the still-open
// transaction's journal and rolls it back automatically (pager::pager's
// hot-journal recovery), leaving the database exactly as it was at the
// last time a page_store over it was cleanly destroyed, rather than in
// whatever partial state the interrupted writes left it in.
namespace sqlite::backend::tree {

namespace os = sqlite::backend::os;
namespace pgr = sqlite::backend::pager;

class page_store_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class page_store {
public:
    // Opens `path`, creating it (and formatting page 1) if it doesn't
    // already exist. `page_size` is only used for a newly created file;
    // an existing file's page size is read back from its database header
    // (sqlite::backend::pager::pager itself has no notion of this
    // library's database_header format, so that page size has to be
    // discovered here, before pager_ is constructed, and passed in).
    explicit page_store(std::string path, int page_size = 4096)
        : pager_(path, detect_page_size(path, page_size)) {
        if (pager_.page_count() == 0) format_new_database();
        pager_.begin();
    }

    page_store(const page_store&) = delete;
    page_store& operator=(const page_store&) = delete;

    ~page_store() {
        if (pager_.in_transaction()) pager_.commit();
    }

    [[nodiscard]] int page_size() const noexcept { return pager_.page_size(); }
    [[nodiscard]] int usable_size() const noexcept { return pager_.page_size(); }  // reserved space always 0
    [[nodiscard]] std::uint32_t page_count() const noexcept { return pager_.page_count(); }

    // Writes the first database_header_size bytes of a page-1 buffer with
    // the current database header (magic string, page size, page count).
    // Every caller that rewrites page 1's *content* (btree.hpp's
    // write_leaf_page()/write_interior_page(), which build page 1's buffer
    // from scratch each time) must call this before writing that buffer --
    // otherwise the freshly zero-initialized buffer they start from would
    // silently overwrite the database header with zero bytes, corrupting
    // the file's page size/page count the moment page 1's B-tree content
    // first changes.
    void format_header(unsigned char* page1_buf) const {
        database_header header;
        header.page_size = pager_.page_size() == 65536 ? 1 : static_cast<std::uint16_t>(pager_.page_size());
        header.page_count = pager_.page_count();
        header.write(page1_buf);
    }

    void read_page(std::uint32_t pgno, unsigned char* buf) {
        const unsigned char* content = pager_.read_page(pgno);
        std::memcpy(buf, content, static_cast<std::size_t>(pager_.page_size()));
    }

    void write_page(std::uint32_t pgno, const unsigned char* buf) {
        unsigned char* slot = pager_.write_page(pgno);
        std::memcpy(slot, buf, static_cast<std::size_t>(pager_.page_size()));
    }

    // Reserves the next page number; the caller is responsible for
    // writing content to it via write_page() before relying on it.
    [[nodiscard]] std::uint32_t allocate_page() { return pager_.allocate_page(); }

private:
    static int detect_page_size(const std::string& path, int fallback) {
        os::unix::unix_vfs vfs;
        bool exists = false;
        vfs.access(path, exists);
        if (!exists) return fallback;

        os::unix::unix_file file;
        if (vfs.open(path, os::open_flags::readwrite | os::open_flags::main_db, file) != os::result::ok) {
            return fallback;
        }
        unsigned char header_bytes[database_header_size];
        if (file.read(header_bytes, database_header_size, 0) != os::result::ok) return fallback;
        database_header header = database_header::parse(header_bytes);
        return header.page_size == 1 ? 65536 : header.page_size;
    }

    // A brand-new (zero-page) database: format page 1's database header
    // plus an empty leaf-page root, exactly like the pre-pager page_store
    // used to do -- except now via a real (short, self-contained)
    // transaction, so this formatting step itself is crash-safe rather
    // than a bare unjournaled write.
    void format_new_database() {
        pager_.begin();
        std::uint32_t pgno = pager_.allocate_page();
        unsigned char* buf = pager_.write_page(pgno);
        std::memset(buf, 0, static_cast<std::size_t>(pager_.page_size()));
        format_header(buf);
        page_header root{page_type::leaf_table, 0, 0, 0, 0, 0};
        root.write(buf, database_header_size);
        pager_.commit();
    }

    pgr::pager pager_;
};

} // namespace sqlite::backend::tree
