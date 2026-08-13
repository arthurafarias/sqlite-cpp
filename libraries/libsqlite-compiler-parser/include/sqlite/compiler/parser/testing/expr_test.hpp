#pragma once

#include "../parser.hpp"
#include "sexpr.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::compiler::parser::testing {

using sqlite::utils::testing::test_group;

namespace {
std::string parse(std::string_view sql) {
    parser p(sql);
    return to_sexpr(p.parse_expr());
}
} // namespace

inline const test_group expr_tests("expr", {
    {"literals", [](auto& ctx) {
        ctx.equal(parse("42"), std::string{"42"});
        ctx.equal(parse("3.14"), std::string{"3.14"});
        ctx.equal(parse("NULL"), std::string{"NULL"});
        ctx.equal(parse("?1"), std::string{"?1"});
        ctx.equal(parse(":name"), std::string{":name"});
    }},
    {"a string literal has its quotes stripped and doubled quotes un-escaped", [](auto& ctx) {
        ctx.equal(parse("'hello'"), std::string{"hello"});
        ctx.equal(parse("'it''s'"), std::string{"it's"});
    }},
    {"a blob literal keeps just the hex digits", [](auto& ctx) {
        ctx.equal(parse("x'DEADBEEF'"), std::string{"DEADBEEF"});
    }},
    {"column references, bare and table-qualified", [](auto& ctx) {
        ctx.equal(parse("col"), std::string{"col"});
        ctx.equal(parse("t.col"), std::string{"t.col"});
    }},
    {"* binds tighter than +", [](auto& ctx) {
        ctx.equal(parse("1+2*3"), std::string{"(+ 1 (* 2 3))"});
        ctx.equal(parse("2*3+1"), std::string{"(+ (* 2 3) 1)"});
    }},
    {"+ and - are left-associative", [](auto& ctx) {
        ctx.equal(parse("1-2-3"), std::string{"(- (- 1 2) 3)"});
    }},
    {"comparison binds looser than arithmetic", [](auto& ctx) {
        ctx.equal(parse("1+1 = 2"), std::string{"(= (+ 1 1) 2)"});
    }},
    {"AND binds tighter than OR", [](auto& ctx) {
        ctx.equal(parse("a OR b AND c"), std::string{"(or a (and b c))"});
    }},
    {"comparison binds tighter than AND", [](auto& ctx) {
        ctx.equal(parse("a = 1 AND b = 2"), std::string{"(and (= a 1) (= b 2))"});
    }},
    {"|| binds tighter than comparison and looser than unary", [](auto& ctx) {
        ctx.equal(parse("a || b = c"), std::string{"(= (|| a b) c)"});
    }},
    {"bitwise operators bind between relational and additive", [](auto& ctx) {
        ctx.equal(parse("1 & 2 + 3"), std::string{"(& 1 (+ 2 3))"});
        ctx.equal(parse("1 < 2 & 3"), std::string{"(< 1 (& 2 3))"});
    }},
    {"parentheses override precedence", [](auto& ctx) {
        ctx.equal(parse("(1+2)*3"), std::string{"(* (+ 1 2) 3)"});
    }},
    {"unary minus and double unary minus", [](auto& ctx) {
        ctx.equal(parse("-5"), std::string{"(u- 5)"});
        ctx.equal(parse("- -5"), std::string{"(u- (u- 5))"});
        ctx.equal(parse("-a*b"), std::string{"(* (u- a) b)"});
    }},
    {"NOT is a right-associative prefix operator looser than comparison", [](auto& ctx) {
        ctx.equal(parse("NOT a = 1"), std::string{"(not (= a 1))"});
        ctx.equal(parse("NOT NOT a"), std::string{"(not (not a))"});
    }},
    {"NOT binds looser than AND but the whole NOT-expr is one AND operand", [](auto& ctx) {
        ctx.equal(parse("NOT a AND b"), std::string{"(and (not a) b)"});
    }},
    {"IS NULL, ISNULL, NOTNULL, IS NOT NULL", [](auto& ctx) {
        ctx.equal(parse("a IS NULL"), std::string{"(isnull a)"});
        ctx.equal(parse("a ISNULL"), std::string{"(isnull a)"});
        ctx.equal(parse("a NOTNULL"), std::string{"(notnull a)"});
        ctx.equal(parse("a IS NOT NULL"), std::string{"(notnull a)"});
    }},
    {"IS and IS NOT as a general comparison", [](auto& ctx) {
        ctx.equal(parse("a IS b"), std::string{"(is a b)"});
        ctx.equal(parse("a IS NOT b"), std::string{"(is_not a b)"});
    }},
    {"IN with an expression list, and NOT IN", [](auto& ctx) {
        ctx.equal(parse("a IN (1, 2, 3)"), std::string{"(in a 1 2 3)"});
        ctx.equal(parse("a NOT IN (1, 2)"), std::string{"(not-in a 1 2)"});
        ctx.equal(parse("a IN ()"), std::string{"(in a)"});
    }},
    {"BETWEEN, and NOT BETWEEN, with its low/high not swallowing a trailing AND", [](auto& ctx) {
        ctx.equal(parse("a BETWEEN 1 AND 10"), std::string{"(between a 1 10)"});
        ctx.equal(parse("a NOT BETWEEN 1 AND 10"), std::string{"(not-between a 1 10)"});
        ctx.equal(parse("a BETWEEN 1 AND 10 AND b"), std::string{"(and (between a 1 10) b)"});
    }},
    {"LIKE, NOT LIKE, GLOB, and LIKE ... ESCAPE", [](auto& ctx) {
        ctx.equal(parse("a LIKE 'x%'"), std::string{"(like a x%)"});
        ctx.equal(parse("a NOT LIKE 'x%'"), std::string{"(not-like a x%)"});
        ctx.equal(parse("a GLOB 'x*'"), std::string{"(like a x*)"});
        ctx.equal(parse("a LIKE 'x^%' ESCAPE '^'"), std::string{"(like a x^% escape ^)"});
    }},
    {"CASE with a base expression", [](auto& ctx) {
        ctx.equal(parse("CASE a WHEN 1 THEN 'one' WHEN 2 THEN 'two' ELSE 'other' END"),
                   std::string{"(case a (when 1 then one) (when 2 then two) (else other))"});
    }},
    {"searched CASE (no base expression)", [](auto& ctx) {
        ctx.equal(parse("CASE WHEN a > 0 THEN 'pos' ELSE 'non-pos' END"),
                   std::string{"(case (when (> a 0) then pos) (else non-pos))"});
    }},
    {"CASE without ELSE", [](auto& ctx) {
        ctx.equal(parse("CASE WHEN a THEN 1 END"), std::string{"(case (when a then 1))"});
    }},
    {"function calls: no args, args, DISTINCT, and *", [](auto& ctx) {
        ctx.equal(parse("now()"), std::string{"(call now)"});
        ctx.equal(parse("max(a, b)"), std::string{"(call max a b)"});
        ctx.equal(parse("count(DISTINCT a)"), std::string{"(call count distinct a)"});
        ctx.equal(parse("count(*)"), std::string{"(call count *)"});
    }},
    {"a malformed expression throws parse_error", [](auto& ctx) {
        ctx.template throws<parse_error>([] { parser p("1 +"); p.parse_expr(); });
        ctx.template throws<parse_error>([] { parser p("(1 + 2"); p.parse_expr(); });
        ctx.template throws<parse_error>([] { parser p("SELECT"); p.parse_expr(); });
    }},
});

} // namespace sqlite::compiler::parser::testing
