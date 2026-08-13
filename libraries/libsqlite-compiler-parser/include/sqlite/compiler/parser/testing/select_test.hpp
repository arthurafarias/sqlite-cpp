#pragma once

#include "../parser.hpp"
#include "sexpr.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::compiler::parser::testing {

using sqlite::utils::testing::test_group;

inline const test_group select_tests("select", {
    {"a minimal SELECT with a literal and no FROM", [](auto& ctx) {
        parser p("SELECT 1");
        ast::select_stmt s = p.parse_select();
        ctx.equal(s.columns.size(), std::size_t{1});
        ctx.check(!s.columns[0].star);
        ctx.equal(to_sexpr(s.columns[0].expression), std::string{"1"});
        ctx.check(!s.from.has_value());
    }},
    {"SELECT * and table.*", [](auto& ctx) {
        parser p("SELECT * FROM t");
        ast::select_stmt s = p.parse_select();
        ctx.check(s.columns[0].star);
        ctx.check(s.columns[0].star_table.empty());

        parser p2("SELECT t.* FROM t");
        ast::select_stmt s2 = p2.parse_select();
        ctx.check(s2.columns[0].star);
        ctx.equal(s2.columns[0].star_table, std::string{"t"});
    }},
    {"DISTINCT and multiple result columns with aliases", [](auto& ctx) {
        parser p("SELECT DISTINCT a AS x, b y, c FROM t");
        ast::select_stmt s = p.parse_select();
        ctx.check(s.distinct);
        ctx.equal(s.columns.size(), std::size_t{3});
        ctx.equal(s.columns[0].alias, std::string{"x"});
        ctx.equal(s.columns[1].alias, std::string{"y"}); // bare alias, no AS
        ctx.check(s.columns[2].alias.empty());
    }},
    {"FROM with a table alias", [](auto& ctx) {
        parser p("SELECT * FROM my_table t");
        ast::select_stmt s = p.parse_select();
        ctx.equal(s.from->name, std::string{"my_table"});
        ctx.equal(s.from->alias, std::string{"t"});
    }},
    {"comma-joined tables and explicit INNER/LEFT JOIN ... ON", [](auto& ctx) {
        parser p("SELECT * FROM a, b JOIN c ON c.id = a.id LEFT JOIN d ON d.id = b.id");
        ast::select_stmt s = p.parse_select();
        ctx.equal(s.joins.size(), std::size_t{3});
        ctx.check(s.joins[0].type == ast::join_type::cross); // comma join
        ctx.check(s.joins[1].type == ast::join_type::inner);
        ctx.equal(to_sexpr(s.joins[1].on_condition), std::string{"(= c.id a.id)"});
        ctx.check(s.joins[2].type == ast::join_type::left_outer);
    }},
    {"JOIN ... USING", [](auto& ctx) {
        parser p("SELECT * FROM a JOIN b USING (id, kind)");
        ast::select_stmt s = p.parse_select();
        ctx.equal(s.joins[0].using_columns.size(), std::size_t{2});
        ctx.equal(s.joins[0].using_columns[0], std::string{"id"});
        ctx.equal(s.joins[0].using_columns[1], std::string{"kind"});
    }},
    {"WHERE", [](auto& ctx) {
        parser p("SELECT * FROM t WHERE a = 1 AND b > 2");
        ast::select_stmt s = p.parse_select();
        ctx.equal(to_sexpr(s.where), std::string{"(and (= a 1) (> b 2))"});
    }},
    {"GROUP BY and HAVING", [](auto& ctx) {
        parser p("SELECT a, count(*) FROM t GROUP BY a HAVING count(*) > 1");
        ast::select_stmt s = p.parse_select();
        ctx.equal(s.group_by.size(), std::size_t{1});
        ctx.equal(to_sexpr(s.group_by[0]), std::string{"a"});
        ctx.equal(to_sexpr(s.having), std::string{"(> (call count *) 1)"});
    }},
    {"ORDER BY with ASC/DESC and multiple terms", [](auto& ctx) {
        parser p("SELECT * FROM t ORDER BY a DESC, b, c ASC");
        ast::select_stmt s = p.parse_select();
        ctx.equal(s.order_by.size(), std::size_t{3});
        ctx.check(s.order_by[0].descending);
        ctx.check(!s.order_by[1].descending);
        ctx.check(!s.order_by[2].descending);
    }},
    {"LIMIT, LIMIT OFFSET, and the legacy LIMIT offset, count comma form", [](auto& ctx) {
        parser p1("SELECT * FROM t LIMIT 10");
        ast::select_stmt s1 = p1.parse_select();
        ctx.equal(to_sexpr(s1.limit), std::string{"10"});
        ctx.check(!s1.offset);

        parser p2("SELECT * FROM t LIMIT 10 OFFSET 20");
        ast::select_stmt s2 = p2.parse_select();
        ctx.equal(to_sexpr(s2.limit), std::string{"10"});
        ctx.equal(to_sexpr(s2.offset), std::string{"20"});

        parser p3("SELECT * FROM t LIMIT 20, 10"); // "LIMIT offset, count"
        ast::select_stmt s3 = p3.parse_select();
        ctx.equal(to_sexpr(s3.limit), std::string{"10"});
        ctx.equal(to_sexpr(s3.offset), std::string{"20"});
    }},
    {"a full statement exercising every clause together", [](auto& ctx) {
        parser p(
            "SELECT DISTINCT a, b AS bb "
            "FROM t1 JOIN t2 ON t1.id = t2.id "
            "WHERE a > 0 "
            "GROUP BY a "
            "HAVING count(*) > 1 "
            "ORDER BY a DESC "
            "LIMIT 5 OFFSET 10");
        ast::select_stmt s = p.parse_select();
        ctx.check(s.distinct);
        ctx.equal(s.columns.size(), std::size_t{2});
        ctx.equal(s.from->name, std::string{"t1"});
        ctx.equal(s.joins.size(), std::size_t{1});
        ctx.equal(to_sexpr(s.where), std::string{"(> a 0)"});
        ctx.equal(s.group_by.size(), std::size_t{1});
        ctx.equal(to_sexpr(s.having), std::string{"(> (call count *) 1)"});
        ctx.equal(s.order_by.size(), std::size_t{1});
        ctx.equal(to_sexpr(s.limit), std::string{"5"});
        ctx.equal(to_sexpr(s.offset), std::string{"10"});
    }},
    {"a SELECT missing its column list throws parse_error", [](auto& ctx) {
        ctx.template throws<parse_error>([] { parser p("SELECT FROM t"); p.parse_select(); });
    }},
});

} // namespace sqlite::compiler::parser::testing
