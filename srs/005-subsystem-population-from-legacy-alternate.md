# Software Requirements Specification

## Subsystem Population from `sqlite3-legacy-alternate` & the `sqlite3_state` Umbrella

*SRS 005 — see the [SRS index](index.md) for the full list of SRS documents.*

| | |
|---|---|
| **Document status** | Draft v0.1 — implemented and verified against a clean build; not yet formally reviewed |
| **Subject system** | `libraries/sqlite3-legacy-alternate` (a one-symbol-per-file decomposition of the SQLite 3.53.4 amalgamation, maintained as its own project inside this workspace) |
| **Target system** | The ten SRS-001 §1.3 subsystem libraries (`sqlite3-utils`, `sqlite3-backend-{os,pager,tree}`, `sqlite3-core-{virtual-machine,command-processor,interface}`, `sqlite3-compiler-{tokenizer,parser,code-generator}`), populated from that decomposition instead of whole legacy files, plus a new umbrella library, `libraries/sqlite3`, that links all ten and exposes a `sqlite3_state`-named public interface |

---

## 1. Introduction

### 1.1 Purpose

SRS 001 populated the ten subsystem libraries by copying *whole original SQLite files* into each library's `csrc/` (§4 FR-3 of that document). By the time this document starts, that population has been reset — every one of the ten libraries held nothing but an empty `csrc/stub.c` placeholder, and `libraries/sqlite3` was an equally empty stub. In its place, `libraries/sqlite3-legacy-alternate` had been grown into a separate, self-contained project: the same SQLite source pulled apart into 262 `.c`/322 `.h` files, one container per struct, function, or typedef, already building cleanly as its own shared library (see that project's own `README.md` and `docs/architecture.md`).

This document specifies (and, per its Status section, implements) using that finer-grained decomposition as the new source of truth for the ten subsystem libraries — replacing SRS-001 FR-3's whole-file-copy population method for these libraries specifically — and adds the piece SRS 001 never had: a real umbrella library, `libraries/sqlite3`, that links all ten together and presents a single public entry point renamed from `sqlite3` to `sqlite3_state`.

This document does not amend SRS 001's own text or its §1.3 library responsibility table — it reuses that table's ten library names and boundaries unchanged, and reuses SRS 001 FR-4's already-established build pattern (each subsystem library is an independently-compiled `SHARED` target with no `target_link_libraries()` between siblings, symbols resolved only once something links the whole set together). What changes is *what fills each library's `csrc/`* and *what finally does that whole-set linking* — SRS 001 never got as far as building a real umbrella; this document adds one.

### 1.2 Scope

**In scope:**
- Categorizing every container in `sqlite3-legacy-alternate/src/sqlite` into one of the ten SRS-001 §1.3 subsystem libraries (§4 FR-1), by the methodology in §3.
- Populating each subsystem library's `csrc/` and `include/sqlite/` from that categorization, copying (not moving) — `sqlite3-legacy-alternate` remains untouched, playing the same "single source of truth" role `legacy/` played for SRS 001 FR-3 (§4 FR-1).
- Wiring each subsystem library's `CMakeLists.txt` to build as a real `SHARED` target with cross-library header visibility but no cross-library linking, reusing SRS 001 FR-4's constraint (§4 FR-2).
- Adding `libraries/sqlite3` as a real umbrella target that links all ten subsystem libraries together and resolves the whole symbol graph (§4 FR-3).
- Exposing a `sqlite3_state`-named public interface from that umbrella, renaming `struct sqlite3` at the API boundary (§4 FR-4, design in §3.3).
- Getting `applications/example-002` — already checked into the tree expecting `sqlite/sqlite3_state.h` and a `sqlite3_state_*`-prefixed API — building and running against the new umbrella (§4 FR-5).

**Out of scope:**
- Any C→C++ conversion (still SRS 002's job, once a library reaches this document's end state).
- Splitting SQLite's official extensions (FTS3/4/5, RTree, session, RBU, ICU, etc.) into their own libraries — SRS 001 §1.3 already deferred these, and this document extends that deferral: any `sqlite3-legacy-alternate` container whose name matches a known extension is excluded from every subsystem library rather than guessed into one (§3.4).
- Function-level re-splitting of any `sqlite3-legacy-alternate` container. This document assigns whole containers to libraries; where a container genuinely mixes concerns from more than one subsystem (§3.5 documents the ones found), the whole container goes to its dominant subsystem rather than being torn apart by hand. That finer split, if ever wanted, is exactly the kind of mechanical pass `sqlite3-legacy-alternate/tools/clang-refactor/` already does for other transformations — not something this document attempts by hand.
- A literal, tree-wide rename of the `sqlite3` struct tag. §3.3 explains why the boundary-level alias chosen here is a deliberate, narrower substitute.

