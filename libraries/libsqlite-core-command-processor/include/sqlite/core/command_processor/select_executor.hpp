#pragma once

#include "btree_table_descriptor.hpp"
#include "query_error.hpp"
#include "query_result.hpp"
#include "record.hpp"
#include "table_descriptor.hpp"

#include <sqlite/backend/tree/cursor.hpp>
#include <sqlite/compiler/code_generator/code_generator.hpp>
#include <sqlite/compiler/code_generator/code_generator_error.hpp>
#include <sqlite/compiler/parser/ast/select_stmt.hpp>
#include <sqlite/core/virtual_machine/aux/program_builder.hpp>
#include <sqlite/core/virtual_machine/interpreter.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Legacy source: prepare.c/select.c/where.c's single-SELECT orchestration
// path -- turning a validated AST into a runnable program and executing it
// (SRS S3.4.2, sqlite::core::command_processor::query). This is command
// processor "phase 1": single-table SELECT only. See docs/index.md for the
// exact scope and, critically, *why* no new VM opcode was needed to build
// this: sqlite-compiler-code-generator's column_resolver already returns a
// register index rather than a value (this library decides that mapping
// and binds each row's values into those exact registers via
// interpreter::reg() before every run()), and
// sqlite::core::virtual_machine::interpreter::reset() already supports
// cheaply rerunning one compiled program per row.
//
// Two row sources share the same compiled-program/execution core below,
// via the detail::row_source callback: table_descriptor's in-memory rows,
// and btree_table_descriptor's real sqlite::backend::tree::cursor, decoded
// through record.hpp. Everything downstream of "get the next row" --
// WHERE evaluation, DISTINCT, ORDER BY, LIMIT/OFFSET -- is identical
// either way.
//
// Everything genuinely out of scope for *expressions* (function calls,
// CASE, IN, BETWEEN, LIKE, IS [NOT] NULL, bitwise operators, blob
// literals) is already rejected by sqlite-compiler-code-generator itself
// (code_generator_error, caught and rewrapped as query_error below) --
// this library only adds the validation specific to statement-level scope
// that code_generator has no way to know about: JOIN, GROUP BY/HAVING, and
// table/column name resolution.
namespace sqlite::core::command_processor {

namespace ast = sqlite::compiler::parser::ast;
namespace cg = sqlite::compiler::code_generator;
namespace vm = sqlite::core::virtual_machine;

namespace detail {

inline bool iequals(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) return false;
    }
    return true;
}

// Returns std::nullopt once every row has been produced. The in-memory
// path captures a vector index; the real-btree path captures a
// tree::cursor and decodes each payload on demand.
using row_source = std::function<std::optional<std::vector<vm::aux::mem>>()>;

inline row_source single_empty_row_source() {
    return [done = false]() mutable -> std::optional<std::vector<vm::aux::mem>> {
        if (done) return std::nullopt;
        done = true;
        return std::vector<vm::aux::mem>{};
    };
}

} // namespace detail

class select_executor {
public:
    // In-memory table_descriptor (see that header's docs for why it's an
    // explicit schema/storage stand-in).
    query_result execute(const ast::select_stmt& stmt, const table_descriptor& table) const {
        validate_statement_scope(stmt);

        std::vector<std::string> columns;
        std::optional<std::string> table_alias;
        detail::row_source next_row = detail::single_empty_row_source();
        if (stmt.from) {
            if (!detail::iequals(stmt.from->name, table.name)) {
                throw query_error("no such table: " + stmt.from->name);
            }
            columns = table.columns;
            if (!stmt.from->alias.empty()) table_alias = stmt.from->alias;

            next_row = [&table, i = std::size_t{0}]() mutable -> std::optional<std::vector<vm::aux::mem>> {
                if (i >= table.rows.size()) return std::nullopt;
                return table.rows[i++];
            };
        }

        try {
            return execute_impl(stmt, table.name, columns, next_row, table_alias);
        } catch (const cg::code_generator_error& e) {
            throw query_error(e.what());
        }
    }

