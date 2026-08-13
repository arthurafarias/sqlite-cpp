#pragma once

#include "../ast/expr.hpp"

#include <string>

// Test-only support (not a legacy port): renders an expr tree as a compact
// s-expression so precedence/associativity tests can assert against a
// single string instead of walking the tree node-by-node by hand.
namespace sqlite::compiler::parser::testing {

inline std::string unary_op_symbol(ast::unary_op op) {
    switch (op) {
        case ast::unary_op::plus: return "u+";
        case ast::unary_op::negate: return "u-";
        case ast::unary_op::bit_not: return "~";
        case ast::unary_op::not_: return "not";
    }
    return "?";
}

inline std::string binary_op_symbol(ast::binary_op op) {
    switch (op) {
        case ast::binary_op::add: return "+";
        case ast::binary_op::subtract: return "-";
        case ast::binary_op::multiply: return "*";
        case ast::binary_op::divide: return "/";
        case ast::binary_op::modulo: return "%";
        case ast::binary_op::concat: return "||";
        case ast::binary_op::eq: return "=";
        case ast::binary_op::ne: return "!=";
        case ast::binary_op::lt: return "<";
        case ast::binary_op::le: return "<=";
        case ast::binary_op::gt: return ">";
        case ast::binary_op::ge: return ">=";
        case ast::binary_op::and_: return "and";
        case ast::binary_op::or_: return "or";
        case ast::binary_op::bit_and: return "&";
        case ast::binary_op::bit_or: return "|";
        case ast::binary_op::shift_left: return "<<";
        case ast::binary_op::shift_right: return ">>";
        case ast::binary_op::is: return "is";
        case ast::binary_op::is_not: return "is_not";
    }
    return "?";
}

inline std::string to_sexpr(const ast::expr* e) {
    if (e == nullptr) return "null";
    switch (e->kind) {
        case ast::expr_kind::integer_literal:
        case ast::expr_kind::float_literal:
        case ast::expr_kind::string_literal:
        case ast::expr_kind::blob_literal:
        case ast::expr_kind::variable:
            return e->text;
        case ast::expr_kind::null_literal:
            return "NULL";
        case ast::expr_kind::column_ref:
            return e->table_qualifier.empty() ? e->text : e->table_qualifier + "." + e->text;
        case ast::expr_kind::unary:
            return "(" + unary_op_symbol(e->u_op) + " " + to_sexpr(e->left.get()) + ")";
        case ast::expr_kind::binary:
            return "(" + binary_op_symbol(e->b_op) + " " + to_sexpr(e->left.get()) + " " +
                   to_sexpr(e->right.get()) + ")";
        case ast::expr_kind::is_null_check:
            return std::string("(") + (e->negate ? "notnull " : "isnull ") + to_sexpr(e->left.get()) + ")";
        case ast::expr_kind::in_list: {
            std::string s = std::string("(") + (e->negate ? "not-in " : "in ") + to_sexpr(e->left.get());
            for (const auto& item : e->list) s += " " + to_sexpr(item.get());
            return s + ")";
        }
        case ast::expr_kind::between:
            return std::string("(") + (e->negate ? "not-between " : "between ") + to_sexpr(e->left.get()) +
                   " " + to_sexpr(e->low.get()) + " " + to_sexpr(e->high.get()) + ")";
        case ast::expr_kind::like: {
            std::string s = std::string("(") + (e->negate ? "not-like " : "like ") + to_sexpr(e->left.get()) +
                             " " + to_sexpr(e->right.get());
            if (e->escape) s += " escape " + to_sexpr(e->escape.get());
            return s + ")";
        }
        case ast::expr_kind::case_expr: {
            std::string s = "(case";
            if (e->case_base) s += " " + to_sexpr(e->case_base.get());
            for (const auto& w : e->when_clauses) {
                s += " (when " + to_sexpr(w.condition.get()) + " then " + to_sexpr(w.result.get()) + ")";
            }
            if (e->case_else) s += " (else " + to_sexpr(e->case_else.get()) + ")";
            return s + ")";
        }
        case ast::expr_kind::function_call: {
            std::string s = "(call " + e->text;
            if (e->star) s += " *";
            if (e->distinct) s += " distinct";
            for (const auto& arg : e->list) s += " " + to_sexpr(arg.get());
            return s + ")";
        }
    }
    return "?";
}

inline std::string to_sexpr(const ast::expr_ptr& e) { return to_sexpr(e.get()); }

} // namespace sqlite::compiler::parser::testing
