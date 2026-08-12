# Software Requirements Specification

## SQLite C++ Modularization ("sqlite-cpp")

| | |
|---|---|
| **Document status** | Draft v0.1 — requirements capture, not yet reviewed or approved |
| **Subject system** | `sqlite-src-3530400` (SQLite 3.53.4), currently built via the CMake workspace described in `CMakeLists.txt` / `libraries/libsqlite3/` |
| **Target system** | A C++ reimplementation of the same functionality, decomposed into namespaced, header-only libraries |

---
## 1. Introduction

### 1.1 Purpose

This document specifies the requirements for refactoring SQLite's C implementation
(`src/`, plus the `ext/` extensions) into a modern C++ codebase organized as a set of
independent, namespaced, header-only libraries, while preserving the existing public
C API and behavior exactly. It is written for whoever plans, implements, or reviews
that refactor — most realistically a sequence of scoped engineering efforts rather
than a single change, given the size of the source (~154 files under `src/`, ~250K
lines including extensions).

### 1.2 Scope

**In scope:** the core SQLite engine (tokenizer, parser, code generator, virtual
machine, B-tree/pager/OS backend, public C API surface) and the bundled extensions
already wired into the existing CMake build (`fts3`, `fts4`, `fts5`, `rtree`,
`geopoly`, `session`, `rbu`, `icu`, `dbstat`/`dbpage`). Command-line and auxiliary
tools (`sqlite3` shell, `sqldiff`, `sqlite3-rsync`, `speedtest1`) are in scope as
consumers that must keep working, per §7.

**Out of scope:** the `ext/wasm` Emscripten build, `mptest`, `ext/jni`, and the TCL
test harness's own implementation (`test/*.test`, `test/tester.tcl`,
`test/testrunner.tcl`) — those are **used unmodified as the acceptance oracle** (§10)
and are not themselves refactored.

### 1.3 Definitions, Acronyms, Abbreviations

| Term | Meaning |
|---|---|
| VDBE | Virtual Database Engine — SQLite's bytecode interpreter |
| AST | Abstract Syntax Tree — parsed representation of a SQL statement |
| ODR | One Definition Rule (C++) |
| SRS | This document |
| header-only | A library whose entire implementation is reachable by including headers, with no separately compiled `.cpp` translation unit the consumer must link against |
| strangler-fig migration | Incrementally replacing a subsystem behind a stable interface, old and new coexisting until the old is fully retired |

### 1.4 References

- `CMakeLists.txt`, `cmake/SqliteFeatures.cmake`, `cmake/SqliteCodegen.cmake`,
  `libraries/libsqlite3/` — the existing CMake conversion of the original C build;
  this refactor's build system should extend that structure, not replace it.
- `/home/arthur/Projects/Software/opheap` — the reference project whose structure
  (`libraries/`, `applications/`, `tests/`, `cmake/<Name>Dependency.cmake`,
  `include/.../testing/*_test.hpp` self-registering test groups) this refactor's
  organization is modeled on, per the user's explicit instruction.
- `doc/` — existing SQLite design notes (`wal-lock.md`, `pager-invariants.txt`,
  `vdbesort-memory.md`, etc.), which document invariants the new implementation
  must continue to uphold even though the code implementing them will move.
- `test/` (1,190 `.test` files) and `test/testrunner.tcl` — the existing behavioral
  test suite, unmodified, used as the correctness oracle for this refactor.

### 1.5 Overview

§2 gives the overall shape of the target architecture and the constraints it must
satisfy. §3 breaks the system into libraries and states, for each, its
responsibility and the legacy files that map onto it. §4–§9 give specific,
numbered requirements (naming, API compatibility, testing, build, documentation,
tooling). §10 gives acceptance criteria. §11 gives constraints, risks, and open
issues — required reading before implementation starts, since several of the
source design goals are in tension with each other or with how C++ actually
behaves, and this section says explicitly how those tensions are resolved. §12
proposes a phased delivery plan, because attempting this as a single change is not
a credible plan for a codebase this size.

---

## 2. Overall Description

### 2.1 Product Perspective