### 1.3 References

- [SRS 001](001-legacy-restructure-and-dynamic-linking.md) — defines the ten subsystem libraries' names, responsibilities, and the no-sibling-linking build constraint this document reuses unchanged.
- `libraries/sqlite3-legacy-alternate/README.md` and `docs/architecture.md` — the one-symbol-per-file decomposition this document draws from.
- [srs/index.md](index.md) — the SRS index and document relationship.

---

## 2. Overall Description

### 2.1 Product Perspective

Before this document, `libraries/sqlite3-legacy-alternate` and the ten SRS-001 subsystem libraries were two unrelated things sitting side by side in the same workspace: the former a complete, working, single-`.so` build of SQLite; the latter ten empty shells. `applications/example-001` linked directly against `sqlite3-legacy-alternate` and worked; `applications/example-002` was scaffolded against a `sqlite3_state` interface that didn't exist anywhere yet and failed to build.

After this document, the ten subsystem libraries are populated from `sqlite3-legacy-alternate`'s decomposition, each still an independent `SHARED` target per SRS 001 FR-4. A new umbrella, `libraries/sqlite3`, links all ten and is the one place the otherwise-cyclic symbol graph among them gets fully resolved (SRS 001 FR-4 already found this graph cyclic and established the "resolve at the final link" pattern for a throwaway test executable; this document makes `libraries/sqlite3` that final link, permanently). `sqlite3-legacy-alternate` itself is untouched and keeps building and working exactly as before — `example-001` still links against it directly, unaffected by this document.

### 2.2 Design and Implementation Constraints

- No line of implementation logic from `sqlite3-legacy-alternate` is edited during categorization or population — every copied file is byte-identical to its origin, same as SRS 001 FR-3's copy discipline.
- SRS 001 FR-4's constraint carries over unchanged: no `target_link_libraries()` between the ten subsystem libraries. `libraries/sqlite3` is the sole exception, by design (§3.2).
- Must build with the toolchain already validated for this workspace (GCC 16.2, CMake 4.4.2), consistent with `sqlite3-legacy-alternate`'s own C23 target.
- `test/`, `mptest/`, and any `.test` file are out of scope for this pass; the workspace currently has no TCL suite wired to these libraries (`tests/CMakeLists.txt` registers nothing at the time this document starts) — see §6 for what verification actually covers instead.

---

## 3. Methodology

### 3.1 Categorization: mapping 322 containers onto ten libraries

`sqlite3-legacy-alternate/src/sqlite` has no subsystem structure of its own — it's a single flat directory, one container per symbol, decomposed straight from the preprocessed amalgamation rather than from SQLite's original per-subsystem files. To place each container into one of SRS 001 §1.3's ten libraries, this document reuses that table (expanded to the ~150 explicit original filenames it lists) as an oracle, and asks, for every container's eponymous symbol: *which original SQLite source file, from the pristine `libraries/sqlite3-legacy/pkg/sqlite-src-3530400/src` tree, uses this symbol the most?*

Concretely, for each container's symbol name, every pristine `.c`/`.h`/`parse.y` file is scanned for whole-word occurrences of that name; the pristine file with the highest occurrence count is taken as the symbol's origin, and SRS 001 §1.3's table maps that origin file to a library. Counting *usage*, not just the definition site, matters because many container symbols (`Expr`, `Select`, `Table`, `Token`, `Parse`, ...) are struct types declared once in the shared `sqliteInt.h` (which SRS 001 §1.3 never assigns to any one library) but overwhelmingly *used* inside one particular subsystem's files — e.g. `Expr` is declared in `sqliteInt.h` but used far more inside `expr.c`/`resolve.c`/`select.c` (command-processor) than anywhere else, and ranking by usage sends it there correctly.