    // Real, on-disk-backed table via sqlite::backend::tree -- see
    // btree_table_descriptor.hpp and record.hpp.
    query_result execute(const ast::select_stmt& stmt, const btree_table_descriptor& table) const {
        validate_statement_scope(stmt);

        std::vector<std::string> columns;
        std::optional<std::string> table_alias;
        std::optional<tree::cursor> cur;
        detail::row_source next_row = detail::single_empty_row_source();
        if (stmt.from) {
            if (!detail::iequals(stmt.from->name, table.name)) {
                throw query_error("no such table: " + stmt.from->name);
            }
            columns = table.columns;
            if (!stmt.from->alias.empty()) table_alias = stmt.from->alias;

            cur.emplace(table.tree_ref);
            next_row = [&cur]() mutable -> std::optional<std::vector<vm::aux::mem>> {
                if (cur->eof()) return std::nullopt;
                std::vector<vm::aux::mem> row = record::decode_record(cur->payload());
                cur->next();
                return row;
            };
        }

        try {
            return execute_impl(stmt, table.name, columns, next_row, table_alias);
        } catch (const cg::code_generator_error& e) {
            throw query_error(e.what());
        }
    }

private:
    static void validate_statement_scope(const ast::select_stmt& stmt) {
        if (!stmt.joins.empty()) throw query_error("JOIN is not supported by this command-processor pass");
        if (!stmt.group_by.empty() || stmt.having) {
            throw query_error("GROUP BY/HAVING is not supported by this command-processor pass");
        }
        if (stmt.columns.empty()) throw query_error("SELECT with no result columns");
    }

    static bool qualifier_matches(std::string_view qualifier, const std::string& table_name,
                                   const std::optional<std::string>& alias) {
        if (alias) return detail::iequals(qualifier, *alias);
        return detail::iequals(qualifier, table_name);
    }

    static std::int64_t resolve_column(std::string_view qualifier, std::string_view column,
                                        const std::vector<std::string>& columns, const std::string& table_name,
                                        const std::optional<std::string>& alias) {
        if (!qualifier.empty() && !qualifier_matches(qualifier, table_name, alias)) {
            throw query_error("no such table: " + std::string(qualifier));
        }
        for (std::size_t i = 0; i < columns.size(); ++i) {
            if (detail::iequals(columns[i], column)) return static_cast<std::int64_t>(i);
        }
        throw query_error("no such column: " + std::string(column));
    }

    static std::string default_column_name(const ast::expr& e) {
        if (e.kind == ast::expr_kind::column_ref) return e.text;
        return "column";
    }

