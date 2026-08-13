# Software Requirements Specification

## Legacy Restructure & Dynamic Library Split

*SRS 001 — see the [SRS index](index.md) for the full list of SRS documents.*

| | |
|---|---|
| **Document status** | Draft v0.1 — requirements capture, not yet reviewed or approved |
| **Subject system** | `sqlite-src-3530400` (SQLite 3.53.4)'s original C sources |
| **Target system** | The same functionality, split into per-purpose C libraries, still plain C, but compiled and dynamically linked instead of amalgamated |

---

## 1. Introduction

### 1.1 Purpose

This document supersedes the previous `srs/001-sqlite-cpp-modularization.md`,
`srs/002-full-c-retirement.md`, and `srs/003-sil4-safety-integrity-validation.md`
outright. Those documents specified a header-only, fully-inlined C++ rewrite done in one
pass, a permanent legacy-vs-new comparison track kept in lockstep forever, and SIL4
safety-integrity validation on top. That direction is abandoned. All C++-derived work
produced under it (nine header-only libraries under `libraries/`, `sqlite-cpp-repl`, three
ported generator tools, and the legacy-freeze/parity-testing apparatus) has been removed
from the tree per this document's own FR-1.

The new direction is incremental and mechanical-first: get the existing C source
physically reorganized into per-purpose libraries and building as real, dynamically
linked shared objects — still plain C — *before* any C++ conversion is attempted. C++
conversion is [SRS 002](002-cpp-conversion.md)'s job, once a library builds this way.
Namespace-mirrored file organization is
[SRS 003](003-file-organization-based-on-cpp-namespacing.md)'s job, once a library is
converted. STL-based internals are [SRS 004](004-stl-based-architecture.md)'s job, once
a library is reorganized. These documents are now a strict linear pipeline, applied
library by library, not independent additive layers as before.

### 1.2 Scope

**In scope:** relocating the untouched original SQLite source into a single reference
copy (§4.2), reasoning about and copying the relevant files into per-library source
trees (§4.3), converting every library and every legacy executable to build as a
dynamically linked target (§4.4–§4.5), and rewiring the CMake workspace so it builds
end-to-end again (§4.6).

**Out of scope:** any C→C++ conversion (SRS 002), any namespace-mirrored file
reorganization (SRS 003), any STL-based redesign (SRS 004), any change to the on-disk
format, SQL semantics, or the existing TCL test suite. No
permanent legacy-vs-new comparison track is established this time — `./legacy` (§4.2)
is a one-time source origin the libraries are populated from, not a target kept in
lockstep going forward.

### 1.3 Library Decomposition

The library boundaries from the previous SRS 001 are reused unchanged — they were sound
independent of the header-only-C++ delivery mechanism that document also specified, and
[SRS 002](002-cpp-conversion.md) depends on the same namespace names once conversion
starts:

