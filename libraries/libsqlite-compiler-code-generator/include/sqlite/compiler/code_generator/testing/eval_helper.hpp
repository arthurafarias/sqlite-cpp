#pragma once

#include "../code_generator.hpp"

#include <sqlite/compiler/parser/parser.hpp>
#include <sqlite/core/virtual_machine/interpreter.hpp>

#include <map>
#include <stdexcept>
#include <string>

// Test-only support (not a legacy port): parses a SQL expression, compiles
// it with code_generator, actually runs the resulting program through
// sqlite::core::virtual_machine::interpreter, and returns the resulting
// mem -- so tests assert against real interpreter output, not just against
// the shape of the emitted instructions.
namespace sqlite::compiler::code_generator::testing {

namespace vm = sqlite::core::virtual_machine;

inline vm::aux::mem eval(std::string_view expr_sql,
                          const std::map<std::string, vm::aux::mem>& columns = {}) {
    sqlite::compiler::parser::parser p(expr_sql);
    auto e = p.parse_expr();

    std::map<std::string, std::int64_t> reg_of;
    std::int64_t next_input_register = 0;
    for (const auto& [name, value] : columns) {
        (void)value;
        reg_of[name] = next_input_register++;
    }

    vm::aux::program_builder builder;
    code_generator gen(builder, next_input_register,
        [&](std::string_view, std::string_view column) -> std::int64_t {
            return reg_of.at(std::string(column));
        });
    std::int64_t result_reg = gen.allocate_register();
    gen.compile(*e, result_reg);
    gen.finish();
    builder.add_op(vm::opcode::result_row, result_reg, 1);
    builder.add_op(vm::opcode::halt, 0);

    vm::interpreter interp(builder.finish());
    for (const auto& [name, value] : columns) interp.reg(reg_of.at(name)) = value;

    if (interp.run() != vm::run_status::row) {
        throw std::runtime_error("eval(): expected the compiled program to yield a row");
    }
    return interp.reg(interp.row_start());
}

} // namespace sqlite::compiler::code_generator::testing
