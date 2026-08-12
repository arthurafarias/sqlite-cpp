#pragma once

#include "../keyword_table.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::compiler::tokenizer::testing {

using sqlite::utils::testing::test_group;

inline const test_group keyword_table_tests("keyword_table", {
    {"recognizes a keyword regardless of case", [](auto& ctx) {
        ctx.check(lookup_keyword("SELECT") == token_type::select);
        ctx.check(lookup_keyword("select") == token_type::select);
        ctx.check(lookup_keyword("SeLeCt") == token_type::select);
    }},
    {"recognizes keywords whose spelling collides with a C++ keyword", [](auto& ctx) {
        ctx.check(lookup_keyword("AND") == token_type::and_);
        ctx.check(lookup_keyword("or") == token_type::or_);
        ctx.check(lookup_keyword("Not") == token_type::not_);
        ctx.check(lookup_keyword("CASE") == token_type::case_);
        ctx.check(lookup_keyword("default") == token_type::default_);
    }},
    {"returns nullopt for a non-keyword identifier", [](auto& ctx) {
        ctx.check(!lookup_keyword("my_table").has_value());
        ctx.check(!lookup_keyword("SELECTX").has_value());
        ctx.check(!lookup_keyword("").has_value());
    }},
    {"CURRENT_TIMESTAMP and CURRENT are distinct keywords", [](auto& ctx) {
        ctx.check(lookup_keyword("CURRENT_TIMESTAMP") == token_type::current_timestamp);
        ctx.check(lookup_keyword("CURRENT") == token_type::current);
    }},
});

} // namespace sqlite::compiler::tokenizer::testing
