# sqlite-compiler-code-generator

`sqlite::compiler::code_generator` -- compiles `sqlite-compiler-parser`'s
expression AST into `sqlite-core-virtual-machine` bytecode. This is the
seam [SRS 001](../../../srs/001-sqlite-cpp-modularization.md) S11.2 names explicitly as the refactor's
highest-risk piece: legacy interleaves "decide what to do" and "emit the
opcodes to do it" statement-by-statement in the same functions
(`select.c`/`expr.c`/`where.c`), and separating them cleanly needs a real
intermediate representation, not a mechanical file move. That IR is exactly
what already exists at this point in the workspace: `ast::expr` (built by
`sqlite-compiler-parser`, with no knowledge of any target machine) and
`aux::program_builder`/`opcode` (built by `sqlite-core-virtual-machine`,
with no knowledge of any AST source). This library is the first thing in
the workspace that links both, and its entire job is walking one to drive
the other.

## Position in the dependency graph

```
sqlite-compiler-parser        sqlite-core-virtual-machine
              \                      /
               sqlite-compiler-code-generator     <- you are here
                          |
               sqlite-core-command-processor       (not yet built)
```

This matches SRS S3.1's stated dependency exactly: "`sqlite-compiler-code-generator`
depends on `sqlite-core-virtual-machine` (it emits VDBE opcodes) and on
`sqlite-compiler-parser`'s AST types."

## Scope: expressions only, agreed before starting

Full SELECT-statement compilation needs table-access opcodes (`OP_OpenRead`,
`OP_Column`, `OP_Rewind`/`OP_Next`, ...), cursors, and schema information --
none of which exist yet (`sqlite-backend-tree` and
`sqlite-core-command-processor`, both unbuilt). This pass compiles
*expressions*: `code_generator::compile(const ast::expr&, target_register)`
walks the tree and emits instructions via `program_builder`, writing the
result into a caller-chosen register. Column references and bind variables
resolve through caller-supplied callbacks (`column_resolver`/
`variable_resolver`) -- a deliberate stand-in for the schema/cursor binding
a real `Parse` context will eventually provide.

## The comparison/boolean-value technique, and why it's not a trivial mapping

This is the one piece of this library that isn't a direct AST-node-to-opcode
lookup, and it's worth calling out because it's a real (if small) instance
of the "decide vs. emit" tension S11.2 describes: `sqlite-core-virtual-machine`'s
comparison opcodes (`eq`/`ne`/`lt`/`le`/`gt`/`ge`) are *conditional jumps*
("if `r[p1] <op> r[p2]` then goto `p3`"), because that's what a WHERE clause
needs. But `a = b` used as a *value* (e.g. `SELECT a = b`) needs an actual
0/1 result in a register, not a jump. `compile_comparison()` bridges this by
emitting the jump, then a fall-through `target = 0`, an unconditional skip,
and a `target = 1` at the jump's landing address -- the same "evaluate,
branch, load 1 or 0" shape legacy's `expr.c` uses for a boolean-context
comparison outside of an optimizable jump position. `AND`/`OR`/`NOT` are
built the same way, on top of the VM's `if_`/`if_not`, with real
short-circuit evaluation (the right operand's *code* is always emitted, but
a false left/true-for-OR operand jumps around actually *executing* it).

## What's deferred, and why

Every `ast::expr_kind`/operator with no corresponding VM opcode throws
`code_generator_error` (`testing/code_generator_test.hpp` checks each one
explicitly) rather than silently miscompiling:

- **Bitwise operators** (`& | << >>`, unary `~`) -- `sqlite-core-virtual-machine`
  has no bitwise opcodes yet (SRS S3.4.1's phase-3 scope note: its opcode
  set covers arithmetic/comparison/control-flow/row-output only).
- **`IS`/`IS NOT`, `IS NULL`/`ISNULL`/`NOTNULL`** -- no VM opcode for a
  NULL-aware equality/identity test yet.
- **`IN (...)`, `BETWEEN`, `LIKE`/`GLOB`/`REGEXP`/`MATCH`** -- each needs
  either a loop-and-compare construct (`IN`) or a pattern-matching primitive
  (`LIKE`/`GLOB`) the VM doesn't have.
- **`CASE`** -- compilable in principle as a chain of conditional jumps
  (much like `compile_comparison()`'s technique, generalized), just not
  implemented this pass.
- **Function calls** -- the VM has no notion of a function registry or a
  call opcode yet; that belongs with whichever phase adds
  `sqlite::core::interface`'s `sqlite3_create_function` equivalent.
- **Blob literals** -- `sqlite-core-virtual-machine`'s opcode set never
  constructs a blob-typed `mem` (only `aux::mem` itself supports the blob
  variant; no opcode produces one).

## Building and running tests independently

```sh
cmake -S libraries/libsqlite-compiler-code-generator -B build-codegen
cmake --build build-codegen
ctest --test-dir build-codegen
```

Or, as part of the workspace build: `ctest --test-dir build -R sqlite-compiler-code-generator`.

Tests compile real SQL expression text through the parser, run the emitted
program through `sqlite::core::virtual_machine::interpreter`, and assert on
the actual runtime result (`testing/eval_helper.hpp`) -- not just on the
shape of the emitted instructions.

## Consuming this library

```cmake
include(cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_compiler_code_generator()
target_link_libraries(your_target PRIVATE sqlite::compiler::code_generator)
```
