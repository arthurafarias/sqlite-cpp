#pragma once

#include <stdexcept>

// Legacy source: sqlite3ErrorMsg()'s use inside expr.c's code-emission
// routines (SRS S3.5.3). Thrown rather than set through a Parse-context
// error field, for the same reason sqlite::compiler::parser::parse_error is
// (SRS S3.5.2's parse_error.hpp): there is no Parse object yet.
namespace sqlite::compiler::code_generator {

struct code_generator_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace sqlite::compiler::code_generator
