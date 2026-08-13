#pragma once

#include <memory>
#include <string>
#include <vector>

// Legacy source: the AST-construction portions of build.c/expr.c -- the
// `Expr` struct (expr.c/sqliteInt.h) and the node-allocation helpers
// (sqlite3PExpr, sqlite3ExprAnd, sqlite3ExprFunction, etc.) that turn parsed
// tokens into a tree (SRS S3.5.2, sqlite::compiler::parser::ast).
//
// Deliberate modernization vs. legacy's `Expr`: the legacy struct is a
// single node type reused for every expression form, distinguishing them by
// an `op` byte and giving each op its own reading of `pLeft`/`pRight`/
// `pList`/`u.zToken` -- optimized for a fixed-size arena allocator. This
// port keeps the same one-struct-many-kinds shape (matching this class of
// tree closely enough that a reader who knows `Expr` will recognize it
// immediately) but names each op's operand slots for what they hold, and
// owns children through std::unique_ptr rather than a custom allocator.
namespace sqlite::compiler::parser::ast {

enum class expr_kind {
    integer_literal,
    float_literal,
    string_literal,
    blob_literal,
    null_literal,
    variable,        // ?, ?N, :name, @name, $name
    column_ref,       // [table.]column
    unary,
    binary,
    is_null_check,    // IS NULL / ISNULL / NOTNULL (negate=true) / IS NOT NULL (negate=true)
    in_list,          // expr [NOT] IN (expr, expr, ...)
    between,          // expr [NOT] BETWEEN low AND high
    like,             // expr [NOT] LIKE/GLOB/REGEXP/MATCH pattern [ESCAPE expr]
    case_expr,        // CASE [base] WHEN cond THEN result ... [ELSE else] END
    function_call,    // name(args...), name(DISTINCT args...), or name(*)
};

enum class unary_op { plus, negate, bit_not, not_ };

enum class binary_op {
    add, subtract, multiply, divide, modulo, concat,
    eq, ne, lt, le, gt, ge,
    and_, or_,
    bit_and, bit_or, shift_left, shift_right,
    is, is_not,
};

struct expr;
using expr_ptr = std::unique_ptr<expr>;

struct case_when_clause {
    expr_ptr condition;
    expr_ptr result;
};

struct expr {
    expr_kind kind;

    // Literal / name payload: the literal's text (integer/float/string/blob,
    // already unescaped for string_literal), the variable's name/number, the
    // column or function name.
    std::string text;
    std::string table_qualifier;   // column_ref's "table" in "table.column"; empty if unqualified

    unary_op u_op{};
    binary_op b_op{};

    // Operand slots, named per the kind that uses them:
    //   unary:        left
    //   binary:       left, right
    //   is_null_check: left
    //   in_list:      left (the tested value), list (the candidates)
    //   between:      left (the value), low, high
    //   like:         left (the value), right (the pattern), escape (optional)
    //   case_expr:    case_base (optional), when_clauses, case_else (optional)
    //   function_call: list (the arguments)
    expr_ptr left;
    expr_ptr right;
    expr_ptr low;
    expr_ptr high;
    expr_ptr escape;
    expr_ptr case_base;
    expr_ptr case_else;
    std::vector<expr_ptr> list;
    std::vector<case_when_clause> when_clauses;

    bool negate = false;    // NOT IN / NOT BETWEEN / NOT LIKE / IS NOT NULL
    bool distinct = false;  // function_call: COUNT(DISTINCT x)
    bool star = false;      // function_call: COUNT(*)
};

inline expr_ptr make_literal(expr_kind kind, std::string text) {
    auto e = std::make_unique<expr>();
    e->kind = kind;
    e->text = std::move(text);
    return e;
}

inline expr_ptr make_column_ref(std::string column, std::string table_qualifier = {}) {
    auto e = std::make_unique<expr>();
    e->kind = expr_kind::column_ref;
    e->text = std::move(column);
    e->table_qualifier = std::move(table_qualifier);
    return e;
}

inline expr_ptr make_unary(unary_op op, expr_ptr operand) {
    auto e = std::make_unique<expr>();
    e->kind = expr_kind::unary;
    e->u_op = op;
    e->left = std::move(operand);
    return e;
}

inline expr_ptr make_binary(binary_op op, expr_ptr left, expr_ptr right) {
    auto e = std::make_unique<expr>();
    e->kind = expr_kind::binary;
    e->b_op = op;
    e->left = std::move(left);
    e->right = std::move(right);
    return e;
}

} // namespace sqlite::compiler::parser::ast
