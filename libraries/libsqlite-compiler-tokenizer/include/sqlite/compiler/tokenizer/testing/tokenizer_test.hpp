#pragma once

#include "../tokenizer.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::compiler::tokenizer::testing {

using sqlite::utils::testing::test_group;

namespace {
token single(std::string_view sql) { return next_token(sql); }
} // namespace

inline const test_group tokenizer_tests("tokenizer", {
    {"empty input is end_of_input", [](auto& ctx) {
        ctx.check(next_token("").type == token_type::end_of_input);
    }},
    {"whitespace runs are one space token", [](auto& ctx) {
        token t = single("   \t\n  select");
        ctx.check(t.type == token_type::space);
        ctx.equal(t.text, std::string_view{"   \t\n  "});
    }},
    {"a keyword lexes to its own token type, not TK_ID", [](auto& ctx) {
        token t = single("SELECT * FROM t");
        ctx.check(t.type == token_type::select);
        ctx.equal(t.text, std::string_view{"SELECT"});
    }},
    {"an identifier that isn't a keyword lexes as id", [](auto& ctx) {
        token t = single("my_table_1");
        ctx.check(t.type == token_type::id);
        ctx.equal(t.text, std::string_view{"my_table_1"});
    }},
    {"single-character punctuation and operators", [](auto& ctx) {
        struct case_t { std::string_view sql; token_type type; };
        case_t cases[] = {
            {"(", token_type::lp}, {")", token_type::rp}, {";", token_type::semi},
            {"+", token_type::plus}, {"*", token_type::star}, {"%", token_type::rem},
            {",", token_type::comma}, {"&", token_type::bitand_}, {"~", token_type::bitnot},
            {".", token_type::dot}, {"/", token_type::slash}, {"-", token_type::minus},
        };
        for (auto& c : cases) {
            token t = single(c.sql);
            ctx.check(t.type == c.type);
            ctx.equal(t.text.size(), std::size_t{1});
        }
    }},
    {"two-character operators are not split into two single-character tokens", [](auto& ctx) {
        struct case_t { std::string_view sql; token_type type; };
        case_t cases[] = {
            {"==", token_type::eq}, {"<=", token_type::le}, {"<>", token_type::ne},
            {"!=", token_type::ne}, {"<<", token_type::lshift}, {">=", token_type::ge},
            {">>", token_type::rshift}, {"||", token_type::concat}, {"->", token_type::ptr},
        };
        for (auto& c : cases) {
            token t = single(c.sql);
            ctx.check(t.type == c.type);
            ctx.equal(t.text.size(), std::size_t{2});
        }
    }},
    {"->> is one three-character token, not -> followed by >", [](auto& ctx) {
        token t = single("->>x");
        ctx.check(t.type == token_type::ptr);
        ctx.equal(t.text, std::string_view{"->>"});
    }},
    {"a lone ! is illegal but != is not-equal", [](auto& ctx) {
        ctx.check(single("!").type == token_type::illegal);
        ctx.check(single("!=").type == token_type::ne);
    }},
    {"single-quoted strings, with a doubled quote as an escaped literal quote", [](auto& ctx) {
        token t = single("'it''s'");
        ctx.check(t.type == token_type::string);
        ctx.equal(t.text, std::string_view{"'it''s'"});
    }},
    {"an unterminated string is illegal", [](auto& ctx) {
        ctx.check(single("'abc").type == token_type::illegal);
    }},
    {"double-quoted and backtick-quoted text lex as quoted identifiers, not strings", [](auto& ctx) {
        ctx.check(single("\"col\"").type == token_type::id);
        ctx.check(single("`col`").type == token_type::id);
    }},
    {"bracket-quoted identifiers", [](auto& ctx) {
        token t = single("[my col]");
        ctx.check(t.type == token_type::id);
        ctx.equal(t.text, std::string_view{"[my col]"});
    }},
    {"an unterminated bracket-quoted identifier is illegal", [](auto& ctx) {
        ctx.check(single("[abc").type == token_type::illegal);
    }},
    {"line comments run to end of line, not past it", [](auto& ctx) {
        token t = single("-- a comment\nSELECT");
        ctx.check(t.type == token_type::comment);
        ctx.equal(t.text, std::string_view{"-- a comment"});
    }},
    {"block comments, including ones spanning no extra text", [](auto& ctx) {
        token t = single("/* hi */x");
        ctx.check(t.type == token_type::comment);
        ctx.equal(t.text, std::string_view{"/* hi */"});
    }},
    {"an unterminated block comment consumes to end of input and is still a comment", [](auto& ctx) {
        token t = single("/* never closed");
        ctx.check(t.type == token_type::comment);
        ctx.equal(t.text, std::string_view{"/* never closed"});
    }},
    {"integer, float, and hex-integer literals", [](auto& ctx) {
        struct case_t { std::string_view sql; token_type type; };
        case_t cases[] = {
            {"123", token_type::integer}, {"0", token_type::integer},
            {"3.14", token_type::float_}, {".5", token_type::float_},
            {"1e10", token_type::float_}, {"1.5e-3", token_type::float_},
            {"0x1F", token_type::integer}, {"0XCAFE", token_type::integer},
        };
        for (auto& c : cases) {
            token t = single(c.sql);
            ctx.check(t.type == c.type);
            ctx.equal(t.text, c.sql);
        }
    }},
    {"a digit run immediately followed by an identifier character is illegal", [](auto& ctx) {
        token t = single("123abc");
        ctx.check(t.type == token_type::illegal);
    }},
    {"a digit separator produces a qnumber", [](auto& ctx) {
        token t = single("1_000_000");
        ctx.check(t.type == token_type::qnumber);
        ctx.equal(t.text, std::string_view{"1_000_000"});
    }},
    {"blob literals", [](auto& ctx) {
        token t = single("x'DEADBEEF'");
        ctx.check(t.type == token_type::blob);
        ctx.equal(t.text, std::string_view{"x'DEADBEEF'"});
    }},
    {"a blob literal with an odd number of hex digits is illegal", [](auto& ctx) {
        ctx.check(single("x'ABC'").type == token_type::illegal);
    }},
    {"numbered and named bind variables", [](auto& ctx) {
        ctx.check(single("?").type == token_type::variable);
        ctx.check(single("?5").type == token_type::variable);
        token t = single(":name");
        ctx.check(t.type == token_type::variable);
        ctx.equal(t.text, std::string_view{":name"});
        ctx.check(single("@abc").type == token_type::variable);
        ctx.check(single("$abc").type == token_type::variable);
    }},
    {"tokenize() drops whitespace and comments by default", [](auto& ctx) {
        auto tokens = tokenize("SELECT  1 -- comment\n FROM t;");
        std::vector<token_type> types;
        for (auto& t : tokens) types.push_back(t.type);
        std::vector<token_type> expected{
            token_type::select, token_type::integer, token_type::from,
            token_type::id, token_type::semi,
        };
        ctx.check(types == expected);
    }},
    {"tokenize() keeps whitespace and comments when asked", [](auto& ctx) {
        auto tokens = tokenize("A B", /*skip_trivia=*/false);
        ctx.equal(tokens.size(), std::size_t{3});
        ctx.check(tokens[1].type == token_type::space);
    }},
    {"a full CREATE TABLE statement tokenizes to the expected keyword/identifier/punctuation sequence", [](auto& ctx) {
        auto tokens = tokenize("CREATE TABLE t(a INTEGER PRIMARY KEY, b TEXT);");
        std::vector<token_type> types;
        for (auto& t : tokens) types.push_back(t.type);
        std::vector<token_type> expected{
            token_type::create, token_type::table, token_type::id, token_type::lp,
            token_type::id, token_type::id, token_type::primary, token_type::key,
            token_type::comma, token_type::id, token_type::id, token_type::rp,
            token_type::semi,
        };
        ctx.check(types == expected);
    }},
});

} // namespace sqlite::compiler::tokenizer::testing