The target is a drop-in replacement for the existing `sqlite3` shared/static
library: same `sqlite3.h` C API, same on-disk file format, same SQL dialect, same
extension ABI (`sqlite3ext.h`), verified against the same TCL test suite. Internally,
it is organized as C++ namespaces mirroring a new library decomposition (§3),
built header-only, with the original flat C API implemented as a thin `extern "C"`
façade over the namespaced C++ implementation (§5).

### 2.2 Design Goals (source requirements, as given)

The following are the design goals as specified by the requester, preserved here
verbatim as the authoritative input to this SRS, with typos in the source
numbering (a duplicated `1.2` and `1.6`) left resolved by context rather than by
renumbering the requester's list:

1. Convert the C implementation to C++, with:
   - The library split into **sqlite-utils**, **sqlite-backend** (further split into
     **sqlite-backend-os**, **sqlite-backend-pager**, **sqlite-backend-tree**),
     **sqlite-core** (further split into **sqlite-core-virtual-machine**,
     **sqlite-core-command-processor**, **sqlite-core-interface**), and
     **sqlite-compiler** (further split into **sqlite-compiler-tokenizer**,
     **sqlite-compiler-parser**, **sqlite-compiler-code-generator**).
   - `sqlite-utils` containing only generic utilities.
   - Every library namespaced by name, e.g. `sqlite::core::command_processor`.
   - Every library defining a nested `testing` namespace
     (e.g. `sqlite::core::command_processor::testing`), following the pattern
     already used by the `opheap` project.
   - Every source file header-only and `inline`; every function `inline`.
   - The original C API preserved exactly, re-exposed through the new namespaces.
   - One source file per declared function or type.
   - Preprocessor constant `#define`s replaced by enumerations.
   - Preprocessor conditional compilation replaced by two independent
     implementations selected by a CMake `if()`, rather than `#ifdef`.
   - Independent documentation per library, with a root-level documentation index
     (for GitHub Pages) linking to each library's own docs plus the main docs.
   - An independent CLI utility application per existing `sqlite3` utility,
     linked against the new library.
   - Test groups named after the element under test.

Each of these is restated as one or more concrete, numbered requirements in §4–§9,
with the interpretation this SRS uses made explicit wherever the source wording
admits more than one reading (flagged inline and cross-referenced to §11).

### 2.3 User Classes

- **C API consumers** — existing applications linking `sqlite3.h`/`libsqlite3`;
  must observe zero behavioral or ABI change.
- **C++ API consumers** (new) — code that can link directly against the namespaced
  libraries (e.g. `sqlite::backend::tree`) instead of the C façade, for use cases
  like embedding just the B-tree layer.
- **Extension authors** — code against `sqlite3ext.h`; must be unaffected.
- **Contributors** — implementing and reviewing this refactor; the primary
  audience for §3, §4, and §12.

### 2.4 Design and Implementation Constraints

- C++20 minimum (needed for `concept`s used in §4.4 to replace preprocessor
  interface contracts, and for `consteval`/`constexpr` used in enum-to-string
  and bitmask-enum helpers, §4.6).
- Must build with the toolchain already validated for this repository: GCC 16.2,
  CMake 4.4.2 (per the earlier CMake conversion's checkpoint).
- Must not modify `test/`, `test/testrunner.tcl`, or any `.test` file: those are
  the acceptance oracle (§10), not an implementation target.
- On-disk file format (page layout, WAL format, journal format) is unchanged —
  this is a re-implementation of behavior, not a format redesign.

### 2.5 Assumptions and Dependencies

- The CMake workspace already produced (`CMakeLists.txt`, `cmake/SqliteFeatures.cmake`,
  `cmake/SqliteCodegen.cmake`, `libraries/libsqlite3/`) remains the build-system
  foundation; this refactor adds new `libraries/*` targets alongside it rather than
  discarding it, mirroring how that CMake work itself was added alongside the
  original C sources rather than physically relocating `src/`/`ext/`.
- `opheap`'s structural conventions (per-library `cmake/<Name>Config.cmake.in`,
  a shared `cmake/<Name>Dependency.cmake` resolver, `testing/*_test.hpp`
  self-registering test headers, one generated `all_tests.cpp` per test binary)
  are assumed available as a pattern to follow, not as a shared dependency to link
  against.

