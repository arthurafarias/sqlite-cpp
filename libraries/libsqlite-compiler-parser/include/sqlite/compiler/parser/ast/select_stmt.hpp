#pragma once

#include "expr.hpp"

#include <optional>
#include <string>
#include <vector>

// Legacy source: the AST-construction portions of select.c/build.c -- the
// `Select`/`SrcList`/`ExprList` structs (SRS S3.5.2,
// sqlite::compiler::parser::ast). Scoped to a single SELECT (no compound
// SELECT via UNION/INTERSECT/EXCEPT, no CTEs, no subqueries in FROM) --
// see this library's docs for the full list of what's deferred and why.
namespace sqlite::compiler::parser::ast {

// A result column: either `expr [AS alias]`, `*`, or `table.*`.
struct result_column {
    expr_ptr expression;   // null when star is true
    std::string alias;     // empty if no AS-alias
    bool star = false;
    std::string star_table; // for "table.*"; empty for a bare "*"
};

struct table_ref {
    std::string name;
    std::string alias;   // empty if no alias
};

enum class join_type { inner, left_outer, cross, natural_inner, natural_left_outer };

struct join_clause {
    join_type type = join_type::inner;
    table_ref table;
    expr_ptr on_condition;              // null if this join uses USING or neither
    std::vector<std::string> using_columns; // empty if this join uses ON or neither
};

struct order_by_term {
    expr_ptr expression;
    bool descending = false;
};

struct select_stmt {
    bool distinct = false;
    std::vector<result_column> columns;
    std::optional<table_ref> from;
    std::vector<join_clause> joins;
    expr_ptr where;
    std::vector<expr_ptr> group_by;
    expr_ptr having;
    std::vector<order_by_term> order_by;
    expr_ptr limit;
    expr_ptr offset;
};

} // namespace sqlite::compiler::parser::ast