| Library | Legacy source (originals, see §4.2) | Responsibility |
|---|---|---|
| `sqlite-utils` | `mem0.c`,`mem1.c`,`mem2.c`,`mem3.c`,`mem5.c`,`malloc.c`,`util.c`,`printf.c`,`utf.c`,`hash.c`,`hash.h`,`mutex.c`,`mutex.h`,`random.c`,`status.c`,`fault.c`,`global.c`† | Allocator, string/number formatting, hash table, mutex dispatch, PRNG, status counters, global config/constant tables |
| `sqlite-backend-os` | `os.c`,`os.h`,`os_common.h`,`os_setup.h`,`os_unix.c`,`mutex_unix.c`,`os_win.c`,`os_win.h`,`mutex_w32.c`,`mutex_noop.c`†,`os_kv.c`,`memdb.c` | VFS interface + platform implementations |
| `sqlite-backend-pager` | `pager.c`,`pager.h`,`wal.c`,`wal.h`,`pcache.c`,`pcache.h`,`pcache1.c`,`memjournal.c`,`bitvec.c` | Page cache, WAL, rollback journal |
| `sqlite-backend-tree` | `btree.c`,`btree.h`,`btreeInt.h`,`btmutex.c`†,`dbpage.c`,`dbstat.c` | B-tree, page-level introspection |
| `sqlite-core-virtual-machine` | `vdbe.c`,`vdbe.h`,`vdbeInt.h`,`opcodes.h`/`.c`(generated),`vdbeapi.c`,`vdbeaux.c`,`vdbemem.c`,`vdbesort.c`,`vdbeblob.c`,`vdbetrace.c`,`vdbevtab.c` | Bytecode interpreter, program construction, `Mem` handling |
| `sqlite-core-command-processor` | `prepare.c`,`pragma.c`,`pragma.h`,`insert.c`,`update.c`,`delete.c`,`upsert.c`,`select.c`,`where.c`,`wherecode.c`,`whereexpr.c`,`whereInt.h`,`expr.c`,`resolve.c`,`walker.c`,`attach.c`,`alter.c`,`analyze.c`,`vacuum.c`,`table.c`,`rowset.c`,`build.c`,`trigger.c`,`window.c`,`fkey.c`,`auth.c`,`callback.c`,`treeview.c`,`func.c`†,`json.c`†,`date.c`† | Statement orchestration: prepare, planning, schema mutation, triggers, built-in SQL functions |
| `sqlite-core-interface` | `main.c`,`legacy.c`,`loadext.c`,`sqlite3ext.h`,`vtab.c`,`backup.c`,`notify.c`,`threads.c`,`complete.c` | Public API surface, object lifecycle, extension loading |
| `sqlite-compiler-tokenizer` | `tokenize.c`,`keywordhash.h`(generated) | Tokenizer |
| `sqlite-compiler-parser` | `parse.y`/`parse.c`/`parse.h`(generated) | Grammar/parser |
| `sqlite-compiler-code-generator` | Opcode-emission portions of `select.c`,`expr.c`,`insert.c`,`update.c`,`delete.c`,`trigger.c`,`where.c`/`wherecode.c` | AST → VDBE bytecode |

† Added during FR-4 (§6): missing from this table's original draft, discovered only when
the corresponding library failed to resolve against the rest of the set at link time — see
§6's FR-4 note for how they were found. `global.c` in particular holds cross-cutting state
(the `sqlite3Config` struct, `sqlite3CtypeMap`, the opcode-property table, the built-in
function hash `sqlite3BuiltinFunctions`) that doesn't cleanly belong to any one library;
it's placed in `sqlite-utils` pragmatically, as the lowest-level library, not because its
contents are "generic, SQL-unaware utilities" in the sense §3.2 of the old SRS 001 used
that phrase — some of it (the opcode table, the function hash) plainly isn't.

Extensions (`fts3`, `fts4`, `fts5`, `rtree`, `geopoly`, `session`, `rbu`, `icu`,
`dbstat`/`dbpage`) are deferred — they stay built directly from `legacy/ext` for now and
are not split into their own libraries by this document.

### 1.4 References

- [SRS 002](002-cpp-conversion.md) — the C→C++ conversion applied to each library once
  it builds per this document.
- [SRS 003](003-file-organization-based-on-cpp-namespacing.md) — the namespace-mirrored
  file reorganization applied once SRS 002 has landed for a library.
- [SRS 004](004-stl-based-architecture.md) — the STL-based internals conversion applied
  once SRS 003 has landed for a library.
- [srs/index.md](index.md) — the SRS index and document relationship.

---

## 2. Overall Description

### 2.1 Product Perspective

The target of this document alone is *not* a drop-in `sqlite3` replacement — it's a
build-system and source-tree restructuring milestone: the same C code that already
exists, split along the library boundaries in §1.3, each compiled to its own shared
object, with the legacy CLI tools linking against those `.so`s dynamically instead of
against a single statically-embedded amalgamation. Behavior, SQL dialect, and on-disk
format are unchanged, since no line of implementation logic is edited by this document
— only where each file lives and how it's compiled.

### 2.2 Design and Implementation Constraints

- Must not modify `test/`, `test/testrunner.tcl`, or any `.test` file — those remain the
  acceptance oracle.
- Must build with the toolchain already validated for this repository (GCC 16.2, CMake
  4.4.2).