---

## 3. System Architecture — Library Decomposition

### 3.1 Top-Level Libraries and Dependency Order

Dependencies must be acyclic. The required order (a library may depend only on
libraries to its left) is:

```
sqlite-utils
     │
     ├──────────────┬──────────────┐
     ▼              ▼              ▼
sqlite-backend-os  sqlite-backend-pager  sqlite-backend-tree
     └──────────────┴──────────────┘
                     │  (sqlite-backend, collectively)
                     ▼
     ┌───────────────────────────────┐
     │        sqlite-core            │
     │  virtual-machine ◄── command-processor ◄── interface │
     └───────────────┬───────────────┘
                      │
     ┌────────────────────────────────┐
     │          sqlite-compiler        │
     │  tokenizer → parser → code-generator │
     └──────────────────────────────────┘
```

`sqlite-compiler-code-generator` depends on `sqlite-core-virtual-machine` (it emits
VDBE opcodes) and on `sqlite-compiler-parser`'s AST types. `sqlite-core-interface`
(the public API surface) depends on everything below it — it is the composition
root. `sqlite-core-command-processor` depends on `sqlite-compiler` (it drives
parsing and code generation) and on `sqlite-backend` (it drives execution).
Extensions (§3.5) depend only on `sqlite-core-interface`'s public extension API,
exactly as they do today via `sqlite3ext.h`.

### 3.2 `sqlite-utils`

Generic, SQL-unaware utilities used across every other library. No dependency on
any other `sqlite-*` library.

| Namespace | Legacy source | Responsibility |
|---|---|---|
| `sqlite::utils::memory` | `mem0.c`, `mem1.c`, `mem2.c`, `mem3.c`, `mem5.c`, `malloc.c` | Pluggable allocator interface + built-in allocators |
| `sqlite::utils::text` | `util.c`, `printf.c`, `utf.c` | String/number parsing, `mprintf`-style formatting, UTF-8/16 conversion |
| `sqlite::utils::hash` | `hash.c`, `hash.h` | Generic hash table |
| `sqlite::utils::mutex` | `mutex.c`, `mutex.h` | Mutex *interface* and dispatch only — OS-specific bodies live in `sqlite-backend-os` (§3.3.1), per requirement 1.11 |
| `sqlite::utils::random` | `random.c` | PRNG |
| `sqlite::utils::status` | `status.c` | Runtime status-counter registry |
| `sqlite::utils::testing` | `fault.c` | Fault-injection helpers, reclassified under `testing` rather than shipped as production surface |

### 3.3 `sqlite-backend`

#### 3.3.1 `sqlite-backend-os`

