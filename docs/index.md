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

## Existing project documentation

- [SRS 001](../srs/001-sqlite-cpp-modularization.md) -- the requirements this refactor implements.
- [SRS 002](../srs/002-full-c-retirement.md) -- the plan for fully retiring
  the legacy C implementation, per library, once each `sqlite-cpp`
  replacement is accepted.
- [SRS 003](../srs/003-sil4-safety-integrity-validation.md) -- SIL4 safety-integrity
  validation requirements layered on top of SRS 001, per library and then per
  application.
- [srs/index.md](../srs/index.md) -- the index of all SRS documents.
- [doc/](../doc/) -- legacy SQLite design notes (`wal-lock.md`,
  `pager-invariants.txt`, `vdbesort-memory.md`, etc.) describing invariants
  the new implementation must continue to uphold.
- [README.md](../README.md) -- the original SQLite project README.
