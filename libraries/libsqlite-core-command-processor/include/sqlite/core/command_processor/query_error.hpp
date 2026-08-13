#pragma once

#include <stdexcept>

// Legacy source: sqlite3ErrorMsg()'s use inside select.c/where.c's
// planning routines (SRS S3.4.2, sqlite::core::command_processor::query).
// Thrown rather than set through a Parse-context error field, for the same
// reason sqlite::compiler::code_generator::code_generator_error is: there
// is no Parse object yet. Also used for anything genuinely out of this
// pass's declared scope (joins, GROUP BY/HAVING, compound SELECT,
// subqueries, an unknown table/column) -- see this library's docs/index.md
// for the full list.
namespace sqlite::core::command_processor {

struct query_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace sqlite::core::command_processor