| Namespace | Legacy source | Responsibility |
|---|---|---|
| `sqlite::backend::os` | `os.c`, `os.h`, `os_common.h`, `os_setup.h` | VFS interface, dispatch |
| `sqlite::backend::os::unix` | `os_unix.c`, `mutex_unix.c` | POSIX implementation — one of the two CMake-selected variants (requirement 1.11's paradigm case) |
| `sqlite::backend::os::windows` | `os_win.c`, `os_win.h`, `mutex_w32.c` | Win32 implementation — the other variant |
| `sqlite::backend::os::memory` | `os_kv.c`, `memdb.c` | In-memory VFS |

#### 3.3.2 `sqlite-backend-pager`

| Namespace | Legacy source | Responsibility |
|---|---|---|
| `sqlite::backend::pager` | `pager.c`, `pager.h` | Page cache ↔ disk, journal/WAL coordination |
| `sqlite::backend::pager::wal` | `wal.c`, `wal.h` | Write-ahead log |
| `sqlite::backend::pager::cache` | `pcache.c`, `pcache.h`, `pcache1.c` | Page cache |
| `sqlite::backend::pager::journal` | `memjournal.c`, `bitvec.c` | Rollback journal support |

#### 3.3.3 `sqlite-backend-tree`

| Namespace | Legacy source | Responsibility |
|---|---|---|
| `sqlite::backend::tree` | `btree.c`, `btree.h`, `btreeInt.h` | B-tree implementation |
| `sqlite::backend::tree::introspection` | `dbpage.c`, `dbstat.c` | `sqlite_dbpage`/`dbstat` virtual tables (page-level introspection, layered directly on the tree) |

### 3.4 `sqlite-core`

#### 3.4.1 `sqlite-core-virtual-machine`

| Namespace | Legacy source | Responsibility |
|---|---|---|
| `sqlite::core::virtual_machine` | `vdbe.c`, `vdbe.h`, `vdbeInt.h`, `opcodes.h/.c` (generated) | The bytecode interpreter loop and opcode table — runtime only |
| `sqlite::core::virtual_machine::api` | `vdbeapi.c` | `sqlite3_stmt` step/bind/column API |
| `sqlite::core::virtual_machine::aux` | `vdbeaux.c`, `vdbemem.c`, `vdbesort.c`, `vdbeblob.c`, `vdbetrace.c`, `vdbevtab.c` | Program construction helpers, `Mem` cell handling, external merge sort, incremental blob I/O |

`vdbeaux.c`'s program-*construction* API (`sqlite3VdbeAddOp*`, `sqlite3VdbeMakeReady`)
is a builder consumed by `sqlite-compiler-code-generator`, not by the runtime loop;
its declaration therefore lives in `sqlite-core-virtual-machine` (it operates on
VM-owned types) but its only caller is the compiler. This split is called out
explicitly because it is the sharpest boundary in the whole decomposition — see
§11.2.

#### 3.4.2 `sqlite-core-command-processor`

The statement-orchestration layer: turns a validated AST into schema changes
and/or a request to the code generator, and owns planning decisions.

| Namespace | Legacy source |
|---|---|
| `sqlite::core::command_processor::prepare` | `prepare.c` |
| `sqlite::core::command_processor::pragma` | `pragma.c`, `pragma.h` |
| `sqlite::core::command_processor::dml` | `insert.c`, `update.c`, `delete.c`, `upsert.c` |
| `sqlite::core::command_processor::query` | `select.c` (planning portions — see §11.2), `where.c`, `wherecode.c`, `whereexpr.c`, `whereInt.h` |
| `sqlite::core::command_processor::expr` | `expr.c`, `resolve.c`, `walker.c` |
| `sqlite::core::command_processor::schema` | `attach.c`, `alter.c`, `analyze.c`, `vacuum.c`, `table.c`, `rowset.c` (schema-mutation portions of `build.c` — see §11.2) |
| `sqlite::core::command_processor::trigger` | `trigger.c` |
| `sqlite::core::command_processor::window` | `window.c` |
| `sqlite::core::command_processor::fkey` | `fkey.c` |
| `sqlite::core::command_processor::auth` | `auth.c`, `callback.c` |
| `sqlite::core::command_processor::debug` | `treeview.c` |

#### 3.4.3 `sqlite-core-interface`

The public C API surface and top-level object lifecycle — the composition root.

| Namespace | Legacy source |
|---|---|
| `sqlite::core::interface` | `main.c`, `legacy.c` |
| `sqlite::core::interface::extension` | `loadext.c`, `sqlite3ext.h` |
| `sqlite::core::interface::vtab` | `vtab.c` |
| `sqlite::core::interface::backup` | `backup.c` |
| `sqlite::core::interface::notify` | `notify.c` |
| `sqlite::core::interface::threads` | `threads.c` |
| `sqlite::core::interface::text` | `complete.c` (`sqlite3_complete`) |

### 3.5 `sqlite-compiler`

#### 3.5.1 `sqlite-compiler-tokenizer`

| Namespace | Legacy source |
|---|---|
| `sqlite::compiler::tokenizer` | `tokenize.c`, `keywordhash.h` (generated) |

#### 3.5.2 `sqlite-compiler-parser`

| Namespace | Legacy source |
|---|---|
| `sqlite::compiler::parser` | `parse.y`/`parse.c`/`parse.h` (generated, lemon grammar) |
| `sqlite::compiler::parser::ast` | AST-construction portions of `build.c` (`sqlite3StartTable`, `sqlite3CreateIndex`, etc. — validation and node construction; see §11.2 for the split with `command_processor::schema`) |

#### 3.5.3 `sqlite-compiler-code-generator`

| Namespace | Legacy source |
|---|---|
| `sqlite::compiler::code_generator` | Opcode-emission portions of `select.c`, `expr.c`, `insert.c`, `update.c`, `delete.c`, `trigger.c`, `where.c`/`wherecode.c` |

This is the single largest and riskiest extraction in the whole plan: in the
existing code, "decide what to do" and "emit the opcodes to do it" are
interleaved statement-by-statement, not separated into distinct passes. §11.2
gives the required resolution strategy.

### 3.6 Extensions

`fts3`, `fts4`, `fts5`, `rtree`, `geopoly`, `session`, `rbu`, `icu`, and the
`dbstat`/`dbpage` virtual tables already live in `ext/` as self-contained modules
built atop the public C API/`sqlite3ext.h`. They are **not** subject to the
file-per-symbol or full-inlining requirements of §4 — they are ported to C++ and
namespaced (`sqlite::extensions::fts5`, `sqlite::extensions::rtree`, etc.) with
their own `testing` namespace and documentation, but as a lighter-weight pass than
the core, since they already sit behind a stable ABI boundary and are already
independently toggleable (per the existing `SQLITE_ENABLE_*` CMake options).

---

## 4. Naming, Namespacing, and Source Organization Requirements

- **FR-1.** Every library's C++ implementation lives under a namespace matching
  its position in §3's table exactly, e.g. `sqlite::core::command_processor`,
  `sqlite::backend::os::unix`.
- **FR-2.** Every library defines a nested `testing` namespace
  (`sqlite::<path>::testing`) holding that library's test code, mirroring
  `opheap`'s `testing/*_test.hpp` convention: test headers are colocated with the
  code they test, under a `testing/` subdirectory of the library's `include/`
  tree, and self-register their test group when included.
- **FR-3.** Every library is header-only: all declarations and definitions are
  reachable via `#include`, with function definitions marked `inline` (free
  functions) or implicitly inline (class member functions defined in-class,
  `constexpr`/`consteval` functions). See §11.1 for what `inline` does and does
  not guarantee, and why this requirement should be read as "ODR-safe to include
  from multiple translation units," not "the compiler is forced to inline calls."
- **FR-4.** One header per declared function or type, as the default rule.
  §11.3 states the specific, narrow exception this SRS requires (private
  helper functions used by exactly one public entity may share that entity's
  file) and why the literal, unqualified reading of this requirement is
  rejected.
- **FR-5.** `#define`-based constants are replaced by `enum class` (scoped
  enumerations). Where the legacy constant is a bitmask meant to be combined
  with `|`/`&` (e.g. `SQLITE_OPEN_*` flags, VDBE opcode flags), the enum must
  additionally define `operator|`, `operator&`, and `operator~` for that enum
  type (the standard C++ scoped-bitmask-enum pattern), so callers keep
  bitwise-flag ergonomics without reverting to unscoped enums or raw ints.
- **FR-6.** Preprocessor conditional compilation for **platform-family**
  branching (the unix/Windows split throughout `os_unix.c`/`os_win.c`,
  `mutex_unix.c`/`mutex_w32.c`) is replaced by two separate header files
  implementing a common interface, selected via `if(WIN32) ... else() ... endif()`
  in CMake (`target_sources`), not `#ifdef`. §11.4 states which existing
  `#ifdef` usages this requirement does and does not cover.

---

## 5. API Compatibility Requirements

- **FR-7.** `sqlite3.h`'s public C API is preserved byte-for-byte: same function
  signatures, same struct layouts, same constant values, same `SQLITE_VERSION`
  semantics. It is implemented as an `extern "C"` façade (a thin compatibility
  library, e.g. `libraries/libsqlite-c-api/`) that forwards each `sqlite3_*` call
  into the corresponding namespaced C++ entry point in `sqlite::core::interface`.
- **FR-8.** `sqlite3ext.h`'s extension ABI is likewise unchanged; existing
  loadable extensions must load and function against the new library without
  recompilation.
- **FR-9.** On-disk formats (database file, WAL, journal) are unchanged; a
  database file written by the existing C `sqlite3` must open correctly under
  the new implementation and vice versa.
- **FR-10.** The existing `test/` TCL suite (`testfixture`, `test/testrunner.tcl`)
  must run unmodified against the new implementation and pass with no new
  failures relative to the baseline already established in the CMake conversion
  (that baseline includes exactly one known pre-existing flake, `zipfile-25.0`,
  which is not a target of this refactor to fix).

---

## 6. Testing Requirements

- **FR-11.** Every library ships its own test group, named after the element
  under test (e.g. the test group for `sqlite::backend::tree::Cursor` is named
  `Cursor`, not `BtreeTests` or similar), following `opheap`'s pattern of one
  `_test.hpp` per tested header, each self-registering via inclusion into a
  generated `all_tests.cpp`.
- **FR-12.** Each library's test binary is runnable independently
  (`ctest -R <library>`), in addition to being aggregated into a workspace-wide
  run — mirroring the granularity `opheap`'s `tests/CMakeLists.txt` glob-and-
  generate approach already provides.
- **FR-13.** New C++-level unit tests are additive to, not a replacement for,
  the existing TCL behavioral suite (FR-10); the TCL suite remains the
  authoritative correctness oracle for SQL-visible behavior for the duration of
  this refactor (§12).

---

## 7. Build System Requirements

- **FR-14.** The build system is CMake, extending — not replacing — the
  workspace already established in `CMakeLists.txt`, `cmake/SqliteFeatures.cmake`,
  and `cmake/SqliteCodegen.cmake`. New libraries are added under `libraries/`
  following that same per-library `CMakeLists.txt` + `cmake/<Name>Config.cmake.in`
  + `CMakePackageConfigHelpers` export pattern already used for `libsqlite3`
  and modeled on `opheap`'s `libopheap-core`.
- **FR-15.** A shared `cmake/SqliteCppDependency.cmake` resolver (mirroring
  `cmake/SqliteDependency.cmake` and `opheap`'s `OpheapDependency.cmake`)
  provides one `sqlite_cpp_require_<library>()` function per library, so any
  consumer (an application, a test binary, another library) can depend on it
  whether built in-workspace, via `add_subdirectory` fallback, or via an
  installed `find_package`.
- **FR-16.** For each existing CMake-selected platform/feature variant (§4,
  FR-6), the two implementation files are separate `target_sources()` entries
  chosen by a CMake `if()`, never by a preprocessor `#ifdef` inside a shared
  file.

---

## 8. Documentation Requirements

- **FR-17.** Every library has its own documentation, independent of the main
  project documentation, stored under that library's own directory (e.g.
  `libraries/libsqlite-backend-tree/docs/`).
- **FR-18.** A documentation index exists at the repository root (per the
  requester's explicit instruction, for GitHub Pages to publish both the source
  and every library's independent documentation from one root). The main
  project documentation links out to each library's documentation rather than
  duplicating it.
- **FR-19.** Each library's documentation covers, at minimum: its namespace and
  public API surface, its position in the dependency graph (§3.1), the legacy
  source files it replaces (traceable back to this SRS's §3 tables), and how to
  build and run its tests independently (FR-12).

---

## 9. Tooling Requirements

- **FR-20.** For every existing `sqlite3`-derived command-line utility
  (`sqlite3` shell, `sqldiff`, `sqlite3-rsync`, `speedtest1`, and any future
  equivalents), an independent application target exists under `applications/`,
  linked against the new libraries (directly against the relevant namespaced
  library where practical, or against the `sqlite3` C-API façade where the
  tool is inherently written against the C API, as the existing `sqlite3` shell
  is — see §11.5).

---

## 10. Acceptance Criteria

1. `cmake --build` succeeds for the full workspace (existing C targets plus all
   new C++ targets) with no `#ifdef`-based platform/feature branching remaining
   in any new C++ source, per FR-6/FR-16.
2. `ctest` — the existing `sqlite_veryquick` target plus every new per-library
   C++ test target — passes, with the sole permitted pre-existing failure being
   `zipfile-25.0` (FR-10).
3. `sqlite_fulltest` (the full 1,190-file TCL suite) completes with no new
   failures relative to the pre-refactor baseline.
4. `speedtest1` shows no statistically significant regression against the
   pre-refactor C build (a specific threshold, e.g. ≤5%, is an open item for
   whoever approves this SRS to set — see §11.6).
5. An existing loadable extension or client application built against
   `sqlite3.h`/`sqlite3ext.h` continues to compile and run unmodified (FR-7,
   FR-8).
6. GitHub Pages, built from the root documentation index (FR-18), renders both
   the top-level docs and every library's independent documentation.

---

## 11. Constraints, Risks, and Open Issues

This section is required reading before implementation starts. Several of the
source design goals, read literally, are either self-contradictory, technically
mistaken about what C++ guarantees, or carry a cost the requester should
explicitly accept (or adjust) before work begins.

### 11.1 "Every function should be inlined" does not mean what it sounds like

The C++ `inline` keyword does **not** instruct the compiler to inline a call at
its call site — modern compilers make that decision themselves based on
optimization heuristics, largely ignoring the keyword for that purpose.
`inline`'s actual, load-bearing effect is exempting a function from the One
Definition Rule so it can be defined identically in every translation unit that
includes its header — which is precisely what makes a header-only library
possible. This SRS (FR-3) requires `inline` for that ODR reason. It does **not**
require (and implementation should not attempt) forcing real inlining of large,
hot-path functions like the VDBE interpreter's opcode-dispatch loop
(`sqlite::core::virtual_machine`'s main loop) — doing so would very likely
*hurt* performance (instruction-cache pressure) and directly threatens
acceptance criterion 4. If the requester's intent was performance-motivated
rather than header-only-motivated, that is a different, narrower requirement
(`[[gnu::always_inline]]`/`__forceinline` on specific, measured hot functions)
and should be raised as a separate, explicit item — not assumed.

### 11.2 Code generation cannot be cleanly separated from command processing without a deeper redesign than "move some functions"

In the existing code, `select.c` (and `insert.c`, `update.c`, `delete.c`,
`where.c`) simultaneously decide *what* a statement means (name resolution,
query planning) and emit VDBE opcodes to do it, statement-by-statement, in the
same functions. §3.4.2/§3.5.3's split (`command_processor` decides,
`code_generator` emits) is achievable, but only by introducing an intermediate
representation — a builder/visitor API that `command_processor` calls into
rather than calling `sqlite3VdbeAddOp*` directly — which is itself new design
work, not a mechanical file move. This is the single highest-effort, highest-
risk part of this refactor and should be scheduled as its own phase (§12) with
its own design review, not folded into the initial library-scaffolding pass.
The same tension exists, at smaller scale, between `sqlite-compiler-parser`'s
AST-construction and `sqlite-core-command-processor`'s schema-mutation halves
of `build.c` (§3.5.2).

### 11.3 "One source file per function or type," read literally, does not scale to this codebase

`src/` alone has ~154 files; a strict one-header-per-function reading would
produce several thousand headers for the core alone, before extensions. This
SRS's working rule (FR-4) is: one header per *public* function or type is
required; private helper functions used by exactly one public entity may be
defined in that entity's own header, to avoid a build graph and IDE experience
that becomes actively hostile to navigation at this scale. If the requester
wants the fully literal reading (including private helpers), that should be
confirmed explicitly, since it changes the file count by roughly an order of
magnitude.

### 11.4 The "two-implementation CMake `if()`" pattern applies cleanly to platform forks, not to feature flags

`os_unix.c` vs. `os_win.c` (and `mutex_unix.c` vs. `mutex_w32.c`) are true binary
forks of the same interface and are the paradigm case FR-6 targets. Most of the
rest of the codebase's `#ifdef`s are **feature flags** with more than two states
or with a default/omitted-feature asymmetry (`SQLITE_ENABLE_FTS5`,
`SQLITE_OMIT_*`) — these are already handled, in the existing CMake conversion,
by compiling the feature's source unconditionally and gating it with a
`target_compile_definitions()` flag (see `cmake/SqliteFeatures.cmake`), which is
the correct existing pattern and is retained rather than replaced. FR-6 is
scoped to platform-family branching specifically; feature-flag `#ifdef`s inside
a single function body (there are some, e.g. debug/trace instrumentation) are an
open item — recommend resolving them with a compile-time `if constexpr` over a
`concept`-constrained trait rather than a file split, since a file split does
not make sense for a three-line conditional.

### 11.5 The public C API façade means most tooling stays a C-API consumer, not a namespaced-library consumer

`sqldiff`, `sqlite3-rsync`, and the `sqlite3` shell are, in upstream SQLite,
deliberately written against the public C API (the shell in particular embeds
its own amalgamation compile for compile-time feature control — see the
existing CMake conversion's `applications/sqlite3-shell/CMakeLists.txt`). FR-20
does not require rewriting these tools against the internal namespaced C++
libraries; it requires that they continue to link and work against the C façade
(FR-7). A tool written directly against, say, `sqlite::backend::tree` would be
a new, deliberately C++-native tool, not a port of an existing one, and is out
of scope unless separately requested.

### 11.6 Performance acceptance threshold is unset

Acceptance criterion 4 needs a concrete regression threshold before it is
testable. This SRS does not set one; whoever approves this document should.

### 11.7 Effort and scope reality check

This refactor touches essentially the entire SQLite core (~154 files, on the
order of 150K+ lines excluding extensions) plus ~10 extensions, under
constraints (full header-only, file-per-symbol, exact API preservation, zero
behavioral regression against a 1,190-file test suite) that are individually
reasonable but collectively make this one of the larger C-to-C++ modularization
efforts realistically undertaken on a codebase of this maturity. §12's phased
plan is not optional scaffolding — attempting this as one change is not a
credible plan, and no phase should begin without the previous phase's
acceptance criteria (§10, scoped per-phase) passing first.

---

## 12. Phased Delivery Plan

Each phase ends with the existing TCL suite (FR-10) still passing against a
build where that phase's libraries are live and everything downstream of them
still calls into the legacy C for anything not yet migrated (strangler-fig:
the C and C++ implementations coexist, selected at the smallest practical
granularity, until a phase's C sources are fully retired).

1. **Scaffolding.** `sqlite-utils` (no dependencies, lowest risk, exercises
   FR-1–FR-6 and the documentation/testing conventions once, as the template
   for every later phase).
2. **Backend.** `sqlite-backend-os` (the FR-6 paradigm case), then
   `sqlite-backend-pager`, then `sqlite-backend-tree`, each verified against
   the existing pager/B-tree invariant docs in `doc/`.
3. **Virtual machine.** `sqlite-core-virtual-machine`, the runtime half only
   (§3.4.1) — deliberately *before* the compiler, so it can be validated by
   feeding it bytecode still generated by the legacy C code generator during
   this phase.
4. **Compiler and command processor, together.** Tokenizer and parser first
   (lower risk), then the code-generator/command-processor split (§11.2) as
   its own reviewed sub-phase — this is the phase most likely to need its
   timeline revisited once the intermediate-representation design lands.
5. **Interface façade.** `sqlite-core-interface` plus the `extern "C"` FR-7
   compatibility layer, cutting the public `sqlite3.h` over to the new
   implementation.
6. **Extensions.** Per §3.6, each extension ported independently once §5 is
   stable.
7. **Tooling and documentation.** FR-20 applications and the FR-17/FR-18
   documentation set, closing out once every earlier phase's libraries exist
   to document and link against.

---

## 13. Glossary

See §1.3 for acronyms. Namespace, library, and legacy-file terminology is
defined at first use in §3's tables, which are the authoritative mapping this
document establishes between the old codebase and the new one.
