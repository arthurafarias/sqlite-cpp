#pragma once

#include "aux/mem.hpp"
#include "instruction.hpp"
#include "opcode.hpp"
#include "program.hpp"

#include <cstdint>
#include <utility>
#include <vector>

// Legacy source: vdbe.c's sqlite3VdbeExec() -- the opcode dispatch loop (SRS
// S3.4.1, sqlite::core::virtual_machine, "runtime only"). Ports the
// arithmetic/comparison/control-flow/row-output subset of opcodes this
// phase defines (opcode.hpp); see that header's comment for what's out of
// scope and why. sqlite3VdbeExec() is famously the hottest function in
// SQLite (SRS S11.1's whole point about not forcing real inlining on it
// applies directly here) -- this port keeps the same shape (one function, a
// switch over the opcode, a mutable register array, an explicit program
// counter) rather than e.g. a table of std::function opcode handlers, since
// indirect calls through std::function would defeat the branch predictor
// and add allocation overhead the legacy switch-based dispatch doesn't have.
namespace sqlite::core::virtual_machine {

enum class run_status {
    row,     // a row is available; read [row_start(), row_start()+row_count())
    done,    // OP_Halt reached; result_code() holds Vdbe's halt code
    error,   // a runtime error occurred (e.g. malformed program); result_code() holds a nonzero code
};

class interpreter {
public:
    // Owns its program by value (matching legacy's Vdbe, which likewise
    // owns its own aOp array rather than borrowing one) -- deliberately not
    // a `const program&`, since a caller building one inline
    // (`interpreter(builder.finish())`) would otherwise bind the reference
    // to a temporary that's destroyed at the end of that full expression,
    // leaving `this` holding a dangling pointer the moment run() is called.
    explicit interpreter(program prog)
        : program_(std::move(prog)), registers_(static_cast<std::size_t>(program_.register_count)) {}

    // sqlite3_step(): runs from the current program counter until the next
    // OP_ResultRow, OP_Halt, or error. Calling again after a `row` result
    // resumes right after the OP_ResultRow that produced it -- the same
    // "step, read row, step again" protocol sqlite3_step()/vdbeapi.c gives
    // callers.
    run_status run() {
        if (finished_) return status_;

        while (pc_ < static_cast<std::int64_t>(program_.instructions.size())) {
            const instruction& ins = program_.instructions[static_cast<std::size_t>(pc_)];
            switch (ins.op) {
                case opcode::goto_:
                    pc_ = ins.p2;
                    continue;

                case opcode::if_:
                    if (truthy(reg(ins.p1))) { pc_ = ins.p2; continue; }
                    break;

                case opcode::if_not:
                    if (!truthy(reg(ins.p1))) { pc_ = ins.p2; continue; }
                    break;

                case opcode::halt:
                    halt_code_ = ins.p1;
                    finished_ = true;
                    status_ = ins.p1 == 0 ? run_status::done : run_status::error;
                    return status_;

                case opcode::null_:
                    reg(ins.p2) = aux::mem::make_null();
                    break;

                case opcode::integer:
                    reg(ins.p2) = aux::mem::make_integer(ins.p1);
                    break;

                case opcode::real:
                    reg(ins.p2) = aux::mem::make_real(ins.real_operand);
                    break;

                case opcode::string:
                    reg(ins.p2) = aux::mem::make_text(ins.text_operand);
                    break;

                case opcode::copy:
                    reg(ins.p2) = reg(ins.p1);
                    break;

                case opcode::add: case opcode::subtract: case opcode::multiply:
                case opcode::divide: case opcode::remainder:
                    reg(ins.p3) = arithmetic(ins.op, reg(ins.p1), reg(ins.p2));
                    break;

                case opcode::concat: {
                    const aux::mem& a = reg(ins.p1);
                    const aux::mem& b = reg(ins.p2);
                    reg(ins.p3) = (a.is_null() || b.is_null())
                        ? aux::mem::make_null()
                        : aux::mem::make_text(a.as_text() + b.as_text());
                    break;
                }

                case opcode::eq: case opcode::ne: case opcode::lt:
                case opcode::le: case opcode::gt: case opcode::ge: {
                    const aux::mem& a = reg(ins.p1);
                    const aux::mem& b = reg(ins.p2);
                    if (!a.is_null() && !b.is_null() && compare_true(ins.op, aux::compare(a, b))) {
                        pc_ = ins.p3;
                        continue;
                    }
                    break;
                }

                case opcode::result_row:
                    row_start_ = ins.p1;
                    row_count_ = ins.p2;
                    ++pc_;
                    status_ = run_status::row;
                    return status_;
            }
            ++pc_;
        }

        finished_ = true;
        status_ = run_status::done;
        halt_code_ = 0;
        return status_;
    }

