# sqlite-cpp documentation index

This is the root documentation index for the `sqlite-cpp` refactor described
in [SRS.md](../SRS.md) (FR-18): a C++ reimplementation of SQLite's C core as
a set of independent, namespaced, header-only libraries, preserving the
existing public C API and behavior exactly.

This index links out to each library's own independent documentation rather
than duplicating it (FR-19), plus the pre-existing project documentation the
refactor does not replace.

## sqlite-cpp libraries

Per the SRS's [phased delivery plan](../SRS.md#12-phased-delivery-plan), only
phase 1 exists so far:

| Library | Status | Docs |
|---|---|---|
| `sqlite-utils` | Implemented (phase 1) | [libraries/libsqlite-utils/docs/index.md](../libraries/libsqlite-utils/docs/index.md) |
| `sqlite-backend-os` / `-pager` / `-tree` | Not yet started (phase 2) | -- |
| `sqlite-core-virtual-machine` | Not yet started (phase 3) | -- |
| `sqlite-compiler-*` / `sqlite-core-command-processor` / `-interface` | Not yet started (phase 4-5) | -- |
| Extensions (`fts5`, `rtree`, ...) | Not yet started (phase 6) | -- |

## Existing project documentation

- [SRS.md](../SRS.md) -- the requirements this refactor implements.
- [doc/](../doc/) -- legacy SQLite design notes (`wal-lock.md`,
  `pager-invariants.txt`, `vdbesort-memory.md`, etc.) describing invariants
  the new implementation must continue to uphold.
- [README.md](../README.md) -- the original SQLite project README.
