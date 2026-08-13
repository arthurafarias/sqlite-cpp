#pragma once

#include <stdexcept>
#include <string>

// Legacy source: sqlite3ErrorMsg()'s use inside the parser (the LEMON-driven
// sqlite3RunParser() reports a syntax error through the same Parse::rc/zErrMsg
// fields every other compile-time error uses; SRS S3.5.2). This port reports
// a parse failure by throwing rather than setting an out-parameter/error
// code, since there's no `Parse` context object yet to hold one (that
// belongs to a later phase's sqlite-core-command-processor).
namespace sqlite::compiler::parser {

struct parse_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace sqlite::compiler::parser
