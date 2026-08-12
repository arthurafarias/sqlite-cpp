#pragma once

#include "../../testing/test_group.hpp"
#include "../printf.hpp"

namespace sqlite::utils::text::testing {

using sqlite::utils::testing::test_group;

inline const test_group printf_tests("printf", {
    {"formats decimal, hex, octal, and string conversions", [](auto& ctx) {
        ctx.equal(format("%d", 42), std::string{"42"});
        ctx.equal(format("%d", -7), std::string{"-7"});
        ctx.equal(format("%x", 255), std::string{"ff"});
        ctx.equal(format("%X", 255), std::string{"FF"});
        ctx.equal(format("%o", 8), std::string{"10"});
        ctx.equal(format("%s", "abc"), std::string{"abc"});
        ctx.equal(format("%%"), std::string{"%"});
    }},
    {"applies width and zero-padding to numeric conversions", [](auto& ctx) {
        ctx.equal(format("%5d", 42), std::string{"   42"});
        ctx.equal(format("%-5d|", 42), std::string{"42   |"});
        ctx.equal(format("%05d", 42), std::string{"00042"});
        ctx.equal(format("%05d", -42), std::string{"-0042"});
    }},
    {"%q doubles single quotes without wrapping", [](auto& ctx) {
        ctx.equal(format("%q", "O'Brien"), std::string{"O''Brien"});
    }},
    {"%Q wraps in single quotes, doubles embedded quotes, and prints NULL for a null arg", [](auto& ctx) {
        ctx.equal(format("%Q", "O'Brien"), std::string{"'O''Brien'"});
        ctx.equal(format("%Q", nullptr), std::string{"NULL"});
    }},
    {"%w doubles double-quotes", [](auto& ctx) {
        ctx.equal(format("%w", "a\"b"), std::string{"a\"\"b"});
    }},
    {"multiple conversions consume arguments left to right", [](auto& ctx) {
        ctx.equal(format("%s=%d, %s=%d", "a", 1, "b", 2), std::string{"a=1, b=2"});
    }},
    {"precision truncates %s", [](auto& ctx) {
        ctx.equal(format("%.3s", "abcdef"), std::string{"abc"});
    }},
    {"%f formats a float with the default and an explicit precision", [](auto& ctx) {
        ctx.equal(format("%.2f", 3.14159), std::string{"3.14"});
        ctx.equal(format("%.0f", 3.7), std::string{"4"});
    }},
    {"an unrecognized conversion is emitted verbatim", [](auto& ctx) {
        ctx.equal(format("%y"), std::string{"%y"});
    }},
});

} // namespace sqlite::utils::text::testing
