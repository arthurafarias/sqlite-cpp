#pragma once

#include "code_generator_error.hpp"

#include <sqlite/compiler/parser/ast/expr.hpp>
#include <sqlite/core/virtual_machine/aux/program_builder.hpp>
#include <sqlite/core/virtual_machine/opcode.hpp>

#include <charconv>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

// Legacy source: expr.c's sqlite3ExprCodeTarget()/sqlite3ExprCode() -- the
// AST-to-bytecode walk (SRS S3.5.3, sqlite::compiler::code_generator). This
// is the seam SRS S11.2 calls out as the highest-risk part of the whole
// refactor: legacy interleaves "decide what to do" and "emit the opcodes to
// do it" statement-by-statement in the same functions, and separating them
// cleanly requires exactly the kind of intermediate representation this
// library is. Concretely: sqlite::compiler::parser produces an
// ast::expr tree (the "decide" side, already built, independent of any
// target machine); this library walks that tree and drives
// sqlite::core::virtual_machine::aux::program_builder (the "emit" side,
// likewise already built and independent of any particular AST source) to
// produce a runnable program::instructions sequence. Neither of those two
// libraries knows the other exists; this is the only library in the
// workspace so far that links both.
//
// Scope of this pass (agreed before starting -- see this library's docs):
// compiles *expressions* to bytecode, using only the opcode set
// sqlite-core-virtual-machine currently implements. Column references and
// bind variables resolve through caller-supplied callbacks (a stand-in for
// the schema/cursor binding a real Parse context would provide -- that
// belongs to a not-yet-built sqlite-core-command-processor). AST node kinds
// with no corresponding opcode yet (CASE, IN, BETWEEN, LIKE, function
// calls, bitwise operators, IS/IS NOT, blob literals) throw
// code_generator_error rather than silently miscompiling -- see the docs
// for the full list and why each is deferred rather than compiled.
namespace sqlite::compiler::code_generator {

namespace vm = sqlite::core::virtual_machine;
namespace ast = sqlite::compiler::parser::ast;

class code_generator {
public:
    using column_resolver = std::function<std::int64_t(std::string_view table_qualifier, std::string_view column)>;
    using variable_resolver = std::function<std::int64_t(std::string_view name)>;

    code_generator(vm::aux::program_builder& builder, std::int64_t first_scratch_register,
                   column_resolver resolve_column, variable_resolver resolve_variable = {})
        : builder_(builder), next_register_(first_scratch_register),
          resolve_column_(std::move(resolve_column)), resolve_variable_(std::move(resolve_variable)) {}

    // sqlite3ExprCodeTarget(): compiles `e`, writing its value into register
    // `target`, and returns `target`. Every AST node this pass supports
    // ends up written to exactly the register the caller asked for (legacy
    // sometimes returns a different, already-live register as an
    // optimization; this port always honors `target`, trading a few
    // avoidable OP_Copy's for a simpler contract).
    std::int64_t compile(const ast::expr& e, std::int64_t target) {
        switch (e.kind) {
            case ast::expr_kind::integer_literal:
                emit_integer(target, parse_integer_literal(e.text));
                return target;
            case ast::expr_kind::float_literal:
                emit_real(target, parse_real_literal(e.text));
                return target;
            case ast::expr_kind::string_literal:
                builder_.add_op_string(vm::opcode::string, target, e.text);
                return target;
            case ast::expr_kind::null_literal:
                builder_.add_op(vm::opcode::null_, 0, target);
                return target;
            case ast::expr_kind::variable:
                return compile_variable(e, target);
            case ast::expr_kind::column_ref:
                return compile_column_ref(e, target);
            case ast::expr_kind::unary:
                return compile_unary(e, target);
            case ast::expr_kind::binary:
                return compile_binary(e, target);
            case ast::expr_kind::blob_literal:
                throw code_generator_error("blob literals have no VM opcode yet");
            case ast::expr_kind::is_null_check:
                throw code_generator_error("IS [NOT] NULL has no VM opcode yet");
            case ast::expr_kind::in_list:
                throw code_generator_error("IN (...) has no VM opcode yet");
            case ast::expr_kind::between:
                throw code_generator_error("BETWEEN has no VM opcode yet");
            case ast::expr_kind::like:
                throw code_generator_error("LIKE/GLOB/REGEXP/MATCH have no VM opcode yet");
            case ast::expr_kind::case_expr:
                throw code_generator_error("CASE has no VM opcode yet");
            case ast::expr_kind::function_call:
                throw code_generator_error("function calls have no VM opcode yet");
        }
        throw code_generator_error("unrecognized expression kind");
    }

    // Allocates a fresh scratch register above every register used so far
    // (by this compile() call or any previous one on this generator).
    std::int64_t allocate_register() { return next_register_++; }

