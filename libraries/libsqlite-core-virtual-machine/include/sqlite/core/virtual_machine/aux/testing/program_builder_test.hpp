#pragma once

#include "../program_builder.hpp"

#include <sqlite/utils/testing/test_group.hpp>

namespace sqlite::core::virtual_machine::aux::testing {

using sqlite::utils::testing::test_group;

inline const test_group program_builder_tests("program_builder", {
    {"add_op returns the index of the instruction just added", [](auto& ctx) {
        program_builder b;
        ctx.equal(b.add_op(opcode::integer, 1, 0), std::int64_t{0});
        ctx.equal(b.add_op(opcode::integer, 2, 1), std::int64_t{1});
        ctx.equal(b.next_address(), std::int64_t{2});
    }},
    {"set_p2/set_p3 patch a previously emitted instruction", [](auto& ctx) {
        program_builder b;
        auto jmp = b.add_op(opcode::goto_, 0, /*placeholder*/ -1);
        b.add_op(opcode::integer, 99, 0);
        b.set_p2(jmp, b.next_address());
        program p = b.finish();
        ctx.equal(p.instructions[static_cast<std::size_t>(jmp)].p2, b.next_address());
    }},
    {"finish() carries the instructions and the highest use_registers() count through", [](auto& ctx) {
        program_builder b;
        b.add_op(opcode::integer, 1, 0);
        b.use_registers(5);
        b.use_registers(3);
        program p = b.finish();
        ctx.equal(p.instructions.size(), std::size_t{1});
        ctx.equal(p.register_count, std::int64_t{5});
    }},
    {"add_op_real and add_op_string carry their payload through to the program", [](auto& ctx) {
        program_builder b;
        b.add_op_real(opcode::real, 0, 3.5);
        b.add_op_string(opcode::string, 1, "hello");
        program p = b.finish();
        ctx.equal(p.instructions[0].real_operand, 3.5);
        ctx.equal(p.instructions[1].text_operand, std::string{"hello"});
    }},
});

} // namespace sqlite::core::virtual_machine::aux::testing
