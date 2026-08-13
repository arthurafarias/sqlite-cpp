#pragma once

#include "../select_executor.hpp"

#include <sqlite/compiler/parser/parser.hpp>

#include <string_view>

// Test-only support (not a legacy port): parses real SQL text end-to-end
// and executes it via select_executor, so tests assert against actual
// query-execution output -- not just against hand-built ASTs or the shape
// of emitted instructions. Mirrors
// sqlite-compiler-code-generator/testing/eval_helper.hpp's role one layer
// up the stack.
namespace sqlite::core::command_processor::testing {

inline query_result run_select(std::string_view sql, const table_descriptor& table) {
    sqlite::compiler::parser::parser p(sql);
    ast::select_stmt stmt = p.parse_select();
    select_executor executor;
    return executor.execute(stmt, table);
}

// A small demo table with several column types, reused across tests:
// id (integer, 1..5), name (text), score (real, some NULL).
inline table_descriptor demo_table() {
    table_descriptor t;
    t.name = "t";
    t.columns = {"id", "name", "score"};
    t.rows = {
        {vm::aux::mem::make_integer(1), vm::aux::mem::make_text("alice"), vm::aux::mem::make_real(9.5)},
        {vm::aux::mem::make_integer(2), vm::aux::mem::make_text("bob"), vm::aux::mem::make_real(7.0)},
        {vm::aux::mem::make_integer(3), vm::aux::mem::make_text("carol"), vm::aux::mem::make_null()},
        {vm::aux::mem::make_integer(4), vm::aux::mem::make_text("dave"), vm::aux::mem::make_real(7.0)},
        {vm::aux::mem::make_integer(5), vm::aux::mem::make_text("eve"), vm::aux::mem::make_real(3.25)},
    };
    return t;
}

} // namespace sqlite::core::command_processor::testing
