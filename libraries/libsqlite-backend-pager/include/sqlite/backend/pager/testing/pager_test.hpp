#pragma once

#include "../pager.hpp"

#include <sqlite/backend/os/unix/unix_vfs.hpp>
#include <sqlite/utils/testing/test_group.hpp>

#include <cstring>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>

namespace sqlite::backend::pager::testing {

using sqlite::utils::testing::test_group;

namespace {
std::string make_temp_path(const char* suffix) {
    std::string tmpl = "/tmp/sqlite_cpp_backend_pager_test_XXXXXX";
    std::vector<char> buf(tmpl.begin(), tmpl.end());
    buf.push_back('\0');
    int fd = ::mkstemp(buf.data());
    if (fd >= 0) ::close(fd);
    std::string path(buf.data());
    ::unlink(path.c_str());
    return path + suffix;
}

void fill(unsigned char* dst, int size, unsigned char byte) { std::memset(dst, byte, static_cast<std::size_t>(size)); }

bool journal_exists(const std::string& db_path) {
    sqlite::backend::os::unix::unix_vfs vfs;
    bool exists = false;
    vfs.access(db_path + "-journal", exists);
    return exists;
}

void remove_files(const std::string& db_path) {
    ::unlink(db_path.c_str());
    ::unlink((db_path + "-journal").c_str());
}
} // namespace

inline const test_group pager_tests("pager", {
    {"a committed transaction's content survives destroying and reopening the pager", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        {
            pager p(path, 512);
            p.begin();
            std::uint32_t pgno = p.allocate_page();
            ctx.equal(pgno, std::uint32_t{1});
            fill(p.write_page(pgno), 512, 0xAA);
            p.commit();
            ctx.check(!p.in_transaction());
        }
        {
            pager p2(path, 512);
            ctx.equal(p2.page_count(), std::uint32_t{1});
            const unsigned char* content = p2.read_page(1);
            ctx.check(content[0] == 0xAA && content[511] == 0xAA);
        }
        ctx.check(!journal_exists(path));
        remove_files(path);
    }},
    {"commit deletes the journal file", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        pager p(path, 512);
        p.begin();
        fill(p.write_page(p.allocate_page()), 512, 1);
        ctx.check(journal_exists(path));
        p.commit();
        ctx.check(!journal_exists(path));
        remove_files(path);
    }},
    {"explicit rollback restores original content and the pre-transaction page count", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        pager p(path, 512);

        p.begin();
        std::uint32_t pgno = p.allocate_page();
        fill(p.write_page(pgno), 512, 0x11);
        p.commit();
        ctx.equal(p.page_count(), std::uint32_t{1});

        p.begin();
        fill(p.write_page(pgno), 512, 0x22);       // modify the existing page
        std::uint32_t new_pgno = p.allocate_page(); // and allocate a second one
        fill(p.write_page(new_pgno), 512, 0x33);
        ctx.equal(p.page_count(), std::uint32_t{2});

        p.rollback();

        ctx.check(!p.in_transaction());
        ctx.equal(p.page_count(), std::uint32_t{1});  // the allocation is undone
        const unsigned char* content = p.read_page(pgno);
        ctx.check(content[0] == 0x11);  // the modification is undone
        ctx.check(!journal_exists(path));
        remove_files(path);
    }},
    {"write_page only journals a page's pre-image once per transaction", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        pager p(path, 512);
        p.begin();
        std::uint32_t pgno = p.allocate_page();
        fill(p.write_page(pgno), 512, 0x11);
        p.commit();

        p.begin();
        fill(p.write_page(pgno), 512, 0x22);  // pre-image 0x11 journaled here
        fill(p.write_page(pgno), 512, 0x33);  // must NOT re-journal 0x22 as if it were the original
        p.rollback();

        const unsigned char* content = p.read_page(pgno);
        ctx.check(content[0] == 0x11);  // the true original, not the intermediate 0x22
        remove_files(path);
    }},
    {"a newly allocated page within a transaction is not journaled (rollback just truncates it away)", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        pager p(path, 512);
        p.begin();
        std::uint32_t pgno = p.allocate_page();
        fill(p.write_page(pgno), 512, 0x11);
        ctx.equal(p.page_count(), std::uint32_t{1});
        p.rollback();
        ctx.equal(p.page_count(), std::uint32_t{0});
        remove_files(path);
    }},
    {"write_page/allocate_page/commit/rollback outside a transaction throw", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        pager p(path, 512);
        ctx.template throws<pager_error>([&]() { (void)p.allocate_page(); });
        ctx.template throws<pager_error>([&]() { (void)p.write_page(1); });
        ctx.template throws<pager_error>([&]() { p.commit(); });
        ctx.template throws<pager_error>([&]() { p.rollback(); });
        remove_files(path);
    }},
    {"crash recovery: a pager destroyed mid-transaction after flush() leaves a hot journal "
     "that the next pager opened over the same file automatically replays",
     [](auto& ctx) {
         std::string path = make_temp_path(".db");
         std::uint32_t pgno;
         {
             pager setup(path, 512);
             setup.begin();
             pgno = setup.allocate_page();
             fill(setup.write_page(pgno), 512, 0xAA);
             setup.commit();
         }
         ctx.check(!journal_exists(path));

         {
             // Simulate a crash partway through a transaction: modify the
             // page, flush() it to the main file (mirrors legacy's
             // cache-pressure-driven mid-transaction spill -- see
             // pager.hpp's flush() comment), then destroy the pager
             // *without* calling commit() or rollback(). The journal
             // (written synchronously by write_page(), before flush() or
             // destruction) is left behind on disk, and the main file now
             // genuinely holds the uncommitted 0xBB content.
             pager crashing(path, 512);
             crashing.begin();
             fill(crashing.write_page(pgno), 512, 0xBB);
             crashing.flush();
             // no commit(), no rollback() -- `crashing` is now destroyed.
         }
         ctx.check(journal_exists(path));  // the hot journal is really there

         // Reopening must detect the hot journal and recover automatically,
         // restoring the pre-crash committed content, before this pager is
         // usable at all.
         pager recovered(path, 512);
         ctx.check(!journal_exists(path));  // recovery consumes the journal
         ctx.equal(recovered.page_count(), std::uint32_t{1});
         const unsigned char* content = recovered.read_page(pgno);
         ctx.check(content[0] == 0xAA);  // not 0xBB -- the crashed write never got to commit

         remove_files(path);
     }},
    {"a pager reopened cleanly (no leftover journal) just sees the last committed state", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        {
            pager p(path, 512);
            p.begin();
            std::uint32_t pgno = p.allocate_page();
            fill(p.write_page(pgno), 512, 0x55);
            p.commit();
        }
        pager p2(path, 512);
        ctx.equal(p2.page_count(), std::uint32_t{1});
        ctx.check(p2.read_page(1)[0] == 0x55);
        remove_files(path);
    }},
});

} // namespace sqlite::backend::pager::testing
