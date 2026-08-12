# sqlite-compiler-tokenizer

`sqlite::compiler::tokenizer` -- the SQL lexer: turns raw SQL text into a
stream of typed tokens (keywords, identifiers, literals, operators,
punctuation). The first, lower-risk half of phase 4 of the
[phased delivery plan](../../../SRS.md#12-phased-delivery-plan) in
[SRS.md](../../../SRS.md); per the SRS's own §12 note, phase 4's
code-generator/command-processor half is deliberately deferred as its own
reviewed sub-phase ("the phase most likely to need its timeline revisited"),
and this pass implements only the tokenizer.

## Position in the dependency graph

```
sqlite-utils
     |
sqlite-backend-os          sqlite-core-virtual-machine
     |                                |
     +----------------+---------------+
                       |
          sqlite-compiler-tokenizer     <- you are here
                       |
          sqlite-compiler-parser         (not yet built)
                       |
          sqlite-compiler-code-generator (not yet built)
```

Links `sqlite::utils` for build plumbing (the shared test harness) only --
no header under `include/sqlite/compiler/tokenizer/` (excluding `testing/`)
includes anything from `sqlite::utils`. Does not depend on
`sqlite-backend-os` or `sqlite-core-virtual-machine`: tokenizing SQL text is
purely a string-in, tokens-out operation with no I/O or VM involvement.

## Namespaces and legacy source mapping

| Namespace | Header(s) | Legacy source |
|---|---|---|
| `sqlite::compiler::tokenizer` | `token_type.hpp`, `token.hpp`, `keyword_table.hpp`, `tokenizer.hpp` | `tokenize.c`'s `sqlite3GetToken()`, `parse.h`'s generated `TK_*` constants, `tool/mkkeywordhash.c`'s generated `keywordhash.h` |

## What's real here

`next_token()` is a complete, working port of `sqlite3GetToken()`'s
character-class dispatch -- every case in the legacy function has a
corresponding branch here, verified against the same inputs
(`testing/tokenizer_test.hpp`): whitespace runs, `--` and `/* */` comments
(including the legacy quirk where an unterminated block comment consumes to
end-of-input and is still reported as a comment, not an error), all the
operators and their multi-character forms (`==`, `<=`, `<>`, `<<`, `>=`,
`>>`, `||`, `->`, `->>`, `!=`), single-quoted strings with `''`-doubled-quote
escaping, double-quoted/backtick-quoted/bracket-quoted identifiers, integer/
float/hex-integer literals with SQLite's `_`-digit-separator extension
(`TK_QNUMBER`), blob literals (`x'...'`), numbered and named bind variables
(`?`, `?5`, `:name`, `@name`, `$name`, `#name`, including the TCL-variable
`$name(...)` extension), and the full 148-keyword table from
`tool/mkkeywordhash.c`, matched case-insensitively.

`token_type.hpp`'s enum has one member per SQL keyword (not collapsed into
legacy's `TK_JOIN_KW`/`TK_LIKE_KW`/`TK_CTIME_KW` fallback classes -- see that
header's comment for why) plus every operator/punctuation/literal class, and
`keyword_table.hpp` replaces the generated perfect hash with a plain linear
scan over a 148-entry array: asymptotically slower, but not
performance-relevant at this size, and far easier to keep in sync with
`token_type.hpp` by inspection than hand-transcribing generated hash-function
output would be.

## What's deferred, and why

- **The EBCDIC code path.** `aiClass[]`'s `#ifdef SQLITE_EBCDIC` half is not
  ported: no supported target platform (SRS §2.4's GCC/CMake toolchain) uses
  EBCDIC.
- **`analyzeWindowKeyword`/`analyzeOverKeyword`/`analyzeFilterKeyword`**
  (tokenize.c) -- the lookahead logic that disambiguates `WINDOW`/`OVER`/
  `FILTER` as keywords vs. identifiers depending on surrounding tokens. This
  is inherently a parser-level concern (it needs to see tokens *after* the
  one being classified), so it belongs with `sqlite-compiler-parser`, not
  the tokenizer.
- **`sqlite3RunParser()`'s driver loop** (tokenize.c) -- feeding tokens one
  at a time into the LEMON-generated parser engine. That's
  `sqlite-compiler-parser`'s job once it exists; this library's own
  `tokenize()` convenience function (collect the whole stream, optionally
  dropping trivia) is what a parser will eventually replace with real
  token-by-token consumption.

## Building and running tests independently

```sh
cmake -S libraries/libsqlite-compiler-tokenizer -B build-tokenizer
cmake --build build-tokenizer
ctest --test-dir build-tokenizer
```

Or, as part of the workspace build: `ctest --test-dir build -R sqlite-compiler-tokenizer`.

## Consuming this library

```cmake
include(cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_compiler_tokenizer()
target_link_libraries(your_target PRIVATE sqlite::compiler::tokenizer)
```