This was verified against the two symbols SRS 001 §1.3 states most plainly: `sqlite3` itself ranked highest in `main.c` → `sqlite3-core-interface`, exactly SRS 001's own placement; `sqlite3_stmt` ranked highest in `vdbeapi.c` → `sqlite3-core-virtual-machine`, likewise exact. That agreement on the two clearest cases is the basis for trusting the same method on the remaining ~300.

### 3.2 Build wiring: header visibility without link cycles

SRS 001 FR-4 already discovered that the real symbol graph between these ten libraries is cyclic and that CMake refuses target-level cycles between `SHARED` targets — its answer was: compile each independently, leave cross-library symbols unresolved by design, resolve them only once something links the whole set together. This document keeps that answer and supplies the piece SRS 001 stopped short of: `libraries/sqlite3` **is** that final link.

But *headers* are a different problem from symbols: a file physically placed in one library routinely `#include`s a container header that this document's categorization placed in a different library (e.g. `Btree.c`, sorted into `sqlite3-backend-tree`, reaches for `Pager.h`, sorted into `sqlite3-backend-pager`). Headers carry no link-time cost, so this is solved by giving every subsystem library's `CMakeLists.txt` a compile-time (not link-time) include path onto every sibling's public headers — `cmake/SqliteCppLibrary.cmake`'s `sqlite3_cpp_add_split_library()` function does this once, for all ten. A second, narrower issue surfaced during implementation: a handful of `sqlite3-legacy-alternate` containers `#include` their own header as a bare `"X.h"` instead of the tree's own `"sqlite/X.h"` convention (e.g. `bft.c` → `"bft.h"`) — harmless there because both files sat in the same flat directory, but broken once `csrc/` and `include/sqlite/` are physically separate directories. Rather than patch the (untouched-by-policy) source, each library's `include/sqlite/` subdirectory is added to the search path directly, alongside its `include/` parent, for itself and every sibling — neutralizing the whole bare-include class without editing a single copied file.

### 3.3 The `sqlite3_state` interface: a boundary rename, not a tree-wide one

A literal rename of the `sqlite3` struct tag would touch every one of its hundreds of use sites across all ten libraries — a large, high-risk edit for what `applications/example-002` actually needs, which is just: a type called `sqlite3_state`, and a `sqlite3_state_*`-prefixed function for each of the handful of calls it makes. This document takes the narrower, lower-risk path: `libraries/sqlite3/include/sqlite/sqlite3_state.h` declares

```c
typedef sqlite3 sqlite3_state;
```

— a type alias, not a second, differently-laid-out struct — plus one forwarding function per `sqlite3_state_*` name `example-002` calls (`open`, `close`, `prepare_v2`, `step`, `finalize`, `column_count`, `column_type`, `column_text`, `column_int`, `column_double`), each a one-line call into the real `sqlite3_*` function from the subsystem libraries. `sqlite3_stmt` is *not* renamed — `example-002` already includes `"sqlite/sqlite3_stmt.h"` directly and expects that name unchanged; only the top-level connection type and the entry points that take it get the `sqlite3_state` treatment. This satisfies the interface `example-002` was written against exactly, at a fraction of the blast radius of a real identifier rename — and is recorded here as a deliberate design choice, not a shortcut taken silently, for anyone who did want the deeper rename to redirect.

### 3.4 Exclusions: extensions

Consistent with SRS 001 §1.3's own deferral of FTS3/4/5, RTree, geopoly, session, RBU, and ICU, any `sqlite3-legacy-alternate` container whose name matches one of these extensions (by prefix or embedded token — `fts5_*`, `rtree_*`, `sqlite3_rtree_*`, `session*`, `rbu*`, `icu*`, and a further ~20 known extension/test-harness-only names) is excluded from all ten subsystem libraries entirely, rather than guessed into one by the usage-ranking method. 11 containers were excluded this way; none of the ten libraries build any extension support as a result, matching SRS 001's existing scope line.

### 3.5 Known imprecision (documented, not hidden)

