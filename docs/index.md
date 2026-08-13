# sqlite-cpp documentation index

This is the root documentation index for the `sqlite-cpp` refactor, currently governed
by the SRS set at [srs/index.md](../srs/index.md): a strict linear pipeline — legacy
source restructuring and dynamic linking (SRS 001), then C++ conversion (SRS 002), then
namespace-mirrored file organization (SRS 003), then STL-based internals (SRS 004) —
applied library by library.

## Current status

[SRS 001](../srs/001-legacy-restructure-and-dynamic-linking.md) is complete: `legacy/`
holds an untouched copy of the original SQLite source; each of the ten libraries below
builds as a real, independently compiled `.so` from its own `csrc/` (populated from
`legacy/`); every `sqlite3`-dependent legacy application links against that split
dynamically instead of embedding the amalgamation. The whole workspace configures,
builds, and passes the existing TCL oracle (`ctest -R sqlite_veryquick`) at the same
pre-existing `zipfile-25.0`-only baseline it always has.

[SRS 002](../srs/002-cpp-conversion.md) has been applied to the first four
dependency leaves, through `sqlite-backend-tree`. They are C++17 shared
libraries with namespaced state-container APIs. The other six libraries remain
plain C at the SRS 001 end state; `sqlite-core-virtual-machine` is next.

| Library | `.so` | SRS stage |
|---|---|---|
| `sqlite-utils` | `libsqlite-utils.so` | SRS 002 — C++17 (`sqlite::utils`) |
| `sqlite-backend-os` | `libsqlite-backend-os.so` | SRS 002 — C++17 (`sqlite::backend::os`) |
| `sqlite-backend-pager` | `libsqlite-backend-pager.so` | SRS 002 — C++17 (`sqlite::backend::pager`) |
| `sqlite-backend-tree` | `libsqlite-backend-tree.so` | SRS 002 — C++17 (`sqlite::backend::tree`) |
| `sqlite-core-virtual-machine` | `libsqlite-core-virtual-machine.so` | SRS 001 — C |
| `sqlite-core-command-processor` | `libsqlite-core-command-processor.so` | SRS 001 — C |
| `sqlite-core-interface` | `libsqlite-core-interface.so` | SRS 001 — C |
| `sqlite-compiler-tokenizer` | `libsqlite-compiler-tokenizer.so` | SRS 001 — C |
| `sqlite-compiler-parser` | `libsqlite-compiler-parser.so` | SRS 001 — C |
| `sqlite-compiler-code-generator` | `libsqlite-compiler-code-generator.so` | SRS 001 — C |

None of the sibling libraries link against each other (`target_link_libraries`) —
symbol resolution happens at process load time once something links against the whole
set (`cmake/SqliteCppLibrary.cmake`'s `sqlite_cpp_link_full_split()`). This is a
private implementation constraint: the original symbol graph is cyclic across these
boundaries. It is not part of the installed namespaced C++ API. See SRS 001 §6
(FR-4's status note) for the full reasoning.

Extensions (`fts3`, `fts4`, `fts5`, `rtree`, `geopoly`, `session`, `rbu`, `icu`,
`dbstat`/`dbpage`) are out of scope for SRS 001 — they still build directly from
`legacy/ext`. Two of the legacy applications (`sqlite3-shell-legacy`,
`sqlite3-rsync-legacy`) lost FTS4/RTree/stmt-vtab support as a result, until extensions
get their own pass (SRS 001 §6's FR-5 note).

## Legacy executables

| Application | Links against |
|---|---|
| `sqlite3-shell-legacy` (`sqlite3-legacy`) | The 10-library split |
| `sqldiff-legacy` | The 10-library split (previously `libsqlite3-legacy`) |
| `sqlite3-rsync-legacy` | The 10-library split (previously embedded the amalgamation; lost its single-threaded build, see SRS 001 §6) |
| `speedtest1-legacy` | The 10-library split (previously embedded the amalgamation) |
| `lemon-legacy`, `mkkeywordhash-legacy`, `mkopcodeh-legacy`, `mkopcodec-legacy` | Nothing — standalone host build tools, no `sqlite3` API dependency |

`libraries/libsqlite3-legacy` (the original, monolithic amalgamation build) still
exists unchanged, alongside the split — nothing in SRS 001 removes it.

## SRS documents

- [SRS 001](../srs/001-legacy-restructure-and-dynamic-linking.md) — legacy source
  consolidation, per-library C source split, and the dynamic-linking conversion.
  **Complete.**
- [SRS 002](../srs/002-cpp-conversion.md) — C to C++ conversion, applied per library
  once SRS 001 lands for it. **In progress: the first four dependency leaves,
  through `sqlite-backend-tree`, are complete.**
- [SRS 003](../srs/003-file-organization-based-on-cpp-namespacing.md) — namespace-mirrored
  file organization, applied per library once SRS 002 lands for it. **Not started.**
- [SRS 004](../srs/004-stl-based-architecture.md) — STL-based internals, applied per
  library once SRS 003 lands for it. **Not started.**
- [srs/index.md](../srs/index.md) — the index of all SRS documents and how they relate.

## Existing project documentation

- [doc/](../doc/) — legacy SQLite design notes (`wal-lock.md`,
  `pager-invariants.txt`, `vdbesort-memory.md`, etc.) describing invariants
  the restructured implementation must continue to uphold.
- [README.md](../README.md) — the original SQLite project README.
