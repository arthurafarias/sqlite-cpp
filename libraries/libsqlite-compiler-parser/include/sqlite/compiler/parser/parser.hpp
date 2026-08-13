#pragma once

#include "ast/expr.hpp"
#include "ast/select_stmt.hpp"
#include "parse_error.hpp"

#include <sqlite/compiler/tokenizer/tokenizer.hpp>

#include <string>
#include <vector>

// Legacy source: parse.y's grammar rules for `expr` and `select` (compiled
// by LEMON into parse.c's LALR(1) tables; SRS S3.5.2, sqlite::compiler::parser).
// This is a hand-written recursive-descent/precedence-climbing parser, not a
// port of the generated table-driven automaton -- hand-transcribing LALR
// tables isn't a meaningful "port" (see this library's docs for the fuller
// reasoning). It implements the *same* grammar, including SQLite's actual
// operator precedence and associativity (parse.y's `%left`/`%right`
// declarations, reproduced in the parse_or/parse_and/.../parse_unary
// call chain below, each function documenting which precedence level it is),
// verified against real SQL text in testing/parser_test.hpp rather than by
// construction alone.
namespace sqlite::compiler::parser {

using tokenizer::token;
using tokenizer::token_type;

class parser {
public:
    explicit parser(std::string_view sql) : tokens_(tokenizer::tokenize(sql)) {}

    // The full precedence chain, lowest to highest (parse.y's declaration
    // order, low to high): OR, AND, NOT(prefix), {IS,MATCH,LIKE,BETWEEN,IN,
    // ISNULL,NOTNULL,NE,EQ}, {GT,LE,LT,GE}, {BITAND,BITOR,LSHIFT,RSHIFT},
    // {PLUS,MINUS}, {STAR,SLASH,REM}, {CONCAT,PTR}, unary {PLUS,MINUS,BITNOT}.
    // COLLATE is not implemented (SRS scope note in this library's docs).
    ast::expr_ptr parse_expr() { return parse_or(); }

    ast::select_stmt parse_select();

private:
    // --- token stream helpers ---
    [[nodiscard]] const token& peek() const {
        static const token eof{token_type::end_of_input, {}};
        return pos_ < tokens_.size() ? tokens_[pos_] : eof;
    }
    [[nodiscard]] bool check(token_type t) const { return peek().type == t; }
    token advance() { const token& t = peek(); if (pos_ < tokens_.size()) ++pos_; return t; }
    bool match(token_type t) { if (check(t)) { advance(); return true; } return false; }
    token expect(token_type t, const char* what) {
        if (!check(t)) throw parse_error(std::string("expected ") + what);
        return advance();
    }

    // --- expression grammar, one function per precedence level ---
    ast::expr_ptr parse_or();
    ast::expr_ptr parse_and();
    ast::expr_ptr parse_not_level();
    ast::expr_ptr parse_eq_level();
    ast::expr_ptr parse_rel_level();
    ast::expr_ptr parse_bitwise_level();
    ast::expr_ptr parse_additive();
    ast::expr_ptr parse_multiplicative();
    ast::expr_ptr parse_concat_level();
    ast::expr_ptr parse_unary();
    ast::expr_ptr parse_primary();
    ast::expr_ptr parse_case();
    ast::expr_ptr parse_function_call(std::string name);
    std::vector<ast::expr_ptr> parse_expr_list_until(token_type close);

    // --- SELECT grammar ---
    ast::result_column parse_result_column();
    ast::table_ref parse_table_ref();
    ast::join_clause parse_join();
    std::vector<ast::order_by_term> parse_order_by();

    static std::string unescape_string_literal(std::string_view raw);
    static std::string strip_blob_literal(std::string_view raw);

