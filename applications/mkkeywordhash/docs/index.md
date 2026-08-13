# mkkeywordhash

`sqlite::compiler::tooling::mkkeywordhash` -- SRS 002 RR-5's complete C++
replacement for `applications/mkkeywordhash-legacy/mkkeywordhash.c`, a
build-time generator application (not a runtime library) that has no
`sqlite3` dependency and is therefore unblocked rather than facade-gated
(SRS 002 S5.4.2).

## What it does

Emits `keywordhash.h`: a perfect-hash lookup table over SQLite's SQL
keywords, and the `sqlite3KeywordCode()`/`sqlite3_keyword_name()`/
`sqlite3_keyword_count()`/`sqlite3_keyword_check()` C functions built on
top of it. Unlike `mkopcodeh`/`mkopcodec`, this tool takes no CLI input --
the keyword table is compiled in (see `include/sqlite/compiler/tooling/mkkeywordhash/detail/keyword_table.hpp`,
transcribed in the legacy source's own declaration order, which the offset-
packing algorithm's tie-breaking depends on).

## Namespace and API surface

| Header | Contents |
|---|---|
| `detail/keyword_table.hpp` | The literal keyword source table (name, parser token constant, priority, and whether it survives this workspace's fixed build configuration -- see the header's own comment on why `WITHIN` alone is dropped) |
| `generator.hpp` | `sqlite::compiler::tooling::mkkeywordhash::generate() -> std::string`, plus the `detail::` algorithm internals (hash computation, chain reordering, offset packing) |

`main.cpp` is a two-line CLI wrapper: `std::cout << generate();`, matching
the legacy tool's stdout-only contract exactly.

## Legacy source traceability

Ported from `applications/mkkeywordhash-legacy/mkkeywordhash.c` (relocated
from `tool/mkkeywordhash.c` by SRS 002 RR-4). Every algorithm step --
substring-embedding search, longest-suffix computation, the chained
tail-sharing offset-packing loop, the O(n^2) hash-table-size search, and
the hash-collision-chain priority reordering -- is a faithful, deterministic
line-by-line port; none of it depends on iteration order that C++'s
`std::vector`/`std::sort` don't already guarantee identically to the
legacy `qsort`-based C (every comparator asserts no ties are possible).

The only intentional output difference from the legacy tool is the
provenance banner comment naming *this* tool's path instead of the legacy
one's -- SRS 002 TR-1's explicitly-anticipated, documented exception,
normalized away by `tool/check-generator-parity.sh` before the RR-7
`generator_parity_mkkeywordhash` CTest comparison. Everything else is
byte-for-byte identical, verified by that same test.

## Building and running tests independently

```sh
cmake -S applications/mkkeywordhash -B build-mkkeywordhash
cmake --build build-mkkeywordhash
ctest --test-dir build-mkkeywordhash
```

Or, as part of the full workspace build, run just this application's tests
with `ctest -R sqlite-mkkeywordhash`, and the RR-7 parity check against the
legacy tool with `ctest -R generator_parity_mkkeywordhash`.
