#pragma once

#include "../aux/program_builder.hpp"
#include "../interpreter.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::core::virtual_machine::testing {

using sqlite::utils::testing::test_group;
using aux::program_builder;

inline const test_group interpreter_tests("interpreter", {
    {"a hand-assembled program computes 2+3 and yields one row", [](auto& ctx) {
        program_builder b;
        b.add_op(opcode::integer, 2, 0);           // r0 = 2
        b.add_op(opcode::integer, 3, 1);            // r1 = 3
        b.add_op(opcode::add, 0, 1, 2);              // r2 = r0 + r1
        b.add_op(opcode::result_row, 2, 1);           // yield r2
        b.add_op(opcode::halt, 0);
        b.use_registers(3);
        program p = b.finish();

        interpreter vm(p);
        ctx.check(vm.run() == run_status::row);
        ctx.equal(vm.reg(vm.row_start()).as_integer(), std::int64_t{5});
        ctx.check(vm.run() == run_status::done);
        ctx.equal(vm.result_code(), std::int64_t{0});
    }},
    {"arithmetic propagates NULL", [](auto& ctx) {
        program_builder b;
        b.add_op(opcode::null_, 0, 0);
        b.add_op(opcode::integer, 5, 1);
        b.add_op(opcode::add, 0, 1, 2);
        b.add_op(opcode::result_row, 2, 1);
        b.add_op(opcode::halt, 0);
        b.use_registers(3);
        interpreter vm(b.finish());

        ctx.check(vm.run() == run_status::row);
        ctx.check(vm.reg(vm.row_start()).is_null());
    }},
    {"division and remainder by zero yield NULL rather than crashing", [](auto& ctx) {
        program_builder b;
        b.add_op(opcode::integer, 10, 0);
        b.add_op(opcode::integer, 0, 1);
        b.add_op(opcode::divide, 0, 1, 2);
        b.add_op(opcode::remainder, 0, 1, 3);
        b.add_op(opcode::result_row, 2, 2);
        b.add_op(opcode::halt, 0);
        b.use_registers(4);
        interpreter vm(b.finish());

        ctx.check(vm.run() == run_status::row);
        ctx.check(vm.reg(2).is_null());
        ctx.check(vm.reg(3).is_null());
    }},
    {"integer arithmetic stays integer; mixed/text operands promote to real", [](auto& ctx) {
        program_builder b;
        b.add_op(opcode::integer, 7, 0);
        b.add_op(opcode::integer, 2, 1);
        b.add_op(opcode::divide, 0, 1, 2);          // integer division: 7/2 = 3
        b.add_op_real(opcode::real, 3, 2.0);
        b.add_op(opcode::divide, 0, 3, 4);            // real-tainted: 7/2.0 = 3.5
        b.add_op(opcode::result_row, 2, 3);
        b.add_op(opcode::halt, 0);
        b.use_registers(5);
        interpreter vm(b.finish());

        ctx.check(vm.run() == run_status::row);
        ctx.check(vm.reg(2).type() == aux::value_type::integer);
        ctx.equal(vm.reg(2).as_integer(), std::int64_t{3});
        ctx.check(vm.reg(4).type() == aux::value_type::real);
        ctx.equal(vm.reg(4).as_real(), 3.5);
    }},
    {"concat joins text and propagates NULL", [](auto& ctx) {
        program_builder b;
        b.add_op_string(opcode::string, 0, "foo");
        b.add_op_string(opcode::string, 1, "bar");
        b.add_op(opcode::concat, 0, 1, 2);
        b.add_op(opcode::null_, 3);
        b.add_op(opcode::concat, 0, 3, 4);
        b.add_op(opcode::result_row, 2, 3);
        b.add_op(opcode::halt, 0);
        b.use_registers(5);
        interpreter vm(b.finish());

        ctx.check(vm.run() == run_status::row);
        ctx.equal(vm.reg(2).as_text(), std::string{"foobar"});
        ctx.check(vm.reg(4).is_null());
    }},
    {"comparison opcodes jump exactly when the comparison holds, and never on a NULL operand", [](auto& ctx) {
        program_builder b;
        b.add_op(opcode::integer, 1, 0);
        b.add_op(opcode::integer, 2, 1);
        auto jmp_taken = b.add_op(opcode::lt, 0, 1, -1);    // 1 < 2: jump taken
        b.add_op(opcode::integer, 0, 2);                     // skipped if jump taken
        b.set_p3(jmp_taken, b.next_address());
        b.add_op(opcode::integer, 1, 2);                     // landing point

        b.add_op(opcode::null_, 3);
        auto jmp_null = b.add_op(opcode::eq, 3, 0, -1);       // NULL == 1: never jumps
        b.set_p3(jmp_null, b.next_address() + 1);
        b.add_op(opcode::integer, 42, 4);                     // executed, since the NULL comparison didn't jump

        b.add_op(opcode::result_row, 2, 3);
        b.add_op(opcode::halt, 0);
        b.use_registers(5);
        interpreter vm(b.finish());

        ctx.check(vm.run() == run_status::row);
        ctx.equal(vm.reg(2).as_integer(), std::int64_t{1});
        ctx.equal(vm.reg(4).as_integer(), std::int64_t{42});
    }},
    {"a loop with goto/if_not sums 1..5 and yields multiple rows before halting", [](auto& ctx) {
        // r0 = counter (1..5), r1 = limit (5), r2 = sum, r3 = one, r4 = cond
        program_builder b;
        b.add_op(opcode::integer, 1, 0);
        b.add_op(opcode::integer, 5, 1);
        b.add_op(opcode::integer, 0, 2);
        b.add_op(opcode::integer, 1, 3);

        auto loop_start = b.next_address();
        auto exit_jump = b.add_op(opcode::gt, 0, 1, -1);       // if counter > limit, exit
        b.add_op(opcode::add, 2, 0, 2);                          // sum += counter
        b.add_op(opcode::result_row, 2, 1);                       // yield running sum
        b.add_op(opcode::add, 0, 3, 0);                            // counter += 1
        b.add_op(opcode::goto_, 0, loop_start);
        b.set_p3(exit_jump, b.next_address());
        b.add_op(opcode::halt, 0);
        b.use_registers(5);
        interpreter vm(b.finish());

        std::vector<std::int64_t> rows;
        while (vm.run() == run_status::row) rows.push_back(vm.reg(vm.row_start()).as_integer());
        ctx.equal(vm.result_code(), std::int64_t{0});

        std::vector<std::int64_t> expected{1, 3, 6, 10, 15};
        ctx.check(rows == expected);
    }},
    {"halt with a nonzero p1 reports an error status", [](auto& ctx) {
        program_builder b;
        b.add_op(opcode::halt, 7);
        interpreter vm(b.finish());
        ctx.check(vm.run() == run_status::error);
        ctx.equal(vm.result_code(), std::int64_t{7});
    }},
    {"reset() rewinds the program counter so it can be run again", [](auto& ctx) {
        program_builder b;
        b.add_op(opcode::integer, 9, 0);
        b.add_op(opcode::result_row, 0, 1);
        b.add_op(opcode::halt, 0);
        b.use_registers(1);
        interpreter vm(b.finish());

        ctx.check(vm.run() == run_status::row);
        ctx.check(vm.run() == run_status::done);
        vm.reset();
        ctx.check(vm.run() == run_status::row);
        ctx.equal(vm.reg(0).as_integer(), std::int64_t{9});
    }},
});

} // namespace sqlite::core::virtual_machine::testing
