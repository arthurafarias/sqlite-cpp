#pragma once

// Legacy source: opcodes.h (generated from vdbe.c's `case OP_*:` labels by
// the `mkopcodeh.tcl`/mkopcodec tooling; SRS S3.4.1). FR-5: the generated
// `#define OP_Add 91`-style constants are replaced by a scoped enumeration.
//
// This is a real, working, but intentionally narrow subset of the ~185
// opcodes the legacy VDBE defines -- exactly the ones needed to build and
// run genuine hand-assembled programs (arithmetic, comparisons, control
// flow, row output) in this phase, per this library's docs on why full
// legacy-bytecode interop (and therefore the rest of the opcode set, most of
// which exists to drive B-tree cursors, triggers, and virtual tables that
// aren't built yet) is deferred to when sqlite-compiler exists.
namespace sqlite::core::virtual_machine {

enum class opcode {
    // Control flow.
    goto_,          // p2 = target instruction index
    if_,            // if truthy(r[p1]) goto p2; NULL is not truthy
    if_not,         // if !truthy(r[p1]) goto p2; NULL is not truthy either
    halt,           // stop execution; p1 = result code (0 = done normally)

    // Constant loads.
    null_,          // r[p2] = NULL
    integer,        // r[p2] = p1 (as a 64-bit integer)
    real,           // r[p2] = the double in the instruction's `real_operand`
    string,         // r[p2] = the string in the instruction's `text_operand`

    // Register movement.
    copy,           // r[p2] = r[p1]

    // Arithmetic: r[p3] = r[p1] <op> r[p2]. NULL propagates: any NULL
    // operand makes the result NULL. Note this p1/p2/p3 operand assignment
    // (left, right, destination) is chosen for this phase's hand-authored
    // programs and does NOT match legacy vdbe.c's p1/p2/p3 convention for
    // these opcodes -- see this library's docs for why matching it isn't
    // meaningful yet (nothing generates legacy-convention bytecode until
    // sqlite-compiler exists).
    add,
    subtract,
    multiply,
    divide,         // result is NULL (not an error) on division by zero, matching legacy
    remainder,
    concat,         // r[p3] = text(r[p1]) ++ text(r[p2])

    // Comparisons: if r[p1] <op> r[p2] then goto p3. Mirrors legacy's own
    // NULL handling: if either operand is NULL, the comparison is neither
    // true nor false and the jump is never taken (for op != not_equal) --
    // this is the same "NULL propagates to false-ish" rule SQL's WHERE
    // clause relies on.
    eq,
    ne,
    lt,
    le,
    gt,
    ge,

    // Row output.
    result_row,     // yield r[p1..p1+p2) as one output row; resumes after this instruction on the next step()
};

} // namespace sqlite::core::virtual_machine