- **`sqlite3-compiler-tokenizer` is effectively empty.** The usage-ranking method found no container whose dominant original-file association was `tokenize.c` — the real tokenizer entry points (`sqlite3RunParser`, `sqlite3GetToken`, `sqlite3Tokenize`) turned out to live inside the much larger `Parse.c` container (see next point), which ranks to `sqlite3-core-command-processor` instead. `sqlite3-compiler-tokenizer` keeps its placeholder `csrc/stub.c` and builds as a valid, near-empty `SHARED` target. This is a real gap, not swept under the rug: recovering a dedicated tokenizer library would need the tokenizer-specific functions split out of `Parse.c` by hand or by a `clang-refactor`-style tool, which is explicitly out of scope (§1.2).
- **A few containers bundle more than their name suggests.** `Parse.c` in particular carries the `Parse` struct and its heavy use across command-processor *and* the tokenizer entry points *and* several unrelated static date-parsing helpers (`parseTimezone`, `parseHhMmSs`, `parseYyyyMmDd`) that arguably belong with `date.c`. `sqlite3.c` similarly bundles the `sqlite3` connection object's core API together with JSON function registration and a few other loosely-related globals. Both are assigned whole to their usage-ranked dominant library (`sqlite3-core-command-processor` and `sqlite3-core-interface` respectively) rather than torn apart — the same kind of whole-file trade-off SRS 001 FR-3 already made for its own boundary cases (§1.3's `†`-marked files, and the eight code-generator files duplicated wholesale rather than split).
- **`sqlite3-compiler-code-generator` duplication.** Following SRS 001 FR-3's own precedent (its eight-file duplication note), every container whose usage-ranked origin file is one of `select.c, expr.c, insert.c, update.c, delete.c, trigger.c, where.c, wherecode.c` is copied into **both** `sqlite3-core-command-processor` and `sqlite3-compiler-code-generator` — 40 containers, under this document's categorization. The opcode-emission logic in these files still isn't separable from command-processor's planning logic at file granularity; that split remains SRS 002's job, exactly as SRS 001 stated.

A machine-readable record of every container's classification — matched library, decision reason (usage match / manual override / primitive fallback / extension skip), and (where applicable) the top five candidate origin files with their occurrence counts — is kept at [`005-subsystem-population-from-legacy-alternate.md.d/categorization-report.json`](005-subsystem-population-from-legacy-alternate.md.d/categorization-report.json), alongside the script that produced it, mirroring `sqlite3-legacy-alternate/tools/clang-refactor/uncategorized-report.json`'s own practice of keeping a historical record of judgment calls of this kind.

---

## 4. Functional Requirements

- **FR-1 (Categorization & population).** Every non-extension container in `sqlite3-legacy-alternate/src/sqlite` is assigned to exactly one of the ten SRS-001 §1.3 libraries per §3.1, and copied (never moved) into that library's `csrc/` (`.c`) and `include/sqlite/` (`.h`). `sqlite3-legacy-alternate` itself is left byte-for-byte unmodified. Containers ranked to one of the eight code-generator-adjacent files (§3.5) are copied into `sqlite3-compiler-code-generator` as well as their primary library.

- **FR-2 (Independent build, shared headers).** Each of the ten libraries builds as its own `SHARED` target over its FR-1-populated `csrc/`, via the shared `cmake/SqliteCppLibrary.cmake` helper. No `target_link_libraries()` exists between any two of the ten. Every library's public headers are visible at compile time to every other library (both the `include/` and `include/sqlite/` forms, per §3.2), regardless of the absence of a link edge.

