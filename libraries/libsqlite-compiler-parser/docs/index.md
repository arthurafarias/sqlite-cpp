# sqlite-compiler-parser

`sqlite::compiler::parser` -- turns a token stream into an AST: SQL
expressions (full operator precedence) and single SELECT statements. The
second piece of phase 4's "lower risk" half in the
[phased delivery plan](../../../srs/001-sqlite-cpp-modularization.md#12-phased-delivery-plan)
([SRS 001](../../../srs/001-sqlite-cpp-modularization.md)), scoped down from the full SQL grammar to
expressions + SELECT by explicit agreement before implementation started --
see "What this phase covers" below.

## Position in the dependency graph

```
sqlite-compiler-tokenizer
           |
sqlite-compiler-parser        <- you are here
           |
sqlite-compiler-code-generator  (not yet built)
```

Links `sqlite::compiler::tokenizer` (consumes its `token`/`token_type`/
`tokenize()`) and `sqlite::utils` (test harness only).

## Namespaces and legacy source mapping

| Namespace | Header(s) | Legacy source |
|---|---|---|
| `sqlite::compiler::parser` | `parser.hpp`, `parse_error.hpp` | `parse.y`'s grammar rules for `expr` and `select`, compiled by LEMON into `parse.c`'s LALR(1) tables |
| `sqlite::compiler::parser::ast` | `ast/expr.hpp`, `ast/select_stmt.hpp` | The `Expr`/`Select`/`SrcList`/`ExprList` structs (expr.c/select.c/sqliteInt.h) and the AST-construction portions of build.c |

## Why this is hand-written, not a port of the generated grammar

`parse.y` is a LEMON grammar source: LEMON compiles it into `parse.c`, a
table-driven LALR(1) automaton (state-transition tables plus a byte-code-like
action dispatcher), the same relationship `applications/mkkeywordhash-legacy/mkkeywordhash.c` has to
`keywordhash.h`. Hand-transcribing generated parser tables isn't a
meaningful "port" the way porting `sqlite3GetToken()`'s character-by-character
logic was for the tokenizer -- there's no hand-writable equivalent of an
LALR table that stays recognizable as the "same code." This library instead
implements the grammar *rules* `parse.y` describes directly, as a
recursive-descent parser with one function per precedence level for
expressions (`parse_or` → `parse_and` → ... → `parse_unary` → `parse_primary`,
`parser.hpp`), matching `parse.y`'s own `%left`/`%right` precedence
declarations level-for-level (each function's doc comment names which one).

## What this phase covers

**Expressions**, with SQLite's actual precedence and associativity, verified
against real SQL text (`testing/expr_test.hpp`) rather than by construction
alone: arithmetic (`+ - * / %`), string concatenation (`||`), comparisons
(`= == != <> < <= > >=`), logical (`AND OR NOT`), bitwise (`& | << >>`),
`IS`/`IS NOT`, `IS NULL`/`ISNULL`/`NOTNULL`/`IS NOT NULL`, `[NOT] IN (...)`,
`[NOT] BETWEEN ... AND ...`, `[NOT] LIKE/GLOB/REGEXP/MATCH ... [ESCAPE ...]`,
`CASE [base] WHEN ... THEN ... [ELSE ...] END` (both searched and
base-expression forms), function calls (`f(...)`, `f(DISTINCT ...)`,
`f(*)`), parenthesized sub-expressions, and every literal kind (integer,
float, string with `''`-escaping, blob, `NULL`, bind variables).

**Single SELECT statements**: `DISTINCT`/`ALL`, result columns (`expr [[AS]
alias]`, `*`, `table.*`), `FROM` with a table (and optional alias),
comma-joins, `[NATURAL] [INNER|LEFT [OUTER]|CROSS] JOIN ... ON`/`USING`,
`WHERE`, `GROUP BY ... HAVING`, `ORDER BY ... [ASC|DESC]`, and `LIMIT`
(including both the `LIMIT n OFFSET m` and legacy `LIMIT m, n` spellings).

## What's deferred, and why

Each of these needs either a schema (which requires
`sqlite-core-command-processor`, not built) or is simply out of the
expressions-and-SELECT slice agreed before starting this phase:

- **`INSERT`/`UPDATE`/`DELETE`/`UPSERT`, `CREATE TABLE`/`INDEX`/`VIEW`/
  `TRIGGER`, `ALTER`, `PRAGMA`, transactions.** Different statement grammars
  entirely; each is its own follow-up slice once needed.
- **Compound SELECT** (`UNION`/`UNION ALL`/`INTERSECT`/`EXCEPT`), **CTEs**
  (`WITH ... AS (...)`), **subqueries** (in `FROM`, as scalar expressions, or
  in `IN (SELECT ...)`), and **window functions** (`OVER (...)`) -- all real
  `select.c` grammar, all deferred as the next natural extensions of this
  same recursive-descent structure once needed.
- **`RIGHT`/`FULL [OUTER]` JOIN.** `parse_join()` only recognizes `INNER`/
  `LEFT [OUTER]`/`CROSS`/`NATURAL`; a straightforward addition, just not
  included in this pass.
- **The `WINDOW`/`OVER`/`FILTER` keyword-vs-identifier lookahead**
  (tokenize.c's `analyzeWindowKeyword` et al.) -- would only matter once
  window functions are parsed.
- **Keywords used as bare identifiers.** Real SQLite lets many non-reserved
  keywords (e.g. `KEY`, `TEXT`) stand in for column/table names in most
  contexts; this parser only accepts `token_type::id` where an identifier is
  expected, so a keyword can never double as one. A deliberate simplification
  for this phase, not a legacy-compatibility gap that's been overlooked.
- **`COLLATE`.** Present in `parse.y`'s precedence table (between concat and
  unary) but not implemented; collation is meaningless before
  `sqlite-core-command-processor`'s schema/collation-registry work exists.

## Building and running tests independently

```sh
cmake -S libraries/libsqlite-compiler-parser -B build-parser
cmake --build build-parser
ctest --test-dir build-parser
```

Or, as part of the workspace build: `ctest --test-dir build -R sqlite-compiler-parser`.

## Consuming this library

```cmake
include(cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_compiler_parser()
target_link_libraries(your_target PRIVATE sqlite::compiler::parser)
```
