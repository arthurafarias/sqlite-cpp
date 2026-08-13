# mkopcodeh

`sqlite::compiler::tooling::mkopcodeh` -- SRS 002 RR-5's complete C++
replacement for `applications/mkopcodeh-legacy/mkopcodeh.tcl`, a build-time
generator application (not a runtime library) that has no `sqlite3`
dependency and is therefore unblocked rather than facade-gated (SRS 002
S5.4.2) -- though its own work is deliberately sequenced alongside
`sqlite-core-virtual-machine`'s opcode enumeration, per that section.

## What it does

Reads the concatenation of `parse.h` (for `#define TK_x y`) and `vdbe.c`
(for `/* Opcode: ... */`/`** Synopsis: ...` comment pairs and `case OP_x:`
lines carrying attribute-keyword trailing comments) and emits `opcodes.h`:
numeric `#define OP_x n` assignments, the `OPFLG_*` bit constants and
`OPFLG_INITIALIZER` bitvector table, and `SQLITE_MX_JUMP_OPCODE`.

Opcode numbering is **not** simply "in the order opcodes appear" -- it
follows the legacy tool's exact four-pass priority order, which this port
preserves exactly rather than approximating:

1. The nine hardcoded `resolveP2Values()`-special-cased opcodes
   (`OP_Transaction`, `OP_AutoCommit`, `OP_Savepoint`, `OP_Checkpoint`,
   `OP_Vacuum`, `OP_JournalMode`, `OP_VUpdate`, `OP_VFilter`, `OP_Init`)
   get the smallest available numbers first.
2. Every `jump` opcode gets the next-smallest numbers (so
   `SQLITE_MX_JUMP_OPCODE`, the largest jump opcode's value, stays small).
3. Opcodes marked `group` (must stay numerically contiguous with their
   run) get placed via linear probing for a large-enough open run --
   critically, each group's search restarts from the same base position
   the jump pass left `cnt` at, not from wherever the previous group
   ended up.
4. Everything else fills whatever numbers remain.

`same as TK_x` opcodes are the exception to all of the above: their value
is pinned directly to the parser token constant's value (from `parse.h`),
independent of these four passes, in whichever pass first checks
`op(name)>=0` for them.

## Namespace and API surface

| Header | Contents |
|---|---|
| `generator.hpp` | `sqlite::compiler::tooling::mkopcodeh::generate(std::istream&) -> std::string`, plus `detail::opcode_info` and the parsing/formatting helpers |

`main.cpp` is a thin CLI wrapper matching the legacy tool's contract
exactly: reads stdin, writes stdout.

## Legacy source traceability

Ported from `applications/mkopcodeh-legacy/mkopcodeh.tcl` (relocated from
`tool/mkopcodeh.tcl` by SRS 002 RR-4).

The only intentional output difference from the legacy tool is the
provenance banner comment naming *this* tool's path instead of the legacy
one's -- SRS 002 TR-1's explicitly-anticipated, documented exception,
normalized away by `tool/check-generator-parity.sh` before the RR-7
`generator_parity_mkopcodeh` CTest comparison (run against the real,
already-generated `parse.h` + `src/vdbe.c` the main codegen pipeline
consumes). Everything else is byte-for-byte identical, verified by that
same test.

## Building and running tests independently

```sh
cmake -S applications/mkopcodeh -B build-mkopcodeh
cmake --build build-mkopcodeh
ctest --test-dir build-mkopcodeh
```

Or, as part of the full workspace build, run just this application's tests
with `ctest -R sqlite-mkopcodeh`, and the RR-7 parity check against the
legacy tool with `ctest -R generator_parity_mkopcodeh`.