- **FR-3 (Umbrella linking).** `libraries/sqlite3` links all ten subsystem libraries together (`-Wl,--no-as-needed` to force every one onto the executable's `NEEDED` list even where the umbrella's own code doesn't call into it directly) and is the single point where the whole cross-library symbol graph resolves — verified by zero undefined symbols in the resulting `libsqlite3.so` (`ldd -r`).

- **FR-4 (`sqlite3_state` interface).** `libraries/sqlite3/include/sqlite/sqlite3_state.h` declares `sqlite3_state` as a type alias for `sqlite3` and the forwarding functions listed in §3.3, implemented in `libraries/sqlite3/csrc/sqlite3_state.c`.

- **FR-5 (Working example).** `applications/example-002` links against `sqlite3` (the umbrella target, not `sqlite3-legacy-alternate`) and builds without modification to its own source — it was already written against exactly this interface. It's verified to run against a real on-disk database (not just link).

---

## 5. Acceptance Criteria

1. `cmake --build` succeeds for the full workspace from a clean, empty build directory (`CMAKE_BUILD_TYPE=Release`): all ten subsystem libraries, `libraries/sqlite3`, and both `applications/example-001` and `applications/example-002` build with zero compiler errors.
2. `ldd -r` on the built `libsqlite3.so` reports zero undefined symbols.
3. `applications/example-002` runs against a real SQLite database file, executing a real `SELECT` and printing real rows through the `sqlite3_state_*` API — not just a successful link.
4. `applications/example-001` (linking `sqlite3-legacy-alternate` directly, unaffected by this document) continues to build and run correctly, confirming the population pass didn't disturb that project.
5. `sqlite3-legacy-alternate/src` is byte-identical before and after — this document only reads from it.

**Status: all five criteria met**, verified in §6.

---

## 6. Status

- **FR-1 (Categorization & population): done.** 322 containers processed: 251 with a `.c`, 311 with a `.h`; 11 excluded as extension-only (§3.4); 40 duplicated into `sqlite3-compiler-code-generator` per §3.5. Per-library symbol counts: `sqlite3-utils` 43, `sqlite3-backend-os` 17, `sqlite3-backend-pager` 23, `sqlite3-backend-tree` 11, `sqlite3-core-virtual-machine` 33, `sqlite3-core-command-processor` 115, `sqlite3-core-interface` 65, `sqlite3-compiler-tokenizer` 0 (§3.5), `sqlite3-compiler-parser` 4, `sqlite3-compiler-code-generator` 40. Every library's placeholder `csrc/stub.c` was removed except `sqlite3-compiler-tokenizer`'s, which stays (§3.5).
- **FR-2 (Independent build, shared headers): done.** `cmake/SqliteCppLibrary.cmake` (recreated — referenced by stale comments in root `CMakeLists.txt` but absent on disk at the start of this document) supplies `sqlite3_cpp_add_split_library()`, used by all ten libraries' `CMakeLists.txt`. A pre-existing bug found in passing: `sqlite3-core-virtual-machine` and `sqlite3-utils`'s `CMakeLists.txt` had their `project()` names swapped (each declared the other's), silently building the wrong target name under the right directory — fixed as part of rewriting all ten files identically from the shared helper.
- **FR-3 (Umbrella linking): done.** `libraries/sqlite3` links all ten via `-Wl,--no-as-needed`; `ldd -r` on the resulting `libsqlite3.so` reports zero undefined symbols.
- **FR-4 (`sqlite3_state` interface): done.** `sqlite3_state.h`/`.c` implemented per §3.3.
- **FR-5 (Working example): done.** `applications/example-002/CMakeLists.txt` changed from linking `sqlite3-legacy-alternate` to linking `sqlite3` (its `src/main.c` required no change — it already targeted this interface). Verified end-to-end: built a real `expenses.db` with the system `sqlite3` CLI, ran the built `sqlite-cpp-applications-example-002` binary against it, and confirmed it printed the real inserted rows (`coffee, 4.5,` / `books, 30,`) via `sqlite3_state_open` → `sqlite3_state_prepare_v2` → `sqlite3_state_step` → `sqlite3_state_column_*` → `sqlite3_state_finalize` → `sqlite3_state_close`.

Final verification, from a genuinely clean build directory, `CMAKE_BUILD_TYPE=Release`: full workspace configure and build succeed with zero compiler errors, all ten subsystem `.so`s plus `libsqlite3.so` are produced, both examples build and run correctly, and `sqlite3-legacy-alternate/src` is unmodified. **All five acceptance criteria met.**

Not yet done, and explicitly out of scope for this document (§1.2): the `sqlite3-compiler-tokenizer` gap (§3.5), any deeper function-level re-splitting of multi-concern containers, and no TCL/ctest suite currently exercises these libraries (`tests/CMakeLists.txt` registers none at the time of writing — pre-existing, not introduced by this document).

---

## 7. Glossary

| Term | Meaning |
|---|---|
| Container | One `.c`/`.h` pair in `sqlite3-legacy-alternate/src/sqlite`, holding one struct, function, or typedef's worth of code (§ architecture.md in that project) |
| Usage-ranked origin | The pristine original SQLite source file where a container's eponymous symbol appears most often by whole-word count — the basis for this document's categorization (§3.1) |
| Umbrella library | `libraries/sqlite3` — the one target that links all ten subsystem libraries together and is where their otherwise-cyclic symbol graph fully resolves |
| Boundary rename | `sqlite3_state`'s design: a type alias plus forwarding functions at the public API surface, not a rename of the underlying `struct sqlite3` tag itself (§3.3) |
