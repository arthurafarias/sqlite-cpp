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
1-4 (partial) exist so far:

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
| `sqlite-core-command-processor` / `sqlite-core-interface` | Not yet started (phase 5) | -- |
| Extensions (`fts5`, `rtree`, ...) | Not yet started (phase 6) | -- |

## Legacy track (SRS 002)

[SRS 002](../srs/002-full-c-retirement.md) restructures the legacy C
implementation into a permanent, structurally parallel track standing next
to the `sqlite-cpp` libraries above, rather than something deleted
piecemeal as each library lands. This pass implements §10 Phase 1 (legacy
rename and packaging) and the relocation half of Phase 2
(generator-application relocation); the complete C++ replacements RR-5
requires for the four generator applications are deferred follow-up work,
tracked the same way each `sqlite-cpp` library above was delivered as its
own separate piece of work.

| Component | Legacy target | Status |
|---|---|---|
| Core C library | `libsqlite3-legacy` (`libraries/libsqlite3-legacy`) | Renamed and repackaged per RR-1 (own CMake package, `sqlite_cpp_require_legacy()`) |
| `sqlite3` shell | `applications/sqlite3-shell-legacy` | Renamed per RR-2; unsuffixed `sqlite3-shell` reserved (RR-3), façade-gated |
| `sqldiff` | `applications/sqldiff-legacy` | Renamed per RR-2; unsuffixed `sqldiff` reserved (RR-3), façade-gated |
| `sqlite3-rsync` | `applications/sqlite3-rsync-legacy` | Renamed per RR-2; unsuffixed `sqlite3-rsync` reserved (RR-3), façade-gated |
| `speedtest1` | `applications/speedtest1-legacy` | Relocated from `benchmarks/` (closing a pre-existing SRS 001 FR-20 gap) and renamed per RR-2; unsuffixed `speedtest1` reserved (RR-3), meant to coexist permanently once it exists (SRS 002 S5.4.1) |
| `lemon` | `applications/lemon-legacy` | Relocated from `tool/lemon.c` per RR-4. Complete C++ replacement (RR-5, `applications/lemon`) **not yet started** -- deferred |
| `mkkeywordhash` | `applications/mkkeywordhash-legacy` | Relocated from `tool/mkkeywordhash.c` per RR-4. Complete C++ replacement (RR-5) **not yet started** -- deferred |
| `mkopcodeh` | `applications/mkopcodeh-legacy` | Relocated from `tool/mkopcodeh.tcl` per RR-4. Complete C++ replacement (RR-5) **not yet started** -- deferred, sequenced with `sqlite-core-virtual-machine` |
| `mkopcodec` | `applications/mkopcodec-legacy` | Relocated from `tool/mkopcodec.tcl` per RR-4. Complete C++ replacement (RR-5) **not yet started** -- deferred, sequenced with `sqlite-core-virtual-machine` |

RR-6's freeze guarantee is enforced mechanically (TR-4): `tests/legacy-freeze/manifest.sha256`
records a checksum of every file under `libraries/libsqlite3-legacy/` and
`applications/*-legacy/` except each directory's own `CMakeLists.txt`/`cmake/*`
build glue (the carve-out RR-6 itself permits); `ctest -R legacy_freeze_check`
(or `-L comparison`) verifies nothing else has drifted. `tool/update-legacy-freeze-manifest.sh`
regenerates the manifest -- running it is the mechanical form of "an
explicitly reviewed, documented exception."

**Not yet done, left for follow-up work:** RR-5 (the four generators' complete
C++ replacements), RR-7 (generator-output parity -- nothing to diff against
until RR-5 lands), RR-8 (sqlite3-dependent application comparison -- blocked
on the `sqlite-core-interface` facade, phase 5 above), and RR-9 (per-library
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
