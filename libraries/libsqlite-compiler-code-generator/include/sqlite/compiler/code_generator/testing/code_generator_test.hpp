#pragma once

#include "eval_helper.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::compiler::code_generator::testing {

using sqlite::utils::testing::test_group;
namespace vm = sqlite::core::virtual_machine;

inline const test_group code_generator_tests("code_generator", {
    {"integer, float, string, and null literals compile and run to the expected value", [](auto& ctx) {
        ctx.equal(eval("42").as_integer(), std::int64_t{42});
        ctx.equal(eval("3.5").as_real(), 3.5);
        ctx.equal(eval("'hi'").as_text(), std::string{"hi"});
        ctx.check(eval("NULL").is_null());
    }},
    {"a hex integer literal and a digit-separator literal parse to the right value", [](auto& ctx) {
        ctx.equal(eval("0x1F").as_integer(), std::int64_t{31});
        ctx.equal(eval("1_000_000").as_integer(), std::int64_t{1000000});
    }},
    {"a column reference resolves through the caller-supplied resolver and reads the bound value", [](auto& ctx) {
        std::map<std::string, vm::aux::mem> cols{{"a", vm::aux::mem::make_integer(7)}};
        ctx.equal(eval("a", cols).as_integer(), std::int64_t{7});
    }},
    {"arithmetic on two columns, actually executed end to end", [](auto& ctx) {
        std::map<std::string, vm::aux::mem> cols{
            {"a", vm::aux::mem::make_integer(4)}, {"b", vm::aux::mem::make_integer(5)}};
        ctx.equal(eval("a + b", cols).as_integer(), std::int64_t{9});
        ctx.equal(eval("a * b", cols).as_integer(), std::int64_t{20});
        ctx.equal(eval("b - a", cols).as_integer(), std::int64_t{1});
    }},
    {"a nested arithmetic expression compiles with the parser's real precedence", [](auto& ctx) {
        ctx.equal(eval("1 + 2 * 3").as_integer(), std::int64_t{7});
        ctx.equal(eval("(1 + 2) * 3").as_integer(), std::int64_t{9});
    }},
    {"string concatenation", [](auto& ctx) {
        ctx.equal(eval("'foo' || 'bar'").as_text(), std::string{"foobar"});
    }},
    {"unary minus, including double negation", [](auto& ctx) {
        ctx.equal(eval("-5").as_integer(), std::int64_t{-5});
        ctx.equal(eval("- -5").as_integer(), std::int64_t{5});
        std::map<std::string, vm::aux::mem> cols{{"a", vm::aux::mem::make_integer(3)}};
        ctx.equal(eval("-a * 2", cols).as_integer(), std::int64_t{-6});
    }},
    {"unary plus is a no-op", [](auto& ctx) {
        ctx.equal(eval("+5").as_integer(), std::int64_t{5});
    }},
    {"comparisons evaluate to 1 or 0, not just jump", [](auto& ctx) {
        ctx.equal(eval("1 = 1").as_integer(), std::int64_t{1});
        ctx.equal(eval("1 = 2").as_integer(), std::int64_t{0});
        ctx.equal(eval("1 < 2").as_integer(), std::int64_t{1});
        ctx.equal(eval("2 < 1").as_integer(), std::int64_t{0});
        ctx.equal(eval("2 >= 2").as_integer(), std::int64_t{1});
        ctx.equal(eval("1 != 1").as_integer(), std::int64_t{0});
    }},
    {"a comparison result feeding into arithmetic", [](auto& ctx) {
        // (1 < 2) + (3 < 2) == 1 + 0 == 1
        ctx.equal(eval("(1 < 2) + (3 < 2)").as_integer(), std::int64_t{1});
    }},
    {"AND truth table", [](auto& ctx) {
        ctx.equal(eval("1 AND 1").as_integer(), std::int64_t{1});
        ctx.equal(eval("1 AND 0").as_integer(), std::int64_t{0});
        ctx.equal(eval("0 AND 1").as_integer(), std::int64_t{0});
        ctx.equal(eval("0 AND 0").as_integer(), std::int64_t{0});
    }},
    {"OR truth table", [](auto& ctx) {
        ctx.equal(eval("1 OR 1").as_integer(), std::int64_t{1});
        ctx.equal(eval("1 OR 0").as_integer(), std::int64_t{1});
        ctx.equal(eval("0 OR 1").as_integer(), std::int64_t{1});
        ctx.equal(eval("0 OR 0").as_integer(), std::int64_t{0});
    }},
    {"NOT", [](auto& ctx) {
        ctx.equal(eval("NOT 1").as_integer(), std::int64_t{0});
        ctx.equal(eval("NOT 0").as_integer(), std::int64_t{1});
        ctx.equal(eval("NOT NOT 1").as_integer(), std::int64_t{1});
    }},
    {"a realistic mixed boolean/comparison/arithmetic expression over bound columns", [](auto& ctx) {
        std::map<std::string, vm::aux::mem> cols{
            {"price", vm::aux::mem::make_integer(100)}, {"qty", vm::aux::mem::make_integer(3)}};
        // (price * qty > 250) AND NOT (qty = 0)  ->  (300 > 250) AND NOT false  ->  1 AND 1  ->  1
        ctx.equal(eval("(price * qty > 250) AND NOT (qty = 0)", cols).as_integer(), std::int64_t{1});
    }},
    {"a bind variable resolves through the caller-supplied variable resolver", [](auto& ctx) {
        sqlite::compiler::parser::parser p("?1 + 1");
        auto e = p.parse_expr();
        vm::aux::program_builder builder;
        code_generator gen(builder, /*first_scratch_register=*/1,
            [](std::string_view, std::string_view) -> std::int64_t { throw std::runtime_error("no columns"); },
            [](std::string_view) -> std::int64_t { return 0; });
        std::int64_t result = gen.allocate_register();
        gen.compile(*e, result);
        gen.finish();
        builder.add_op(vm::opcode::result_row, result, 1);
        builder.add_op(vm::opcode::halt, 0);

        vm::interpreter interp(builder.finish());
        interp.reg(0) = vm::aux::mem::make_integer(41);
        ctx.check(interp.run() == vm::run_status::row);
        ctx.equal(interp.reg(interp.row_start()).as_integer(), std::int64_t{42});
    }},
    {"expression kinds with no VM opcode yet throw code_generator_error rather than miscompiling", [](auto& ctx) {
        auto expect_unsupported = [&](std::string_view sql) {
            sqlite::compiler::parser::parser p(sql);
            auto e = p.parse_expr();
            vm::aux::program_builder builder;
            code_generator gen(builder, 0, [](std::string_view, std::string_view) -> std::int64_t { return 0; });
            ctx.template throws<code_generator_error>([&] { gen.compile(*e, gen.allocate_register()); });
        };
        expect_unsupported("a IN (1, 2)");
        expect_unsupported("a BETWEEN 1 AND 2");
        expect_unsupported("a LIKE 'x%'");
        expect_unsupported("CASE WHEN 1 THEN 2 END");
        expect_unsupported("count(*)");
        expect_unsupported("x'DEAD'");
        expect_unsupported("a IS b");
        expect_unsupported("a IS NULL");
        expect_unsupported("~a");
        expect_unsupported("a & b");
    }},
});

} // namespace sqlite::compiler::code_generator::testing