    std::vector<token> tokens_;
    std::size_t pos_ = 0;
};

// ============================================================================
// Expression grammar
// ============================================================================

inline ast::expr_ptr parser::parse_or() {
    ast::expr_ptr left = parse_and();
    while (match(token_type::or_)) {
        ast::expr_ptr right = parse_and();
        left = ast::make_binary(ast::binary_op::or_, std::move(left), std::move(right));
    }
    return left;
}

inline ast::expr_ptr parser::parse_and() {
    ast::expr_ptr left = parse_not_level();
    while (match(token_type::and_)) {
        ast::expr_ptr right = parse_not_level();
        left = ast::make_binary(ast::binary_op::and_, std::move(left), std::move(right));
    }
    return left;
}

inline ast::expr_ptr parser::parse_not_level() {
    if (match(token_type::not_)) {
        return ast::make_unary(ast::unary_op::not_, parse_not_level());
    }
    return parse_eq_level();
}

inline ast::expr_ptr parser::parse_eq_level() {
    ast::expr_ptr left = parse_rel_level();
    while (true) {
        if (match(token_type::eq)) {
            left = ast::make_binary(ast::binary_op::eq, std::move(left), parse_rel_level());
        } else if (match(token_type::ne)) {
            left = ast::make_binary(ast::binary_op::ne, std::move(left), parse_rel_level());
        } else if (check(token_type::isnull)) {
            advance();
            auto e = std::make_unique<ast::expr>();
            e->kind = ast::expr_kind::is_null_check;
            e->left = std::move(left);
            left = std::move(e);
        } else if (check(token_type::notnull)) {
            advance();
            auto e = std::make_unique<ast::expr>();
            e->kind = ast::expr_kind::is_null_check;
            e->negate = true;
            e->left = std::move(left);
            left = std::move(e);
        } else if (match(token_type::is)) {
            bool negate = match(token_type::not_);
            if (match(token_type::null)) {
                auto e = std::make_unique<ast::expr>();
                e->kind = ast::expr_kind::is_null_check;
                e->negate = negate;
                e->left = std::move(left);
                left = std::move(e);
            } else {
                ast::expr_ptr right = parse_rel_level();
                left = ast::make_binary(negate ? ast::binary_op::is_not : ast::binary_op::is,
                                         std::move(left), std::move(right));
            }
        } else if (check(token_type::not_) || check(token_type::in) ||
                   check(token_type::between) || check(token_type::like) ||
                   check(token_type::glob) || check(token_type::regexp) ||
                   check(token_type::match)) {
            bool negate = match(token_type::not_);
            if (match(token_type::in)) {
                expect(token_type::lp, "'(' after IN");
                auto e = std::make_unique<ast::expr>();
                e->kind = ast::expr_kind::in_list;
                e->negate = negate;
                e->left = std::move(left);
                e->list = parse_expr_list_until(token_type::rp);
                expect(token_type::rp, "')' to close IN list");
                left = std::move(e);
            } else if (match(token_type::between)) {
                auto e = std::make_unique<ast::expr>();
                e->kind = ast::expr_kind::between;
                e->negate = negate;
                e->left = std::move(left);
                e->low = parse_rel_level();
                expect(token_type::and_, "AND in BETWEEN");
                e->high = parse_rel_level();
                left = std::move(e);
            } else if (check(token_type::like) || check(token_type::glob) ||
                       check(token_type::regexp) || check(token_type::match)) {
                advance();
                auto e = std::make_unique<ast::expr>();
                e->kind = ast::expr_kind::like;
                e->negate = negate;
                e->left = std::move(left);
                e->right = parse_rel_level();
                if (match(token_type::escape)) e->escape = parse_rel_level();
                left = std::move(e);
            } else {
                throw parse_error("expected IN, BETWEEN, LIKE, GLOB, REGEXP, or MATCH after NOT");
            }
        } else {
            break;
        }
    }
    return left;
}

inline ast::expr_ptr parser::parse_rel_level() {
    ast::expr_ptr left = parse_bitwise_level();
    while (true) {
        ast::binary_op op;
        if (check(token_type::gt)) op = ast::binary_op::gt;
        else if (check(token_type::le)) op = ast::binary_op::le;
        else if (check(token_type::lt)) op = ast::binary_op::lt;
        else if (check(token_type::ge)) op = ast::binary_op::ge;
        else break;
        advance();
        left = ast::make_binary(op, std::move(left), parse_bitwise_level());
    }
    return left;
}

inline ast::expr_ptr parser::parse_bitwise_level() {
    ast::expr_ptr left = parse_additive();
    while (true) {
        ast::binary_op op;
        if (check(token_type::bitand_)) op = ast::binary_op::bit_and;
        else if (check(token_type::bitor_)) op = ast::binary_op::bit_or;
        else if (check(token_type::lshift)) op = ast::binary_op::shift_left;
        else if (check(token_type::rshift)) op = ast::binary_op::shift_right;
        else break;
        advance();
        left = ast::make_binary(op, std::move(left), parse_additive());
    }
    return left;
}

inline ast::expr_ptr parser::parse_additive() {
    ast::expr_ptr left = parse_multiplicative();
    while (true) {
        ast::binary_op op;
        if (check(token_type::plus)) op = ast::binary_op::add;
        else if (check(token_type::minus)) op = ast::binary_op::subtract;
        else break;
        advance();
        left = ast::make_binary(op, std::move(left), parse_multiplicative());
    }
    return left;
}

inline ast::expr_ptr parser::parse_multiplicative() {
    ast::expr_ptr left = parse_concat_level();
    while (true) {
        ast::binary_op op;
        if (check(token_type::star)) op = ast::binary_op::multiply;
        else if (check(token_type::slash)) op = ast::binary_op::divide;
        else if (check(token_type::rem)) op = ast::binary_op::modulo;
        else break;
        advance();
        left = ast::make_binary(op, std::move(left), parse_concat_level());
    }
    return left;
}

inline ast::expr_ptr parser::parse_concat_level() {
    ast::expr_ptr left = parse_unary();
    while (match(token_type::concat)) {
        left = ast::make_binary(ast::binary_op::concat, std::move(left), parse_unary());
    }
    return left;
}

inline ast::expr_ptr parser::parse_unary() {
    if (match(token_type::plus)) return ast::make_unary(ast::unary_op::plus, parse_unary());
    if (match(token_type::minus)) return ast::make_unary(ast::unary_op::negate, parse_unary());
    if (match(token_type::bitnot)) return ast::make_unary(ast::unary_op::bit_not, parse_unary());
    return parse_primary();
}

inline ast::expr_ptr parser::parse_primary() {
    const token& t = peek();
    switch (t.type) {
        case token_type::integer:
        case token_type::qnumber:
            advance();
            return ast::make_literal(ast::expr_kind::integer_literal, std::string(t.text));
        case token_type::float_:
            advance();
            return ast::make_literal(ast::expr_kind::float_literal, std::string(t.text));
        case token_type::string:
            advance();
            return ast::make_literal(ast::expr_kind::string_literal, unescape_string_literal(t.text));
        case token_type::blob:
            advance();
            return ast::make_literal(ast::expr_kind::blob_literal, strip_blob_literal(t.text));
        case token_type::null:
            advance();
            return ast::make_literal(ast::expr_kind::null_literal, {});
        case token_type::variable:
            advance();
            return ast::make_literal(ast::expr_kind::variable, std::string(t.text));
        case token_type::lp: {
            advance();
            ast::expr_ptr inner = parse_expr();
            expect(token_type::rp, "')' to close parenthesized expression");
            return inner;
        }
        case token_type::case_:
            return parse_case();
        case token_type::id: {
            advance();
            std::string name(t.text);
            if (check(token_type::lp)) return parse_function_call(std::move(name));
            if (match(token_type::dot)) {
                token col = expect(token_type::id, "column name after '.'");
                return ast::make_column_ref(std::string(col.text), std::move(name));
            }
            return ast::make_column_ref(std::move(name));
        }
        default:
            throw parse_error("expected an expression");
    }
}

inline ast::expr_ptr parser::parse_case() {
    expect(token_type::case_, "CASE");
    auto e = std::make_unique<ast::expr>();
    e->kind = ast::expr_kind::case_expr;
    if (!check(token_type::when)) e->case_base = parse_expr();
    if (!check(token_type::when)) throw parse_error("expected WHEN in CASE expression");
    while (match(token_type::when)) {
        ast::case_when_clause clause;
        clause.condition = parse_expr();
        expect(token_type::then, "THEN in CASE expression");
        clause.result = parse_expr();
        e->when_clauses.push_back(std::move(clause));
    }
    if (match(token_type::else_)) e->case_else = parse_expr();
    expect(token_type::end, "END to close CASE expression");
    return e;
}

inline ast::expr_ptr parser::parse_function_call(std::string name) {
    expect(token_type::lp, "'(' after function name");
    auto e = std::make_unique<ast::expr>();
    e->kind = ast::expr_kind::function_call;
    e->text = std::move(name);
    if (match(token_type::star)) {
        e->star = true;
    } else if (!check(token_type::rp)) {
        e->distinct = match(token_type::distinct);
        e->list = parse_expr_list_until(token_type::rp);
    }
    expect(token_type::rp, "')' to close function call");
    return e;
}

inline std::vector<ast::expr_ptr> parser::parse_expr_list_until(token_type close) {
    std::vector<ast::expr_ptr> list;
    if (check(close)) return list;
    list.push_back(parse_expr());
    while (match(token_type::comma)) list.push_back(parse_expr());
    return list;
}

inline std::string parser::unescape_string_literal(std::string_view raw) {
    // raw is the full token text including the surrounding single quotes,
    // e.g. "'it''s'". Strip the quotes and un-double any escaped ''.
    std::string out;
    if (raw.size() < 2) return out;
    std::string_view inner = raw.substr(1, raw.size() - 2);
    out.reserve(inner.size());
    for (std::size_t i = 0; i < inner.size(); ++i) {
        if (inner[i] == '\'' && i + 1 < inner.size() && inner[i + 1] == '\'') ++i;
        out.push_back(inner[i]);
    }
    return out;
}

inline std::string parser::strip_blob_literal(std::string_view raw) {
    // raw is e.g. "x'DEADBEEF'" -- keep just the hex digits.
    std::size_t quote = raw.find('\'');
    if (quote == std::string_view::npos || raw.size() < quote + 2) return {};
    return std::string(raw.substr(quote + 1, raw.size() - quote - 2));
}

// ============================================================================
// SELECT grammar (single SELECT: no compound SELECT, no CTEs, no subqueries
// in FROM -- see this library's docs for the deferred list).
// ============================================================================

inline ast::result_column parser::parse_result_column() {
    ast::result_column col;
    if (check(token_type::star)) {
        advance();
        col.star = true;
        return col;
    }
    if (check(token_type::id) && tokens_.size() > pos_ + 1 &&
        tokens_[pos_ + 1].type == token_type::dot &&
        tokens_.size() > pos_ + 2 && tokens_[pos_ + 2].type == token_type::star) {
        col.star = true;
        col.star_table = std::string(advance().text);
        advance(); // '.'
        advance(); // '*'
        return col;
    }
    col.expression = parse_expr();
    if (match(token_type::as)) {
        col.alias = std::string(expect(token_type::id, "alias after AS").text);
    } else if (check(token_type::id)) {
        col.alias = std::string(advance().text); // bare alias, no AS
    }
    return col;
}

inline ast::table_ref parser::parse_table_ref() {
    ast::table_ref ref;
    ref.name = std::string(expect(token_type::id, "table name").text);
    if (match(token_type::as)) {
        ref.alias = std::string(expect(token_type::id, "table alias after AS").text);
    } else if (check(token_type::id)) {
        ref.alias = std::string(advance().text);
    }
    return ref;
}

inline ast::join_clause parser::parse_join() {
    ast::join_clause join;
    bool natural = match(token_type::natural);
    if (match(token_type::left)) {
        match(token_type::outer); // optional OUTER after LEFT
        join.type = natural ? ast::join_type::natural_left_outer : ast::join_type::left_outer;
    } else if (match(token_type::inner)) {
        join.type = ast::join_type::inner;
    } else if (match(token_type::cross)) {
        join.type = ast::join_type::cross;
    } else {
        join.type = natural ? ast::join_type::natural_inner : ast::join_type::inner;
    }
    expect(token_type::join, "JOIN");
    join.table = parse_table_ref();
    if (match(token_type::on)) {
        join.on_condition = parse_expr();
    } else if (match(token_type::using_)) {
        expect(token_type::lp, "'(' after USING");
        join.using_columns.push_back(std::string(expect(token_type::id, "column name in USING").text));
        while (match(token_type::comma)) {
            join.using_columns.push_back(std::string(expect(token_type::id, "column name in USING").text));
        }
        expect(token_type::rp, "')' to close USING");
    }
    return join;
}

inline std::vector<ast::order_by_term> parser::parse_order_by() {
    std::vector<ast::order_by_term> terms;
    do {
        ast::order_by_term term;
        term.expression = parse_expr();
        if (match(token_type::desc)) term.descending = true;
        else match(token_type::asc);
        terms.push_back(std::move(term));
    } while (match(token_type::comma));
    return terms;
}

inline ast::select_stmt parser::parse_select() {
    expect(token_type::select, "SELECT");
    ast::select_stmt stmt;
    stmt.distinct = match(token_type::distinct);
    if (!stmt.distinct) match(token_type::all);

    stmt.columns.push_back(parse_result_column());
    while (match(token_type::comma)) stmt.columns.push_back(parse_result_column());

    if (match(token_type::from)) {
        stmt.from = parse_table_ref();
        while (check(token_type::comma) || check(token_type::join) || check(token_type::left) ||
               check(token_type::inner) || check(token_type::cross) || check(token_type::natural)) {
            if (match(token_type::comma)) {
                ast::join_clause join;
                join.type = ast::join_type::cross;
                join.table = parse_table_ref();
                stmt.joins.push_back(std::move(join));
            } else {
                stmt.joins.push_back(parse_join());
            }
        }
    }

    if (match(token_type::where)) stmt.where = parse_expr();

    if (match(token_type::group)) {
        expect(token_type::by, "BY after GROUP");
        stmt.group_by.push_back(parse_expr());
        while (match(token_type::comma)) stmt.group_by.push_back(parse_expr());
        if (match(token_type::having)) stmt.having = parse_expr();
    }

    if (match(token_type::order)) {
        expect(token_type::by, "BY after ORDER");
        stmt.order_by = parse_order_by();
    }

    if (match(token_type::limit)) {
        stmt.limit = parse_expr();
        if (match(token_type::offset)) {
            stmt.offset = parse_expr();
        } else if (match(token_type::comma)) {
            // legacy "LIMIT offset, count" form: swap so `limit`/`offset`
            // always mean the same thing regardless of which spelling was used.
            stmt.offset = std::move(stmt.limit);
            stmt.limit = parse_expr();
        }
    }

    return stmt;
}

} // namespace sqlite::compiler::parser
