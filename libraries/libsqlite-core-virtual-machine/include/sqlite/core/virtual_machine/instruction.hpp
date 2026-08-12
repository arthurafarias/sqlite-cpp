#pragma once

#include "opcode.hpp"

#include <cstdint>
#include <string>

// Legacy source: the `VdbeOp` struct in vdbeInt.h/sqlite3.h (`opcode` +
// `p1`/`p2`/`p3`/`p4`/`p5` operands; SRS S3.4.1). p4 is legacy's
// tagged-union "whatever extra operand this opcode needs" slot (a string, an
// int64, a function pointer, a sub-schema pointer, ...); this port narrows
// it to the two concrete payloads this phase's opcode subset actually uses
// (a double for `real`, a string for `string`), as separate named fields
// rather than a variant, since there is never more than one in play for a
// given opcode and named fields read more clearly at the call site than
// `std::get`.
namespace sqlite::core::virtual_machine {

struct instruction {
    opcode op;
    std::int64_t p1 = 0;
    std::int64_t p2 = 0;
    std::int64_t p3 = 0;
    double real_operand = 0.0;
    std::string text_operand;
};

} // namespace sqlite::core::virtual_machine
