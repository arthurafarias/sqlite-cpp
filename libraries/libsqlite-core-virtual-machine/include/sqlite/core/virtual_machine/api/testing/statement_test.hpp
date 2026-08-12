#pragma once

#include "../../aux/program_builder.hpp"
#include "../statement.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::core::virtual_machine::api::testing {

using sqlite::utils::testing::test_group;
using aux::program_builder;

namespace {
// Builds "SELECT ?1 + ?2" as a hand-assembled program: r0/r1 are bound
// inputs, r2 is the computed sum, yielded as the one output column.
program make_add_program() {
    program_builder b;
    b.add_op(opcode::add, 0, 1, 2);
    b.add_op(opcode::result_row, 2, 1);
    b.add_op(opcode::halt, 0);
    b.use_registers(3);
    return b.finish();
}
} // namespace

inline const test_group statement_tests("statement", {
    {"bind then step then read a column, mirroring sqlite3_step/sqlite3_column_*", [](auto& ctx) {
        statement stmt(make_add_program());
        stmt.bind_int64(0, 4);
        stmt.bind_int64(1, 5);

        ctx.check(stmt.step() == step_result::row);
        ctx.equal(stmt.column_count(), std::int64_t{1});
        ctx.check(stmt.column_type(0) == aux::value_type::integer);
        ctx.check(!stmt.column_is_null(0));
        ctx.equal(stmt.column_int64(0), std::int64_t{9});

        ctx.check(stmt.step() == step_result::done);
        ctx.equal(stmt.result_code(), std::int64_t{0});
    }},
    {"column_double and column_text coerce the same way mem does", [](auto& ctx) {
        statement stmt(make_add_program());
        stmt.bind_double(0, 1.5);
        stmt.bind_double(1, 2.5);
        ctx.check(stmt.step() == step_result::row);
        ctx.equal(stmt.column_double(0), 4.0);
        ctx.equal(stmt.column_text(0), std::string{"4"});
    }},
    {"reset lets a statement be bound and stepped again", [](auto& ctx) {
        statement stmt(make_add_program());
        stmt.bind_int64(0, 1);
        stmt.bind_int64(1, 1);
        ctx.check(stmt.step() == step_result::row);
        ctx.equal(stmt.column_int64(0), std::int64_t{2});
        ctx.check(stmt.step() == step_result::done);

        stmt.reset();
        stmt.bind_int64(0, 10);
        stmt.bind_int64(1, 20);
        ctx.check(stmt.step() == step_result::row);
        ctx.equal(stmt.column_int64(0), std::int64_t{30});
    }},
    {"a bound NULL propagates through to a NULL column, matching mem's arithmetic rule", [](auto& ctx) {
        statement stmt(make_add_program());
        stmt.bind_null(0);
        stmt.bind_int64(1, 5);
        ctx.check(stmt.step() == step_result::row);
        ctx.check(stmt.column_is_null(0));
    }},
});

} // namespace sqlite::core::virtual_machine::api::testing
