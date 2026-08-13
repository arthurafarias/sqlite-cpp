#pragma once

#include "select_helper.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::core::command_processor::testing {

using sqlite::utils::testing::test_group;

inline const test_group select_executor_tests("select_executor", {
    {"selects specific columns in the requested order", [](auto& ctx) {
        auto result = run_select("SELECT name, id FROM t", demo_table());
        ctx.equal(result.column_names.size(), std::size_t{2});
        ctx.check(result.column_names[0] == "name" && result.column_names[1] == "id");
        ctx.equal(result.rows.size(), std::size_t{5});
        ctx.check(result.rows[0][0].as_text() == "alice");
        ctx.check(result.rows[0][1].as_integer() == 1);
    }},
    {"expands * to every table column in order", [](auto& ctx) {
        auto result = run_select("SELECT * FROM t", demo_table());
        ctx.equal(result.column_names.size(), std::size_t{3});
        ctx.check(result.column_names[0] == "id" && result.column_names[1] == "name" && result.column_names[2] == "score");
        ctx.equal(result.rows.size(), std::size_t{5});
    }},
    {"filters rows with WHERE, evaluated per row against real column values", [](auto& ctx) {
        auto result = run_select("SELECT id FROM t WHERE score > 5", demo_table());
        ctx.equal(result.rows.size(), std::size_t{3}); // alice(9.5), bob(7.0), dave(7.0); carol NULL, eve 3.25 excluded
    }},
    {"computes expressions and aliases in result columns", [](auto& ctx) {
        auto result = run_select("SELECT id + 100 AS shifted FROM t WHERE id = 2", demo_table());
        ctx.equal(result.rows.size(), std::size_t{1});
        ctx.check(result.column_names[0] == "shifted");
        ctx.check(result.rows[0][0].as_integer() == 102);
    }},
    {"a FROM-less SELECT evaluates once, with no table columns available", [](auto& ctx) {
        auto result = run_select("SELECT 1 + 2 AS total", demo_table());
        ctx.equal(result.rows.size(), std::size_t{1});
        ctx.check(result.rows[0][0].as_integer() == 3);
    }},
    {"DISTINCT removes duplicate result rows", [](auto& ctx) {
        auto result = run_select("SELECT DISTINCT score FROM t", demo_table());
        // scores: 9.5, 7.0, NULL, 7.0, 3.25 -> 4 distinct values.
        ctx.equal(result.rows.size(), std::size_t{4});
    }},
    {"ORDER BY sorts ascending by default, using real comparison semantics", [](auto& ctx) {
        auto result = run_select("SELECT id FROM t ORDER BY score", demo_table());
        // NULL first (carol=3), then 3.25(eve=5), 7.0(bob=2), 7.0(dave=4), 9.5(alice=1) -- stable among ties.
        ctx.equal(result.rows.size(), std::size_t{5});
        ctx.check(result.rows[0][0].as_integer() == 3);
        ctx.check(result.rows[4][0].as_integer() == 1);
    }},
    {"ORDER BY DESC reverses the comparison", [](auto& ctx) {
        auto result = run_select("SELECT id FROM t ORDER BY id DESC", demo_table());
        ctx.check(result.rows.front()[0].as_integer() == 5);
        ctx.check(result.rows.back()[0].as_integer() == 1);
    }},
    {"LIMIT/OFFSET slice the (sorted) result", [](auto& ctx) {
        auto result = run_select("SELECT id FROM t ORDER BY id LIMIT 2 OFFSET 1", demo_table());
        ctx.equal(result.rows.size(), std::size_t{2});
        ctx.check(result.rows[0][0].as_integer() == 2);
        ctx.check(result.rows[1][0].as_integer() == 3);
    }},
    {"an unknown table throws query_error", [](auto& ctx) {
        ctx.template throws<query_error>([] { run_select("SELECT * FROM nope", demo_table()); });
    }},
    {"an unknown column throws query_error", [](auto& ctx) {
        ctx.template throws<query_error>([] { run_select("SELECT nope FROM t", demo_table()); });
    }},
    {"JOIN is rejected as out of this pass's scope", [](auto& ctx) {
        ctx.template throws<query_error>([] { run_select("SELECT * FROM t JOIN t AS t2 ON t.id = t2.id", demo_table()); });
    }},
    {"GROUP BY is rejected as out of this pass's scope", [](auto& ctx) {
        ctx.template throws<query_error>([] { run_select("SELECT id FROM t GROUP BY id", demo_table()); });
    }},
    {"a function call is rejected (code_generator's own deferred scope, rewrapped)", [](auto& ctx) {
        ctx.template throws<query_error>([] { run_select("SELECT count(id) FROM t", demo_table()); });
    }},
});

} // namespace sqlite::core::command_processor::testing
