# sqlite-cpp-repl

A small REPL that reads SQL text and executes it via
[`sqlite-core-command-processor`](../../libraries/libsqlite-core-command-processor/docs/index.md)'s
`select_executor`, against one hardcoded in-memory demo table (`t(id,
name, score)`, 5 rows including a `NULL` score). It is the first
new-track application in this workspace that runs real, user-typed SQL
end-to-end: tokenizer → parser → code-generator → VM.

## Why this isn't `applications/sqlite3-shell`

[SRS 002](../../srs/002-full-c-retirement.md) RR-3 reserves the unsuffixed
`sqlite3-shell` name for the eventual `sqlite-core-interface`-facade-gated,
C-API-compatible replacement of the real shell -- "must not be reused for
anything else." That doesn't exist yet, and this REPL is a fundamentally
smaller thing: no file-backed database (`.open` doesn't exist), no C API,
a single hardcoded demo table, single-table `SELECT` only (see
`select_executor`'s own docs for exactly what that covers: `WHERE`, result
columns, `DISTINCT`, `ORDER BY`, `LIMIT`/`OFFSET` -- no `JOIN`, no `GROUP
BY`, no other statement kinds). No dot-commands.

## Usage

```sh
cmake --build build --target sqlite-cpp-repl
./build/applications/sqlite-cpp-repl/sqlite-cpp-repl
```

```
sqlite-cpp> SELECT name, score FROM t WHERE score > 5 ORDER BY score DESC;
name|score
alice|9.5
bob|7.0
dave|7.0
sqlite-cpp> SELECT 1 + 2 AS total;
total
3
sqlite-cpp>
```

Output is pipe-separated with a header row (`NULL` prints as an empty
field). Parse/execution errors print to stderr and the loop continues,
matching the real shell's error tolerance. An empty line or EOF quits.

## Building standalone

```sh
cmake -S applications/sqlite-cpp-repl -B build-repl
cmake --build build-repl
./build-repl/sqlite-cpp-repl
```
