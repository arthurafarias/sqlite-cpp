#pragma once

#include "../aux/mem.hpp"
#include "../interpreter.hpp"
#include "../program.hpp"

#include <cstdint>
#include <string>
#include <utility>

// Legacy source: vdbeapi.c (sqlite3_step/sqlite3_bind_*/sqlite3_column_*;
// SRS S3.4.1, sqlite::core::virtual_machine::api). The public "run a
// prepared program, bind inputs, read output rows" surface, layered over
// interpreter.hpp the same way vdbeapi.c layers over vdbe.c's
// sqlite3VdbeExec().
//
// Deliberate simplification vs. legacy: sqlite3_bind_*() writes into a
// distinct parameter array (Vdbe.aVar), separate from the register file
// (Vdbe.aMem) that sqlite3_reset() rewinds computation state in but leaves
// bindings untouched -- that's why real SQLite lets you reset() and step()
// a statement again without re-binding. This port has no separate parameter
// array (registers double as both binding storage and scratch space, same
// as this phase's hand-assembled programs use them for everything else), so
// reset() here clears bound values too; a caller must rebind after reset().
// A distinct binding array is a reasonable addition for whichever future
// pass wires this VM to real bound-parameter SQL syntax.
namespace sqlite::core::virtual_machine::api {

enum class step_result {
    row,
    done,
    error,
};

class statement {
public:
    explicit statement(program prog) : interpreter_(std::move(prog)) {}

    void bind_null(std::int64_t reg) { interpreter_.reg(reg) = aux::mem::make_null(); }
    void bind_int64(std::int64_t reg, std::int64_t v) { interpreter_.reg(reg) = aux::mem::make_integer(v); }
    void bind_double(std::int64_t reg, double v) { interpreter_.reg(reg) = aux::mem::make_real(v); }
    void bind_text(std::int64_t reg, std::string v) { interpreter_.reg(reg) = aux::mem::make_text(std::move(v)); }

    step_result step() {
        switch (interpreter_.run()) {
            case run_status::row: return step_result::row;
            case run_status::done: return step_result::done;
            default: return step_result::error;
        }
    }

    [[nodiscard]] std::int64_t column_count() const noexcept { return interpreter_.row_count(); }

    [[nodiscard]] const aux::mem& column(std::int64_t i) const {
        return interpreter_.reg(interpreter_.row_start() + i);
    }
    [[nodiscard]] aux::value_type column_type(std::int64_t i) const { return column(i).type(); }
    [[nodiscard]] bool column_is_null(std::int64_t i) const { return column(i).is_null(); }
    [[nodiscard]] std::int64_t column_int64(std::int64_t i) const { return column(i).as_integer(); }
    [[nodiscard]] double column_double(std::int64_t i) const { return column(i).as_real(); }
    [[nodiscard]] std::string column_text(std::int64_t i) const { return column(i).as_text(); }

    [[nodiscard]] std::int64_t result_code() const noexcept { return interpreter_.result_code(); }

    void reset() { interpreter_.reset(); }

private:
    interpreter interpreter_;
};

} // namespace sqlite::core::virtual_machine::api
