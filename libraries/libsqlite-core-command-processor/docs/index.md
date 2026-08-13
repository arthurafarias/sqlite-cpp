# sqlite-core-command-processor

`sqlite::core::command_processor` -- turns a validated SELECT AST into
actual query execution: WHERE filtering, result columns, DISTINCT, ORDER
BY, LIMIT/OFFSET, run against real row data. Phase 5 of the
[phased delivery plan](../../../srs/001-sqlite-cpp-modularization.md#12-phased-delivery-plan)
in [SRS 001](../../../srs/001-sqlite-cpp-modularization.md), scoped to the
`query` namespace only -- see "What this phase covers" below for exactly
what that means and doesn't mean.

## Position in the dependency graph

```
sqlite-compiler-parser  sqlite-compiler-code-generator  sqlite-backend-tree
              \                      |                    /
               \                     |                   /
                sqlite-core-command-processor     <- you are here
```

Links `sqlite::compiler::tokenizer`/`parser` (parses SQL text -- used by
this library's own tests and any caller), `sqlite::compiler::code_generator`
(compiles expressions to bytecode), `sqlite::core::virtual_machine` (runs
it), and `sqlite::backend::tree` (real on-disk row storage, `record.hpp`/
`btree_table_descriptor.hpp` -- see below). `sqlite::utils` is
test-harness-only, as everywhere else. SRS 001 §3.1 draws exactly this
edge explicitly: "`sqlite-core-command-processor` depends on...
`sqlite-backend` (it drives execution)" -- unlike
`sqlite-core-virtual-machine`, which deliberately doesn't link backend-tree
itself yet (see that library's own docs).

## What this phase covers, and the trick that made it possible without touching the VM

**Single-table SELECT**: `WHERE`, result columns (arbitrary expressions,
`AS` aliases, `*`, `table.*`), `DISTINCT`, `ORDER BY` (multiple keys,
`ASC`/`DESC`, real NULL-first ordering via `aux::compare()`), `LIMIT`/
`OFFSET`, and a FROM-less `SELECT` (e.g. `SELECT 1+1`, evaluated once, no
table needed).

Full SELECT execution sounds like it should need new VM opcodes
(`OP_OpenRead`/`Column`/`Rewind`/`Next` and friends) and a real row-record
format -- neither of which exist anywhere in this workspace yet
(`sqlite-core-virtual-machine`'s docs list exactly why those opcodes are
out of that phase's scope). This library gets single-table SELECT working
anyway, using only APIs those two libraries already expose:

- `sqlite::compiler::code_generator`'s `column_resolver` callback already
  returns a **register index**, not a value -- so "binding a row" is just
  this library deciding a fixed register per column (`0..columns.size()-1`)
  and writing that row's values into those exact registers via
  `interpreter::reg()`.
- `interpreter::reset()` cheaply rewinds the program counter and clears
  registers, so **one compiled program is reused for every row** -- the
  query is compiled once (`WHERE` → `if_not` → result columns →
  `result_row` → `halt`), then `reset()` + rebind + `run()` once per row.
- `interpreter::run()` already distinguishes `run_status::row` (hit
  `OP_ResultRow` -- the row matched) from `run_status::done` (hit `halt`
  without ever reaching `result_row` -- `WHERE` was false) -- exactly the
  per-row "did this match" signal needed, with no new opcode or ad hoc
  halt-code convention required.
- `aux::mem::compare()`/`operator==` already implement SQLite's real
  NULL<NUMERIC<TEXT<BLOB ordering and value equality, covering `ORDER BY`
  and `DISTINCT` directly.

So this is a pure C++ orchestration layer (`select_executor.hpp`) over
already-existing, already-tested library code -- no VM or code-generator
changes were needed.

## Two row sources, one execution core

`select_executor::execute()` is overloaded on two schema/storage types,
both sharing the exact same compile-once/run-per-row core (a
`detail::row_source` callback abstracts "get the next row"; everything
downstream -- WHERE evaluation, DISTINCT, ORDER BY, LIMIT/OFFSET -- doesn't
care which one it came from):

- **`table_descriptor`** (`table_descriptor.hpp`): name, ordered column
  list, `std::vector<std::vector<aux::mem>>` rows, entirely in memory. An
  explicit, deliberate schema/storage stand-in -- the same kind of
  substitution `code_generator`'s resolver callbacks already are for a
  real `Parse` context.
- **`btree_table_descriptor`** (`btree_table_descriptor.hpp`): name,
  ordered column list, and a live `sqlite::backend::tree::btree&` --
  real, on-disk-backed rows, iterated via a real `tree::cursor` and
  decoded through `record.hpp`. Schema (name/columns) is still
  caller-supplied either way -- there's no catalog
  (`sqlite_master`-equivalent) yet, so a table/root-page lookup by name
  isn't this pass's job.

**What made real storage possible without inventing anything new in
`sqlite-backend-tree` or `sqlite-core-virtual-machine`:** the only missing
piece was the row-record format -- how a row's typed `aux::mem` values get
packed into the single opaque payload `btree::insert()` stores, and
unpacked back out of what `cursor::payload()` returns. `record.hpp` ports
legacy `vdbeaux.c`'s `sqlite3VdbeSerialType`/`SerialTypeLen`/`SerialPut`/
`SerialGet` exactly -- a header of serial-type varints (reusing
`sqlite::backend::tree::put_varint`/`get_varint` directly, not
reimplementing them) followed by each column's value bytes: NULL, six
minimal-width signed-integer forms (1/2/3/4/6/8 bytes) plus zero-byte 0/1
special cases, an 8-byte IEEE double, and length-prefixed-by-serial-type
TEXT/BLOB. This is SQLite's real on-disk record format, not a
simplification -- matching `sqlite-backend-tree`'s own established bar
(round-tripped against real bytes in `testing/record_test.hpp`, and against
a real file closed and reopened in
`testing/btree_select_executor_test.hpp`, mirroring that library's own
persistence-test pattern).

Populating a real table for tests/demos isn't SQL `INSERT` (that's
`sqlite::core::command_processor::dml`, still out of scope) --
`btree_table_descriptor.hpp`'s `insert_row()` is a small, explicitly-labeled
programmatic convenience (encode via `record::encode_record`, call
`btree::insert`), playing the same role manually building
`table_descriptor::rows` already does for the in-memory path.

## What's deferred, and why

- **`JOIN`**, even though the parser already parses `join_clause`s --
  `select_executor::execute()` throws `query_error` if `select_stmt::joins`
  is non-empty. Real join execution needs either a nested-loop-join
  strategy or `sqlite-core-command-processor::schema`'s eventual query
  planner; neither exists yet.
- **`GROUP BY`/`HAVING`/aggregate functions.** No aggregate-accumulator
  opcode exists in the VM, and this library doesn't invent one.
- **Function calls in general.** Already rejected by
  `sqlite-compiler-code-generator` itself (`code_generator_error`, caught
  here and rewrapped as `query_error`) -- this library adds no separate
  check for it.
- **Compound SELECT** (`UNION`/etc.), **CTEs**, **subqueries**. Not
  representable in `sqlite-compiler-parser`'s current `select_stmt` AST at
  all (see that library's own docs), so nothing to reject here --
  impossible to construct in the first place.
- **`INSERT`/`UPDATE`/`DELETE`/`UPSERT`, `CREATE TABLE`/`INDEX`/`VIEW`/
  `TRIGGER`, `ALTER`, `PRAGMA`, transactions, triggers, virtual tables,
  authorization hooks.** Every other `sqlite::core::command_processor::*`
  namespace SRS 001 §3.4.2 lists (`prepare`, `pragma`, `dml`, `schema`,
  `trigger`, `window`, `fkey`, `auth`, `debug`) is simply not built this
  pass -- this library implements `query` only.

## Building and running tests independently

```sh
cmake -S libraries/libsqlite-core-command-processor -B build-command-processor
cmake --build build-command-processor
ctest --test-dir build-command-processor
```

Or, as part of the workspace build: `ctest --test-dir build -R sqlite-core-command-processor`.

Tests parse real SQL text end-to-end and execute it against a small
in-memory demo table (`testing/select_helper.hpp`'s `demo_table()`,
several column types including `NULL`), asserting on actual query
results -- not on hand-built ASTs or the shape of emitted instructions,
mirroring `sqlite-compiler-code-generator/testing/eval_helper.hpp`'s
role one layer down the stack. `testing/record_test.hpp` round-trips every
value kind (including integer-width boundaries and `INT64_MIN`/`INT64_MAX`)
through `encode_record`/`decode_record`; `testing/btree_select_executor_test.hpp`
runs a real `SELECT` against a `page_store`-backed file that's closed and
reopened first, confirming the round trip through actual on-disk bytes.

## Consuming this library

```cmake
include(cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_command_processor()
target_link_libraries(your_target PRIVATE sqlite::core::command_processor)
```

```cpp
#include <sqlite/core/command_processor/select_executor.hpp>
#include <sqlite/compiler/parser/parser.hpp>

sqlite::compiler::parser::parser p("SELECT name FROM t WHERE id > 1 ORDER BY name");
auto stmt = p.parse_select();

sqlite::core::command_processor::table_descriptor table;
table.name = "t";
table.columns = {"id", "name"};
table.rows = { /* ... */ };

sqlite::core::command_processor::select_executor executor;
auto result = executor.execute(stmt, table); // query_result: column_names + rows
```

Against a real, on-disk-backed table instead:

```cpp
#include <sqlite/core/command_processor/btree_table_descriptor.hpp>
#include <sqlite/backend/tree/page_store.hpp>

sqlite::backend::tree::page_store store("mydb.sqlite3", 4096);
sqlite::backend::tree::btree bt(store);
sqlite::core::command_processor::insert_row(bt, /*rowid=*/1,
    {vm::aux::mem::make_integer(1), vm::aux::mem::make_text("alice")});

sqlite::core::command_processor::btree_table_descriptor table{"t", {"id", "name"}, bt};
auto result = executor.execute(stmt, table); // same select_executor, same query_result
```
