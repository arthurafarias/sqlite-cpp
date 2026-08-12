# sqlite-cpp documentation index

This is the root documentation index for the `sqlite-cpp` refactor described
in [SRS.md](../SRS.md) (FR-18): a C++ reimplementation of SQLite's C core as
a set of independent, namespaced, header-only libraries, preserving the
existing public C API and behavior exactly.

This index links out to each library's own independent documentation rather
than duplicating it (FR-19), plus the pre-existing project documentation the
refactor does not replace.

## sqlite-cpp libraries

Per the SRS's [phased delivery plan](../SRS.md#12-phased-delivery-plan), phases
1-4 (partial) exist so far:

| Library | Status | Docs |
|---|---|---|
| `sqlite-utils` | Implemented (phase 1) | [libraries/libsqlite-utils/docs/index.md](../libraries/libsqlite-utils/docs/index.md) |
| `sqlite-backend-os` | Implemented (phase 2, part 1) | [libraries/libsqlite-backend-os/docs/index.md](../libraries/libsqlite-backend-os/docs/index.md) |
| `sqlite-backend-pager` / `-tree` | Not yet started (phase 2, remainder) | -- |
| `sqlite-core-virtual-machine` | Implemented (phase 3, scope renegotiated -- see its docs) | [libraries/libsqlite-core-virtual-machine/docs/index.md](../libraries/libsqlite-core-virtual-machine/docs/index.md) |
| `sqlite-compiler-tokenizer` | Implemented (phase 4, part 1) | [libraries/libsqlite-compiler-tokenizer/docs/index.md](../libraries/libsqlite-compiler-tokenizer/docs/index.md) |
| `sqlite-compiler-parser` / `-code-generator` / `sqlite-core-command-processor` / `-interface` | Not yet started (phase 4 remainder / 5) | -- |
| Extensions (`fts5`, `rtree`, ...) | Not yet started (phase 6) | -- |

## Existing project documentation

- [SRS.md](../SRS.md) -- the requirements this refactor implements.
- [doc/](../doc/) -- legacy SQLite design notes (`wal-lock.md`,
  `pager-invariants.txt`, `vdbesort-memory.md`, etc.) describing invariants
  the new implementation must continue to uphold.
- [README.md](../README.md) -- the original SQLite project README.
