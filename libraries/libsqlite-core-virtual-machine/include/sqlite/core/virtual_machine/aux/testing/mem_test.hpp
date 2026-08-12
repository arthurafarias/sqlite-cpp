#pragma once

#include "../mem.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::core::virtual_machine::aux::testing {

using sqlite::utils::testing::test_group;

inline const test_group mem_tests("mem", {
    {"make_* constructors report the right type()", [](auto& ctx) {
        ctx.check(mem::make_null().type() == value_type::null);
        ctx.check(mem::make_integer(1).type() == value_type::integer);
        ctx.check(mem::make_real(1.0).type() == value_type::real);
        ctx.check(mem::make_text("x").type() == value_type::text);
        ctx.check(mem::make_blob({}).type() == value_type::blob);
    }},
    {"is_null is true only for the null constructor", [](auto& ctx) {
        ctx.check(mem::make_null().is_null());
        ctx.check(!mem::make_integer(0).is_null());
        ctx.check(!mem::make_text("").is_null());
    }},
    {"as_integer coerces real by truncation and text by leading-prefix parse", [](auto& ctx) {
        ctx.equal(mem::make_real(3.9).as_integer(), std::int64_t{3});
        ctx.equal(mem::make_real(-3.9).as_integer(), std::int64_t{-3});
        ctx.equal(mem::make_text("42").as_integer(), std::int64_t{42});
        ctx.equal(mem::make_text("42abc").as_integer(), std::int64_t{42});
        ctx.equal(mem::make_text("  7").as_integer(), std::int64_t{7});
        ctx.equal(mem::make_text("abc").as_integer(), std::int64_t{0});
    }},
    {"as_real coerces integer exactly and text by leading-prefix parse", [](auto& ctx) {
        ctx.equal(mem::make_integer(5).as_real(), 5.0);
        ctx.equal(mem::make_text("3.5").as_real(), 3.5);
        ctx.equal(mem::make_text("nope").as_real(), 0.0);
    }},
    {"as_text stringifies integer and real", [](auto& ctx) {
        ctx.equal(mem::make_integer(42).as_text(), std::string{"42"});
        ctx.equal(mem::make_integer(-1).as_text(), std::string{"-1"});
        ctx.equal(mem::make_real(2.5).as_text(), std::string{"2.5"});
    }},
    {"is_numeric requires the full text to parse as a number", [](auto& ctx) {
        ctx.check(mem::make_integer(1).is_numeric());
        ctx.check(mem::make_real(1.0).is_numeric());
        ctx.check(mem::make_text("42").is_numeric());
        ctx.check(mem::make_text("3.5").is_numeric());
        ctx.check(!mem::make_text("42abc").is_numeric());
        ctx.check(!mem::make_text("").is_numeric());
        ctx.check(!mem::make_blob({}).is_numeric());
    }},
    {"compare orders NULL < NUMERIC < TEXT < BLOB", [](auto& ctx) {
        ctx.check(compare(mem::make_null(), mem::make_integer(0)) < 0);
        ctx.check(compare(mem::make_integer(1000000), mem::make_text("0")) < 0);
        ctx.check(compare(mem::make_text("zzz"), mem::make_blob({})) < 0);
    }},
    {"compare orders integers and reals numerically, mixed or not", [](auto& ctx) {
        ctx.check(compare(mem::make_integer(1), mem::make_integer(2)) < 0);
        ctx.check(compare(mem::make_real(2.5), mem::make_real(1.5)) > 0);
        ctx.check(compare(mem::make_integer(2), mem::make_real(2.0)) == 0);
        ctx.check(compare(mem::make_integer(3), mem::make_real(2.5)) > 0);
    }},
    {"compare orders text byte-wise", [](auto& ctx) {
        ctx.check(compare(mem::make_text("abc"), mem::make_text("abd")) < 0);
        ctx.check(compare(mem::make_text("abc"), mem::make_text("abc")) == 0);
    }},
});

} // namespace sqlite::core::virtual_machine::aux::testing