    // sqlite3VdbeMakeReady()'s register-count bookkeeping: call once, after
    // the last compile(), before builder.finish().
    void finish() { builder_.use_registers(next_register_); }

private:
    void emit_integer(std::int64_t target, std::int64_t value) {
        builder_.add_op(vm::opcode::integer, value, target);
    }
    void emit_real(std::int64_t target, double value) {
        builder_.add_op_real(vm::opcode::real, target, value);
    }

    std::int64_t compile_variable(const ast::expr& e, std::int64_t target) {
        if (!resolve_variable_) throw code_generator_error("no variable resolver was supplied");
        std::int64_t source = resolve_variable_(e.text);
        builder_.add_op(vm::opcode::copy, source, target);
        return target;
    }

    std::int64_t compile_column_ref(const ast::expr& e, std::int64_t target) {
        std::int64_t source = resolve_column_(e.table_qualifier, e.text);
        builder_.add_op(vm::opcode::copy, source, target);
        return target;
    }

    std::int64_t compile_unary(const ast::expr& e, std::int64_t target) {
        switch (e.u_op) {
            case ast::unary_op::plus:
                return compile(*e.left, target);
            case ast::unary_op::negate: {
                std::int64_t zero = allocate_register();
                emit_integer(zero, 0);
                std::int64_t operand = allocate_register();
                compile(*e.left, operand);
                builder_.add_op(vm::opcode::subtract, zero, operand, target);
                return target;
            }
            case ast::unary_op::not_:
                return compile_boolean_of_truthiness(*e.left, target, /*want_true_when_operand_is=*/false);
            case ast::unary_op::bit_not:
                throw code_generator_error("bitwise NOT has no VM opcode yet");
        }
        throw code_generator_error("unrecognized unary operator");
    }

    std::int64_t compile_binary(const ast::expr& e, std::int64_t target) {
        switch (e.b_op) {
            case ast::binary_op::add: return compile_arithmetic(vm::opcode::add, e, target);
            case ast::binary_op::subtract: return compile_arithmetic(vm::opcode::subtract, e, target);
            case ast::binary_op::multiply: return compile_arithmetic(vm::opcode::multiply, e, target);
            case ast::binary_op::divide: return compile_arithmetic(vm::opcode::divide, e, target);
            case ast::binary_op::modulo: return compile_arithmetic(vm::opcode::remainder, e, target);
            case ast::binary_op::concat: return compile_arithmetic(vm::opcode::concat, e, target);
            case ast::binary_op::eq: return compile_comparison(vm::opcode::eq, e, target);
            case ast::binary_op::ne: return compile_comparison(vm::opcode::ne, e, target);
            case ast::binary_op::lt: return compile_comparison(vm::opcode::lt, e, target);
            case ast::binary_op::le: return compile_comparison(vm::opcode::le, e, target);
            case ast::binary_op::gt: return compile_comparison(vm::opcode::gt, e, target);
            case ast::binary_op::ge: return compile_comparison(vm::opcode::ge, e, target);
            case ast::binary_op::and_: return compile_and(e, target);
            case ast::binary_op::or_: return compile_or(e, target);
            case ast::binary_op::bit_and:
            case ast::binary_op::bit_or:
            case ast::binary_op::shift_left:
            case ast::binary_op::shift_right:
                throw code_generator_error("bitwise operators have no VM opcode yet");
            case ast::binary_op::is:
            case ast::binary_op::is_not:
                throw code_generator_error("IS/IS NOT have no VM opcode yet");
        }
        throw code_generator_error("unrecognized binary operator");
    }

    std::int64_t compile_arithmetic(vm::opcode op, const ast::expr& e, std::int64_t target) {
        std::int64_t left = allocate_register();
        compile(*e.left, left);
        std::int64_t right = allocate_register();
        compile(*e.right, right);
        builder_.add_op(op, left, right, target);
        return target;
    }

    // sqlite3ExprCodeTarget()'s handling of a comparison used as a *value*
    // (as opposed to a WHERE-clause jump target): the VM's comparison
    // opcodes are conditional jumps (SRS S3.4.1's interpreter.hpp), not
    // value producers, so evaluating "a = b" as an expression compiles to
    // "jump on the comparison, then load whichever of 1/0 that branch
    // means" -- the same technique legacy uses for a boolean-context
    // comparison outside of an optimizable jump position.
    std::int64_t compile_comparison(vm::opcode cmp, const ast::expr& e, std::int64_t target) {
        std::int64_t left = allocate_register();
        compile(*e.left, left);
        std::int64_t right = allocate_register();
        compile(*e.right, right);

        std::int64_t jump_to_true = builder_.add_op(cmp, left, right, -1);
        emit_integer(target, 0);
        std::int64_t jump_to_end = builder_.add_op(vm::opcode::goto_, 0, -1);
        builder_.set_p3(jump_to_true, builder_.next_address());
        emit_integer(target, 1);
        builder_.set_p2(jump_to_end, builder_.next_address());
        return target;
    }

