# sqlite-cpp documentation index

This is the root documentation index for the `sqlite-cpp` refactor, currently governed
by a new SRS set — see [srs/index.md](../srs/index.md) — that supersedes an earlier,
now-abandoned one. Every artifact produced under the earlier plan (nine header-only C++
libraries, `sqlite-cpp-repl`, three ported generator tools, and the legacy-freeze/parity
testing apparatus) has been removed from the tree.

## Current status

The workspace is mid-reset, at [SRS 001](../srs/001-legacy-restructure-and-dynamic-linking.md)'s
FR-1 (cleanup): the old-plan artifacts are gone and the CMake files that referenced them
are fixed up to stay valid, but the rest of SRS 001 (FR-2 consolidating the original
source into `legacy/`, FR-3 populating per-library `csrc/` trees, FR-4 converting
libraries to dynamic linking, FR-5 doing the same for the legacy executables, FR-6
rewiring the workspace end-to-end) is not yet done. **The workspace does not fully build
right now** — `libraries/libsqlite3-legacy` and the `applications/*-legacy` targets
still build from the untouched original source, but nothing else does.

No per-library documentation exists yet; it will be added under each
`libraries/<name>/docs/` as libraries are populated, per the pattern the previous plan
used (not itself changed by the new SRS set).

## SRS documents

- [SRS 001](../srs/001-legacy-restructure-and-dynamic-linking.md) — legacy source
  consolidation, per-library C source split, and the dynamic-linking conversion.
- [SRS 002](../srs/002-cpp-conversion.md) — C to C++ conversion, applied per library
  once SRS 001 lands for it.
- [SRS 003](../srs/003-stl-based-architecture.md) — STL-based internals, applied per
  library once SRS 002 lands for it.
- [srs/index.md](../srs/index.md) — the index of all SRS documents and how they relate.

## Existing project documentation

- [doc/](../doc/) — legacy SQLite design notes (`wal-lock.md`,
  `pager-invariants.txt`, `vdbesort-memory.md`, etc.) describing invariants
  the restructured implementation must continue to uphold.
- [README.md](../README.md) — the original SQLite project README.