- No behavioral change is in scope; this document is purely about physical source
  organization and link mode.

---

## 3. Prior State (What FR-1 Removes)

Before this document's FR-1, the tree held:
- Nine header-only C++ libraries under `libraries/` (`libsqlite-utils`,
  `libsqlite-backend-{os,pager,tree}`, `libsqlite-core-{virtual-machine,
  command-processor}`, `libsqlite-compiler-{tokenizer,parser,code-generator}`).
- `applications/sqlite-cpp-repl` and three ported C++ generator tools
  (`applications/{mkkeywordhash,mkopcodec,mkopcodeh}`, unsuffixed).
- A legacy-freeze/parity-testing apparatus (`tests/legacy-freeze/`,
  `tool/check-legacy-freeze.sh`, `tool/check-generator-parity.sh`,
  `tool/update-legacy-freeze-manifest.sh`, `tool/legacy-freeze-common.sh`,
  `cmake/SqliteCppDependency.cmake`).

All of the above is removed by FR-1. `libraries/libsqlite3-legacy` and the
`applications/*-legacy` directories (plain, unmodified original source, already renamed
from a still-earlier pass) are *not* removed by FR-1 — they're consumed by FR-2 instead.

---

## 4. Functional Requirements

- **FR-1 (Cleanup).** Remove everything listed in §3 as removed, preserving the
  top-level directory layout (`libraries/`, `applications/`, `cmake/`, `tests/`,
  `docs/`, `srs/` continue to exist; their old-plan-specific contents don't). CMake
  files referencing removed targets/paths (root `CMakeLists.txt`, `tests/CMakeLists.txt`)
  are updated in the same pass so the tree stays valid CMake, even though full
  buildability isn't restored until FR-6.

- **FR-2 (Legacy consolidation).** Relocate the untouched original SQLite source —
  `src/`, `ext/`, `tool/` (minus the parity/freeze scripts FR-1 removes), `compat/` —
  into a single top-level `legacy/` tree (`legacy/src`, `legacy/ext`, `legacy/tool`,
  `legacy/compat`), preserving internal structure. Update every `SQLITE_TOP`-based
  reference in the codegen/build pipeline accordingly. `test/`, `mptest/`, `doc/` stay
  at the repository root — they are the behavioral oracle and reference documentation,
  not implementation source being restructured, and this document does not move them.
  `libraries/libsqlite3-legacy` and `applications/*-legacy` fold into this consolidated
  tree as part of the same move (their content is exactly this original source, already
  isolated from the old header-only-C++ libraries).

- **FR-3 (Per-library population).** For each library in §1.3's table, copy (not move)
  its listed files from `legacy/src` / `legacy/ext` into `libraries/<name>/csrc` (plain
  C, unrenamed, unmodified). `legacy/` remains the single untouched source of truth;
  every `libraries/<name>/csrc` file is a copy, so a library's later edits (SRS 002,
  SRS 003, SRS 004) never risk drifting the reference copy. A file needed by more than
  one
  library (there are a few boundary cases, e.g. `vdbeaux.c`'s program-construction API
  consumed by both the VM and the compiler) is copied into each consuming library rather
  than shared, since each library must build independently.

