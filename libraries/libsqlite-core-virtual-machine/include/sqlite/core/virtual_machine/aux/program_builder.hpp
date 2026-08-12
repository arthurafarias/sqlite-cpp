#pragma once

#include "../instruction.hpp"
#include "../opcode.hpp"
#include "../program.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

// Legacy source: vdbeaux.c's program-construction API
// (sqlite3VdbeAddOp2/3/4, sqlite3VdbeChangeP2, sqlite3VdbeJumpHere,
// sqlite3VdbeMakeReady; SRS S3.4.1). In the legacy design this builder API
// is a dependency of sqlite-compiler-code-generator (SRS S3.4.1's header
// note on the vdbeaux.c/code-generator split, and S11.2): the code generator
// calls sqlite3VdbeAddOp* while walking the AST. Since sqlite-compiler
// doesn't exist yet in this phase, this builder's only caller for now is
// this library's own tests, hand-assembling programs directly -- the same
// role the real code generator will eventually take over.
namespace sqlite::core::virtual_machine::aux {

class program_builder {
public:
    std::int64_t add_op(opcode op, std::int64_t p1 = 0, std::int64_t p2 = 0, std::int64_t p3 = 0) {
        instructions_.push_back(instruction{op, p1, p2, p3, 0.0, {}});
        return next_index();
    }

    std::int64_t add_op_real(opcode op, std::int64_t p2, double value) {
        instructions_.push_back(instruction{op, 0, p2, 0, value, {}});
        return next_index();
    }

    std::int64_t add_op_string(opcode op, std::int64_t p2, std::string value) {
        instructions_.push_back(instruction{op, 0, p2, 0, 0.0, std::move(value)});
        return next_index();
    }

    // sqlite3VdbeCurrentAddr(): the address the *next* add_op() call will
    // land at -- what a forward jump should be patched to via set_p2/set_p3
    // once the code that follows has actually been emitted.
    [[nodiscard]] std::int64_t next_address() const noexcept {
        return static_cast<std::int64_t>(instructions_.size());
    }

    // sqlite3VdbeChangeP2()/sqlite3VdbeJumpHere(): patches a previously
    // emitted instruction's jump-target operand now that the real target
    // address is known (typically next_address() itself, for "jump to right
    // after this block").
    void set_p2(std::int64_t instruction_index, std::int64_t target) {
        instructions_.at(static_cast<std::size_t>(instruction_index)).p2 = target;
    }
    void set_p3(std::int64_t instruction_index, std::int64_t target) {
        instructions_.at(static_cast<std::size_t>(instruction_index)).p3 = target;
    }

    // sqlite3VdbeMakeReady()'s register-count bookkeeping (Vdbe.nMem):
    // records that a register index up to `count`-1 will be referenced, so
    // the interpreter allocates a large enough register file.
    void use_registers(std::int64_t count) {
        if (count > register_count_) register_count_ = count;
    }

    [[nodiscard]] program finish() const { return program{instructions_, register_count_}; }

private:
    [[nodiscard]] std::int64_t next_index() const noexcept {
        return static_cast<std::int64_t>(instructions_.size()) - 1;
    }

    std::vector<instruction> instructions_;
    std::int64_t register_count_ = 0;
};

} // namespace sqlite::core::virtual_machine::aux