    // AND/OR/NOT, compiled as short-circuiting 0/1-valued expressions (the
    // same "evaluate, branch, load 1 or 0" shape compile_comparison() uses,
    // built on the VM's `if_`/`if_not` opcodes rather than a comparison
    // opcode).
    std::int64_t compile_and(const ast::expr& e, std::int64_t target) {
        std::int64_t left = allocate_register();
        compile(*e.left, left);
        std::int64_t short_circuit_false = builder_.add_op(vm::opcode::if_not, left, -1);
        std::int64_t right = allocate_register();
        compile(*e.right, right);
        std::int64_t short_circuit_false2 = builder_.add_op(vm::opcode::if_not, right, -1);
        emit_integer(target, 1);
        std::int64_t jump_to_end = builder_.add_op(vm::opcode::goto_, 0, -1);
        std::int64_t false_label = builder_.next_address();
        builder_.set_p2(short_circuit_false, false_label);
        builder_.set_p2(short_circuit_false2, false_label);
        emit_integer(target, 0);
        builder_.set_p2(jump_to_end, builder_.next_address());
        return target;
    }

    std::int64_t compile_or(const ast::expr& e, std::int64_t target) {
        std::int64_t left = allocate_register();
        compile(*e.left, left);
        std::int64_t short_circuit_true = builder_.add_op(vm::opcode::if_, left, -1);
        std::int64_t right = allocate_register();
        compile(*e.right, right);
        std::int64_t short_circuit_true2 = builder_.add_op(vm::opcode::if_, right, -1);
        emit_integer(target, 0);
        std::int64_t jump_to_end = builder_.add_op(vm::opcode::goto_, 0, -1);
        std::int64_t true_label = builder_.next_address();
        builder_.set_p2(short_circuit_true, true_label);
        builder_.set_p2(short_circuit_true2, true_label);
        emit_integer(target, 1);
        builder_.set_p2(jump_to_end, builder_.next_address());
        return target;
    }

    // Shared by unary NOT: `want_true_when_operand_is=false` means "result
    // is 1 exactly when the operand is falsy" -- i.e. logical negation.
    std::int64_t compile_boolean_of_truthiness(const ast::expr& operand, std::int64_t target, bool want_true_when_operand_is) {
        std::int64_t operand_reg = allocate_register();
        compile(operand, operand_reg);
        vm::opcode branch_if = want_true_when_operand_is ? vm::opcode::if_ : vm::opcode::if_not;
        std::int64_t to_true = builder_.add_op(branch_if, operand_reg, -1);
        emit_integer(target, 0);
        std::int64_t jump_to_end = builder_.add_op(vm::opcode::goto_, 0, -1);
        builder_.set_p2(to_true, builder_.next_address());
        emit_integer(target, 1);
        builder_.set_p2(jump_to_end, builder_.next_address());
        return target;
    }

    // sqlite3Atoi64()-equivalent for this phase: strips SQLite's '_'
    // digit-separator extension (SRS S3.5.1's tokenizer QNUMBER), then
    // parses as hex (0x-prefixed) or decimal.
    static std::int64_t parse_integer_literal(std::string_view text) {
        std::string digits;
        digits.reserve(text.size());
        for (char c : text) if (c != '_') digits.push_back(c);
        std::int64_t value = 0;
        if (digits.size() > 2 && digits[0] == '0' && (digits[1] == 'x' || digits[1] == 'X')) {
            std::from_chars(digits.data() + 2, digits.data() + digits.size(), value, 16);
        } else {
            auto dot = digits.find('.');
            if (dot != std::string::npos) {
                // A digit-separator'd fractional literal (SRS S3.5.2's
                // parser groups TK_QNUMBER under integer_literal
                // unconditionally); fall back to a real parse and truncate
                // rather than misreading the '.' as part of an int.
                return static_cast<std::int64_t>(parse_real_literal(digits));
            }
            std::from_chars(digits.data(), digits.data() + digits.size(), value, 10);
        }
        return value;
    }

    static double parse_real_literal(std::string_view text) {
        std::string digits;
        digits.reserve(text.size());
        for (char c : text) if (c != '_') digits.push_back(c);
        double value = 0;
        std::from_chars(digits.data(), digits.data() + digits.size(), value);
        return value;
    }

    vm::aux::program_builder& builder_;
    std::int64_t next_register_;
    column_resolver resolve_column_;
    variable_resolver resolve_variable_;
};

} // namespace sqlite::compiler::code_generator
