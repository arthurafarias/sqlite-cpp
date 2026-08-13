#pragma once

#include "../btree_table_descriptor.hpp"
#include "../select_executor.hpp"

#include <sqlite/backend/tree/btree.hpp>
#include <sqlite/backend/tree/page_store.hpp>
#include <sqlite/compiler/parser/parser.hpp>
#include <sqlite/utils/testing/test_group.hpp>

#include <unistd.h>

#include <string>
#include <vector>

// Mirrors sqlite-backend-tree's own testing/btree_test.hpp persistence
// pattern (make_temp_path, close-then-reopen) -- this test's whole point is
// confirming select_executor works against *real bytes on disk*, not just
// in-process state, the same bar btree_test.hpp itself holds to.
namespace sqlite::core::command_processor::testing {

using sqlite::utils::testing::test_group;

namespace {
std::string make_temp_db_path() {
    std::string tmpl = "/tmp/sqlite_cpp_command_processor_test_XXXXXX";
    std::vector<char> buf(tmpl.begin(), tmpl.end());
    buf.push_back('\0');
    int fd = ::mkstemp(buf.data());
    if (fd >= 0) ::close(fd);
    std::string path(buf.data());
    ::unlink(path.c_str());
    return path + ".db";
}
} // namespace

inline const test_group btree_select_executor_tests("btree_select_executor", {
    {"queries a real, on-disk-backed table after closing and reopening the file", [](auto& ctx) {
        std::string path = make_temp_db_path();

        {
            tree::page_store store(path, 4096);
            tree::btree bt(store);
            insert_row(bt, 1, {vm::aux::mem::make_integer(1), vm::aux::mem::make_text("alice"), vm::aux::mem::make_real(9.5)});
            insert_row(bt, 2, {vm::aux::mem::make_integer(2), vm::aux::mem::make_text("bob"), vm::aux::mem::make_real(7.0)});
            insert_row(bt, 3, {vm::aux::mem::make_integer(3), vm::aux::mem::make_text("carol"), vm::aux::mem::make_null()});
        } // page_store/btree destroyed, file closed

        {
            tree::page_store store(path, 4096);
            tree::btree bt(store, /*root_page=*/1);
            btree_table_descriptor table{"t", {"id", "name", "score"}, bt};

            sqlite::compiler::parser::parser p("SELECT name FROM t WHERE score > 5 ORDER BY name");
            auto stmt = p.parse_select();
            select_executor executor;
            auto result = executor.execute(stmt, table);

            ctx.equal(result.rows.size(), std::size_t{2});
            ctx.check(result.rows[0][0].as_text() == "alice");
            ctx.check(result.rows[1][0].as_text() == "bob");
        }

        ::unlink(path.c_str());
    }},
    {"a FROM-less SELECT works against a btree_table_descriptor too", [](auto& ctx) {
        std::string path = make_temp_db_path();
        tree::page_store store(path, 4096);
        tree::btree bt(store);
        btree_table_descriptor table{"t", {}, bt};

        sqlite::compiler::parser::parser p("SELECT 40 + 2 AS total");
        auto stmt = p.parse_select();
        select_executor executor;
        auto result = executor.execute(stmt, table);

        ctx.equal(result.rows.size(), std::size_t{1});
        ctx.check(result.rows[0][0].as_integer() == 42);

        ::unlink(path.c_str());
    }},
});

} // namespace sqlite::core::command_processor::testing
