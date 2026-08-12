#pragma once

#include "instruction.hpp"

#include <cstdint>
#include <vector>

// Legacy source: the array of VdbeOp the `Vdbe` object owns
// (Vdbe.aOp/Vdbe.nOp) once sqlite3VdbeMakeReady() has finished with it (SRS
// S3.4.1). A fully-built, ready-to-execute instruction sequence plus the
// register-file size it needs -- what aux::program_builder produces and
// what the interpreter (interpreter.hpp) runs.
namespace sqlite::core::virtual_machine {

struct program {
    std::vector<instruction> instructions;
    std::int64_t register_count = 0;
};

} // namespace sqlite::core::virtual_machine