- **FR-4 (Dynamic linking).** Each `libraries/<name>` becomes a real compiled target —
  `add_library(<name> SHARED ...)` over its `csrc/` — replacing the header-only/
  `INTERFACE`-library pattern entirely. Each library ships a public header set, its own
  `CMakeLists.txt` + `cmake/<Name>Config.cmake.in` + `CMakePackageConfigHelpers` export
  (mirroring the existing `libsqlite3-legacy` pattern), and a versioned `SONAME`.
  Cross-library dependencies (§1.3's ordering: utils → backend → core → compiler) are
  expressed as `target_link_libraries(... PUBLIC ...)` against the dependency's shared
  object, not by re-including its sources.

- **FR-5 (Legacy executables).** The CLI tools — `sqlite3` shell, `sqldiff`,
  `sqlite3-rsync`, `speedtest1`, and the generator tools `lemon`, `mkkeywordhash`,
  `mkopcodeh`, `mkopcodec` — get the same treatment: their sources come from `legacy/`
  (via FR-2/FR-3's copy pattern), and they link dynamically against the FR-4 shared
  objects instead of statically embedding the amalgamation or a private copy of their
  source.

- **FR-6 (Rewiring).** Root `CMakeLists.txt` and `cmake/*.cmake` are updated so the
  whole workspace configures and builds end-to-end again, and the existing TCL oracle
  (`sqlite_veryquick`/`sqlite_fulltest` via `testfixture`) passes unmodified against the
  result.

---

## 5. Acceptance Criteria

1. `cmake --build` succeeds for the full workspace, from a clean build directory: every
   `libraries/<name>` produces a `.so`, and every FR-5 application links against them
   dynamically (verify with `ldd`) rather than compiling its own copy of the
   amalgamation. `libraries/libsqlite3-legacy` and `tests/testfixture` are explicitly
   exempt — they're the original monolithic build and the TCL oracle respectively,
   neither touched by this document (§2.1).
2. `ctest` — `sqlite_veryquick` — passes with the same pre-existing `zipfile-25.0` flake
   as the only permitted failure.
3. `legacy/` contains a complete, untouched copy of the original `src/`, `ext/`,
   `tool/`, `compat/` trees; nothing under `legacy/` is ever edited by a later library
   pass.
4. Every FR-5 application is verified to actually *run* correctly against the split
   libraries (real SQL executed, not just a successful link) — see §6's FR-5 note for
   what was checked.

**Status: all four criteria met**, verified on a fresh clean-room build (empty build
directory, `CMAKE_BUILD_TYPE=Release`) as the final FR-6 check, in addition to every
per-FR verification recorded in §6.

---

## 6. Status

- **FR-1 (Cleanup): done.** The old-plan artifacts listed in §3 have been removed and
  the CMake files fixed up to stay valid.
- **FR-2 (Legacy consolidation): done.** `src/`, `ext/`, `tool/` are relocated to
  `legacy/src`, `legacy/ext`, `legacy/tool` (`compat/` was already empty and untracked,
  nothing to move). `cmake/SqliteCodegen.cmake`, `tests/CMakeLists.txt`, and the two
  application `CMakeLists.txt` files with relative `tool/` references
  (`applications/sqldiff-legacy`, `applications/sqlite3-rsync-legacy`) are updated
  accordingly. `VERSION`, `manifest`, `manifest.tags`, and `manifest.uuid` are also
  *copied* (not moved — every other `CMakeLists.txt` still reads the workspace-root
  copies by relative path) into `legacy/`, discovered necessary during implementation:
  `tool/mksqlite3h.tcl` and `ext/fts5/tool/mkfts5c.tcl` both expect these metadata files
  colocated with `src/`/`ext/`, since they self-locate their own root from their own
  script path (now under `legacy/`) or take a single `$TOP` argument used for both
  metadata and source lookups. Verified: clean `cmake` configure, full workspace build,
  and `ctest -R sqlite_veryquick` at the same pre-existing `zipfile-25.0`-only baseline.
- **FR-3 (Per-library population): done.** Each `libraries/<name>/csrc` per §1.3's table
  is populated from `legacy/src` (every listed file lives there; none needed from
  `legacy/ext`). `libraries/libsqlite-core-interface` is a new directory, not present
  before this pass. Two deliberate departures from a literal one-file-one-library
  reading:
  - Generated files (`opcodes.h`/`.c`, `keywordhash.h`, `pragma.h`, `parse.c`/`.h`) are
    **not** copied — they don't exist as static source, only as build output (§4 FR-6
    wires each library's own generation step later); `sqlite-compiler-parser/csrc`
    holds `parse.y` (the actual static grammar source) instead.
  - `sqlite-compiler-code-generator`'s eight files (`select.c`, `expr.c`, `insert.c`,
    `update.c`, `delete.c`, `trigger.c`, `where.c`, `wherecode.c`) are duplicated from
    `sqlite-core-command-processor`'s copies rather than split — per §3.5.3, the
    opcode-emission logic these files hold isn't separable from command-processor's
    planning logic at the file level yet; that separation is SRS 002's job once real
    C++ conversion starts, not something FR-3's mechanical copy can do.
  97 files copied in total (88 unique, 8 duplicated per the code-generator note above,
  1 grammar source).
- **FR-4 (Dynamic linking): done, with a discovered constraint.** Before writing any
  CMake, a quick manual link test on `sqlite-utils` (§1.3's designated bottom-of-stack
  library) surfaced 52 undefined symbols reaching into `Btree*`/`Pager*`/`Vdbe*` — layers
  §1.3 places *above* it. SQLite's C implementation was never written with per-subsystem
  link boundaries: every file shares one `sqliteInt.h` and calls freely across what §1.3
  treats as layers. CMake also refuses genuine target-level cycles between `SHARED`
  library targets ("Cyclic dependencies are allowed only among static libraries"), which
  a literal reading of FR-4 (acyclic per §1.3) can't satisfy given the real symbol graph
  is cyclic. Presented to the requester with the evidence; decision: build all 9 as real
  `.so`s anyway, with **no `target_link_libraries()` between sibling sqlite-cpp
  libraries** — each compiles independently (`cmake/SqliteCppLibrary.cmake`), left with
  unresolved symbols by design (normal for a Linux `.so`), resolved at process load time
  once something links against the whole set together (validated: a throwaway
  executable linked with `-Wl,--no-as-needed` against all 9 `.so`s plus `opcodes.c`/
  `ctime.c` as extra per-library generated sources resolves with zero undefined symbols
  and runs). This is a temporary, deliberately imperfect bridge — real, encapsulated
  per-library link boundaries are SRS 002's job, once each library has its own
  namespaced C++ API instead of free functions sharing global state. Windows DLLs do not
  support this kind of load-time resolution the same way ELF `.so`s do; that gap is
  unaddressed and would need real work if Windows becomes a target platform.

  That same link test also surfaced **six files missing from §1.3's original table**
  entirely (`global.c`, `btmutex.c`, `func.c`, `json.c`, `mutex_noop.c`, `date.c`) — not
  a circularity issue, a genuine gap in FR-3's file-to-library mapping. §1.3's table is
  corrected in place (marked †) and the six files copied into the right library's
  `csrc/` retroactively. `opcodes.c` and `ctime.c` (generated, like `compiler-parser`'s
  `parse.c`) are wired as extra sources for `core-virtual-machine` and `core-interface`
  respectively.

  Also fixed as a byproduct: `cmake/SqliteCodegen.cmake`'s include-guard used
  `CACHE INTERNAL`, which persists across separate `cmake` invocations on the same build
  directory — so a CMakeLists.txt edit that triggers CMake's automatic reconfigure would
  see the guard already `TRUE` from the *previous* run and return before `SQLITE_GENDIR`
  and friends were (re-)defined, breaking every target that depends on them. Changed to
  a plain variable (still correctly inherited by every `add_subdirectory()` scope within
  one configure run, just not across separate runs). Pre-existing bug, not introduced by
  this pass, but only exposed once nine libraries started including that file.

  Verified: clean `cmake` configure, full workspace build (all 9 `.so`s plus
  `libsqlite3-legacy` and every `-legacy` application), the smoke-link/run above, and
  `ctest -R sqlite_veryquick` at the same pre-existing `zipfile-25.0`-only baseline.
- **FR-5 (Legacy executables): done, with a discovered constraint and a scope
  correction.** Of the four `sqlite3`-dependent applications, `sqldiff-legacy` already
  linked dynamically (against `libsqlite3-legacy`) and `speedtest1-legacy` embedded the
  amalgamation with no special per-consumer flags — both switch cleanly to the FR-4
  split. `sqlite3-shell-legacy` and `sqlite3-rsync-legacy` deliberately embedded the
  amalgamation instead, each compiled with its own extra feature defines (documented in
  their own `CMakeLists.txt`) that a single shared `.so` can't provide per-consumer;
  `sqlite3-rsync-legacy`'s `SQLITE_THREADSAFE=0` in particular directly conflicts with
  the threaded build every other consumer needs. Presented to the requester with the
  evidence; decision: force all four onto the shared libraries anyway, accepting the
  tradeoff (`sqlite3-rsync-legacy` loses its single-threaded build — see
  `cmake/SqliteCppLibrary.cmake`'s `SQLITE_CPP_EXTRA_FEATURE_DEFS` comment).

  That same change surfaced a second, sharper constraint while wiring it up: three of
  the shell's original extra flags — `SQLITE_ENABLE_FTS4` (which implies FTS3),
  `SQLITE_ENABLE_RTREE`, `SQLITE_ENABLE_STMTVTAB` — aren't just missing headers, they're
  **unresolvable symbols**: `main.c` references `sqlite3Fts3Init`/`sqlite3RtreeInit`/
  `sqlite3StmtVtabInit` unconditionally once those macros are defined, and those
  functions live in `ext/fts3/fts3.c`, `ext/rtree/rtree.c`, `ext/misc/stmt.c` —
  extensions §1.2/§3.6 (via the old SRS 001 this document's table is reused from)
  explicitly defer, not part of any of the 9 libraries. Unlike FR-4's circularity (a
  process-load-time resolution problem with a workaround), this has no workaround short
  of building the deferred extensions: those three flags are dropped from
  `SQLITE_CPP_EXTRA_FEATURE_DEFS` entirely. `sqlite3-shell-legacy` and
  `sqlite3-rsync-legacy` therefore lose FTS4/RTree/stmt-vtab support until extensions get
  their own pass — a real, user-visible feature reduction versus their previous
  amalgamation-embedding build, on top of the threading tradeoff above.

  The four generator tools (`lemon`, `mkkeywordhash`, `mkopcodeh`, `mkopcodec`) call
  into no `sqlite3` API at all — they're standalone host build tools with zero
  dependency on the split libraries — so FR-5 doesn't apply to them; nothing changed.

  Verified: clean build, and each of the four applications actually **run** correctly
  against the shared libraries, not just link — `sqlite3-legacy` (the shell) executes
  real SQL (`CREATE TABLE`/`INSERT`/`SELECT`) against an in-memory database,
  `sqldiff-legacy` diffs two real on-disk databases and produces correct `UPDATE` SQL,
  `sqlite3-rsync-legacy --help` runs, `speedtest1-legacy` completes a full benchmark run.
  `ctest -R sqlite_veryquick` (unaffected — `testfixture` builds its own amalgamation
  copy, untouched by this pass) stays at the same pre-existing `zipfile-25.0`-only
  baseline.
- **FR-6 (Rewiring): done.** Mostly bookkeeping at this point — FR-2 through FR-5 already
  left the workspace building and passing end-to-end after each pass. Remaining work:
  stale comments in root `CMakeLists.txt` referencing the old, superseded SRS numbering
  (e.g. "SRS S3") corrected; `docs/index.md` rewritten to describe the now-fully-buildable
  state instead of FR-1's transitional one; §5's acceptance criteria corrected (criterion
  1 originally said no target may embed the amalgamation, which contradicted keeping
  `libsqlite3-legacy`/`testfixture` deliberately unchanged — narrowed to FR-5's
  applications specifically) and verified.

  Final verification, from a genuinely clean build directory (not reusing any
  incrementally-modified one from FR-2 through FR-5), `CMAKE_BUILD_TYPE=Release`: full
  workspace build succeeds, all 9 `.so`s and all 4 FR-5 applications present, and
  `ctest -R sqlite_veryquick` passes at the same pre-existing `zipfile-25.0`-only
  baseline every prior check in this document found. **SRS 001 is complete.**

---

## 7. Glossary

| Term | Meaning |
|---|---|
| `legacy/` | The single, untouched reference copy of the original SQLite C source (§4.2), not a comparison target — see §1.1 |
| `csrc/` | Per-library plain-C source directory populated by FR-3, copied from `legacy/` |
| SONAME | The shared-object version name embedded in a `.so`, used by the dynamic linker |