    [[nodiscard]] aux::mem& reg(std::int64_t i) { return registers_.at(static_cast<std::size_t>(i)); }
    [[nodiscard]] const aux::mem& reg(std::int64_t i) const { return registers_.at(static_cast<std::size_t>(i)); }

    [[nodiscard]] std::int64_t row_start() const noexcept { return row_start_; }
    [[nodiscard]] std::int64_t row_count() const noexcept { return row_count_; }
    [[nodiscard]] std::int64_t result_code() const noexcept { return halt_code_; }

    void reset() {
        pc_ = 0;
        finished_ = false;
        halt_code_ = 0;
        row_start_ = row_count_ = 0;
        for (auto& r : registers_) r = aux::mem::make_null();
    }

private:
    // truthy(): matches SQL's boolean-context coercion closely enough for
    // this phase's If/IfNot: NULL is never true (three-valued logic
    // collapses to "false" for branch purposes, the same way a NULL WHERE
    // clause result excludes a row rather than including it); a real
    // register is nonzero; anything else (integer, or text/blob coerced
    // through mem::as_integer()) is true iff its integer coercion is
    // nonzero.
    static bool truthy(const aux::mem& m) {
        if (m.is_null()) return false;
        if (m.type() == aux::value_type::real) return m.as_real() != 0.0;
        return m.as_integer() != 0;
    }

    static aux::mem arithmetic(opcode op, const aux::mem& a, const aux::mem& b) {
        if (a.is_null() || b.is_null()) return aux::mem::make_null();
        // Integer fast path only when both operands are *already* integers
        // (mirrors legacy's `(type1 & type2 & MEM_Int)` check) -- a numeric
        // string operand always goes through the floating-point path below,
        // same as legacy's fp_math fallback for anything that isn't a bare
        // MEM_Int on both sides.
        if (a.type() == aux::value_type::integer && b.type() == aux::value_type::integer) {
            std::int64_t x = a.as_integer();
            std::int64_t y = b.as_integer();
            switch (op) {
                case opcode::add: return aux::mem::make_integer(x + y);
                case opcode::subtract: return aux::mem::make_integer(x - y);
                case opcode::multiply: return aux::mem::make_integer(x * y);
                case opcode::divide:
                    return y == 0 ? aux::mem::make_null() : aux::mem::make_integer(x / y);
                case opcode::remainder:
                    return y == 0 ? aux::mem::make_null() : aux::mem::make_integer(x % y);
                default: break;
            }
        }
        double x = a.as_real();
        double y = b.as_real();
        switch (op) {
            case opcode::add: return aux::mem::make_real(x + y);
            case opcode::subtract: return aux::mem::make_real(x - y);
            case opcode::multiply: return aux::mem::make_real(x * y);
            case opcode::divide:
                return y == 0.0 ? aux::mem::make_null() : aux::mem::make_real(x / y);
            case opcode::remainder: {
                std::int64_t iy = static_cast<std::int64_t>(y);
                if (iy == 0) return aux::mem::make_null();
                return aux::mem::make_real(static_cast<double>(static_cast<std::int64_t>(x) % iy));
            }
            default: return aux::mem::make_null();
        }
    }

    static bool compare_true(opcode op, int cmp) {
        switch (op) {
            case opcode::eq: return cmp == 0;
            case opcode::ne: return cmp != 0;
            case opcode::lt: return cmp < 0;
            case opcode::le: return cmp <= 0;
            case opcode::gt: return cmp > 0;
            case opcode::ge: return cmp >= 0;
            default: return false;
        }
    }

    program program_;
    std::vector<aux::mem> registers_;
    std::int64_t pc_ = 0;
    bool finished_ = false;
    run_status status_ = run_status::done;
    std::int64_t halt_code_ = 0;
    std::int64_t row_start_ = 0;
    std::int64_t row_count_ = 0;
};

} // namespace sqlite::core::virtual_machine