    query_result execute_impl(const ast::select_stmt& stmt, const std::string& table_name,
                               const std::vector<std::string>& columns, detail::row_source next_row,
                               const std::optional<std::string>& table_alias) const {
        vm::aux::program_builder builder;
        cg::code_generator gen(builder, static_cast<std::int64_t>(columns.size()),
            [&](std::string_view qualifier, std::string_view column) {
                return resolve_column(qualifier, column, columns, table_name, table_alias);
            });

        std::int64_t where_jump_idx = -1;
        if (stmt.where) {
            std::int64_t where_reg = gen.allocate_register();
            gen.compile(*stmt.where, where_reg);
            where_jump_idx = builder.add_op(vm::opcode::if_not, where_reg, 0); // patched below
        }

        std::vector<std::string> output_names;
        std::vector<std::int64_t> output_regs;
        for (const auto& rc : stmt.columns) {
            if (rc.star) {
                if (!rc.star_table.empty() && !qualifier_matches(rc.star_table, table_name, table_alias)) {
                    throw query_error("no such table: " + rc.star_table);
                }
                for (std::size_t i = 0; i < columns.size(); ++i) {
                    std::int64_t r = gen.allocate_register();
                    builder.add_op(vm::opcode::copy, static_cast<std::int64_t>(i), r);
                    output_names.push_back(columns[i]);
                    output_regs.push_back(r);
                }
            } else {
                std::int64_t r = gen.allocate_register();
                gen.compile(*rc.expression, r);
                output_names.push_back(rc.alias.empty() ? default_column_name(*rc.expression) : rc.alias);
                output_regs.push_back(r);
            }
        }

        std::vector<std::int64_t> order_regs;
        for (const auto& term : stmt.order_by) {
            std::int64_t r = gen.allocate_register();
            gen.compile(*term.expression, r);
            order_regs.push_back(r);
        }

        // result_row needs a contiguous register block; output_regs is
        // interleaved with scratch registers used while compiling, so copy
        // each into a fresh contiguous range right before emitting it.
        std::int64_t row_base = gen.allocate_register();
        for (std::size_t i = 0; i < output_regs.size(); ++i) {
            std::int64_t dest = (i == 0) ? row_base : gen.allocate_register();
            builder.add_op(vm::opcode::copy, output_regs[i], dest);
        }
        builder.add_op(vm::opcode::result_row, row_base, static_cast<std::int64_t>(output_regs.size()));
        std::int64_t halt_idx = builder.add_op(vm::opcode::halt, 0);
        if (where_jump_idx != -1) builder.set_p2(where_jump_idx, halt_idx);

        gen.finish();
        vm::interpreter interp(builder.finish());

        struct collected_row {
            std::vector<vm::aux::mem> order_keys;
            std::vector<vm::aux::mem> values;
        };
        std::vector<collected_row> collected;

        while (std::optional<std::vector<vm::aux::mem>> row = next_row()) {
            interp.reset();
            for (std::size_t i = 0; i < columns.size() && i < row->size(); ++i) {
                interp.reg(static_cast<std::int64_t>(i)) = (*row)[i];
            }
            vm::run_status status = interp.run();
            if (status == vm::run_status::error) {
                throw query_error("runtime error evaluating a row");
            }
            if (status != vm::run_status::row) continue; // WHERE was false

            collected_row out;
            for (std::int64_t i = 0; i < interp.row_count(); ++i) {
                out.values.push_back(interp.reg(interp.row_start() + i));
            }
            for (std::int64_t r : order_regs) out.order_keys.push_back(interp.reg(r));
            collected.push_back(std::move(out));
        }

        if (stmt.distinct) {
            std::vector<collected_row> deduped;
            for (auto& row : collected) {
                bool seen = std::any_of(deduped.begin(), deduped.end(),
                                         [&](const collected_row& other) { return other.values == row.values; });
                if (!seen) deduped.push_back(std::move(row));
            }
            collected = std::move(deduped);
        }

        if (!stmt.order_by.empty()) {
            std::stable_sort(collected.begin(), collected.end(), [&](const collected_row& a, const collected_row& b) {
                for (std::size_t i = 0; i < stmt.order_by.size(); ++i) {
                    int cmp = vm::aux::compare(a.order_keys[i], b.order_keys[i]);
                    if (cmp == 0) continue;
                    return stmt.order_by[i].descending ? cmp > 0 : cmp < 0;
                }
                return false;
            });
        }

        std::size_t offset = stmt.offset ? evaluate_scalar_count(*stmt.offset) : 0;
        std::optional<std::size_t> limit = stmt.limit ? std::optional(evaluate_scalar_count(*stmt.limit)) : std::nullopt;

        query_result result;
        result.column_names = std::move(output_names);
        std::size_t begin = std::min(offset, collected.size());
        std::size_t end = limit ? std::min(collected.size(), begin + *limit) : collected.size();
        for (std::size_t i = begin; i < end; ++i) result.rows.push_back(std::move(collected[i].values));
        return result;
    }

    // Evaluates a constant(ish) expression once (LIMIT/OFFSET), independent
    // of any row -- these never reference table columns.
    static std::size_t evaluate_scalar_count(const ast::expr& e) {
        vm::aux::program_builder builder;
        cg::code_generator gen(builder, 0, [](std::string_view, std::string_view column) -> std::int64_t {
            throw query_error("LIMIT/OFFSET cannot reference columns: " + std::string(column));
        });
        std::int64_t r = gen.allocate_register();
        gen.compile(e, r);
        gen.finish();
        vm::interpreter interp(builder.finish());
        vm::run_status status = interp.run();
        if (status == vm::run_status::error) throw query_error("runtime error evaluating LIMIT/OFFSET");
        const vm::aux::mem& value = interp.reg(r);
        if (value.is_null()) return 0;
        std::int64_t n = value.as_integer();
        return n < 0 ? 0 : static_cast<std::size_t>(n);
    }
};

} // namespace sqlite::core::command_processor
