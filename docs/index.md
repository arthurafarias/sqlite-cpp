# sqlite-cpp documentation index

This is the root documentation index for the `sqlite-cpp` refactor described
in [SRS 001](../srs/001-sqlite-cpp-modularization.md) (FR-18): a C++ reimplementation of SQLite's C core as
a set of independent, namespaced, header-only libraries, preserving the
existing public C API and behavior exactly.

This index links out to each library's own independent documentation rather
than duplicating it (FR-19), plus the pre-existing project documentation the
refactor does not replace.

## sqlite-cpp libraries

Per the SRS's [phased delivery plan](../srs/001-sqlite-cpp-modularization.md#12-phased-delivery-plan), phases
1-4 and part of phase 5 exist so far:

| Library | Status | Docs |
|---|---|---|
| `sqlite-utils` | Implemented (phase 1) | [libraries/libsqlite-utils/docs/index.md](../libraries/libsqlite-utils/docs/index.md) |
| `sqlite-backend-os` | Implemented (phase 2, part 1) | [libraries/libsqlite-backend-os/docs/index.md](../libraries/libsqlite-backend-os/docs/index.md) |
| `sqlite-backend-tree` | Implemented (phase 2, tree slice, real on-disk format + multi-page balancing -- see its docs) | [libraries/libsqlite-backend-tree/docs/index.md](../libraries/libsqlite-backend-tree/docs/index.md) |
| `sqlite-backend-pager` | Not yet started (phase 2, remainder -- `sqlite-backend-tree` currently bypasses it, see that library's docs) | -- |
| `sqlite-core-virtual-machine` | Implemented (phase 3, scope renegotiated -- see its docs) | [libraries/libsqlite-core-virtual-machine/docs/index.md](../libraries/libsqlite-core-virtual-machine/docs/index.md) |
| `sqlite-compiler-tokenizer` | Implemented (phase 4, part 1) | [libraries/libsqlite-compiler-tokenizer/docs/index.md](../libraries/libsqlite-compiler-tokenizer/docs/index.md) |
| `sqlite-compiler-parser` | Implemented (phase 4, part 1, scope: expressions + single SELECT -- see its docs) | [libraries/libsqlite-compiler-parser/docs/index.md](../libraries/libsqlite-compiler-parser/docs/index.md) |
| `sqlite-compiler-code-generator` | Implemented (phase 4, S11.2's highest-risk seam, scope: expressions only -- see its docs) | [libraries/libsqlite-compiler-code-generator/docs/index.md](../libraries/libsqlite-compiler-code-generator/docs/index.md) |
| `sqlite-core-command-processor` | Implemented (phase 5, `query` namespace only: single-table SELECT orchestration against an in-memory `table_descriptor` -- no VM/code-generator changes needed, see its docs for how) | [libraries/libsqlite-core-command-processor/docs/index.md](../libraries/libsqlite-core-command-processor/docs/index.md) |
| `sqlite-core-interface` | Not yet started (phase 5, remainder) | -- |
| Extensions (`fts5`, `rtree`, ...) | Not yet started (phase 6) | -- |

## Applications built on sqlite-cpp

| Application | Status | Docs |
|---|---|---|
| `sqlite-cpp-repl` | Implemented -- a REPL running real SQL text end-to-end (tokenizer -> parser -> code-generator -> VM -> command-processor) against a hardcoded in-memory demo table. Deliberately not named `sqlite3-shell` -- SRS 002 RR-3 reserves that name for the eventual facade-gated, C-API-compatible replacement | [applications/sqlite-cpp-repl/docs/index.md](../applications/sqlite-cpp-repl/docs/index.md) |

## Legacy track (SRS 002)

[SRS 002](../srs/002-full-c-retirement.md) restructures the legacy C
implementation into a permanent, structurally parallel track standing next
to the `sqlite-cpp` libraries above, rather than something deleted
piecemeal as each library lands. §10 Phase 1 (legacy rename and packaging)
and Phase 2 (generator-application relocation and replacement) are done for
three of the four generator applications; `lemon`'s replacement (a full
LALR(1) engine, 6,075 lines) is large enough to warrant its own dedicated
pass and remains deferred.

| Component | Legacy target | Status |
|---|---|---|
| Core C library | `libsqlite3-legacy` (`libraries/libsqlite3-legacy`) | Renamed and repackaged per RR-1 (own CMake package, `sqlite_cpp_require_legacy()`) |
| `sqlite3` shell | `applications/sqlite3-shell-legacy` | Renamed per RR-2; unsuffixed `sqlite3-shell` reserved (RR-3), façade-gated |
| `sqldiff` | `applications/sqldiff-legacy` | Renamed per RR-2; unsuffixed `sqldiff` reserved (RR-3), façade-gated |
| `sqlite3-rsync` | `applications/sqlite3-rsync-legacy` | Renamed per RR-2; unsuffixed `sqlite3-rsync` reserved (RR-3), façade-gated |
| `speedtest1` | `applications/speedtest1-legacy` | Relocated from `benchmarks/` (closing a pre-existing SRS 001 FR-20 gap) and renamed per RR-2; unsuffixed `speedtest1` reserved (RR-3), meant to coexist permanently once it exists (SRS 002 S5.4.1) |
| `lemon` | `applications/lemon-legacy` | Relocated from `tool/lemon.c` per RR-4. Complete C++ replacement (RR-5, `applications/lemon`) **not yet started** -- deferred, its own dedicated pass |
| `mkkeywordhash` | `applications/mkkeywordhash-legacy` | Relocated per RR-4. **Complete C++ replacement done** ([`applications/mkkeywordhash`](../applications/mkkeywordhash/docs/index.md), RR-5) -- byte-for-byte parity with the legacy tool verified by `ctest -R generator_parity_mkkeywordhash` (RR-7) |
| `mkopcodeh` | `applications/mkopcodeh-legacy` | Relocated per RR-4. **Complete C++ replacement done** ([`applications/mkopcodeh`](../applications/mkopcodeh/docs/index.md), RR-5) -- byte-for-byte parity verified by `ctest -R generator_parity_mkopcodeh` (RR-7) |
| `mkopcodec` | `applications/mkopcodec-legacy` | Relocated per RR-4. **Complete C++ replacement done** ([`applications/mkopcodec`](../applications/mkopcodec/docs/index.md), RR-5) -- byte-for-byte parity verified by `ctest -R generator_parity_mkopcodec` (RR-7) |

Each of the three completed replacements agrees with its legacy
counterpart's output exactly, except the one difference SRS 002 TR-1
explicitly anticipates and permits: the "generated by `<path>`" provenance
banner, which necessarily differs since the new tool isn't generated by the
old tool's path. `tool/check-generator-parity.sh` normalizes exactly that
line away before diffing -- see each application's own `docs/index.md` for
detail, and its own header comments for why each ported algorithm's control
flow (offset-packing in `mkkeywordhash`, opcode-numbering priority passes
in `mkopcodeh`) is preserved exactly rather than "cleaned up."

RR-6's freeze guarantee is enforced mechanically (TR-4): `tests/legacy-freeze/manifest.sha256`
records a checksum of every file under `libraries/libsqlite3-legacy/` and
`applications/*-legacy/` except each directory's own `CMakeLists.txt`/`cmake/*`
build glue (the carve-out RR-6 itself permits); `ctest -R legacy_freeze_check`
(or `-L comparison`) verifies nothing else has drifted. `tool/update-legacy-freeze-manifest.sh`
regenerates the manifest -- running it is the mechanical form of "an
explicitly reviewed, documented exception."

**Not yet done, left for follow-up work:** `lemon`'s complete C++
replacement (RR-5) and its generator-output parity check (RR-7); RR-8
(sqlite3-dependent application comparison -- blocked on the
`sqlite-core-interface` facade, phase 5 above); and RR-9 (per-library
comparison against `libsqlite3-legacy`, within each library's declared
scope -- substantial work deserving its own pass). Final deletion of the
legacy track (S9.1) remains an open, unmade decision, as SRS 002 requires.

## Existing project documentation

- [SRS 001](../srs/001-sqlite-cpp-modularization.md) -- the requirements this refactor implements.
- [SRS 002](../srs/002-full-c-retirement.md) -- restructures the legacy C
  implementation (`libsqlite3-legacy`, `*-legacy` applications, C++-ported
  code generators) into a permanent track compared directly against
  `sqlite-cpp`; final deletion is left an explicit, later decision.
- [SRS 003](../srs/003-sil4-safety-integrity-validation.md) -- SIL4 safety-integrity
  validation requirements layered on top of SRS 001, per library and then per
  application.
- [srs/index.md](../srs/index.md) -- the index of all SRS documents.
- [doc/](../doc/) -- legacy SQLite design notes (`wal-lock.md`,
  `pager-invariants.txt`, `vdbesort-memory.md`, etc.) describing invariants
  the new implementation must continue to uphold.
- [README.md](../README.md) -- the original SQLite project README.
