# Software Requirements Specification

## Complete C Implementation Retirement — `sqlite-cpp`

*SRS 002 — see the [SRS index](index.md) for the full list of SRS documents.
This document is additive to, and assumes, [SRS 001](001-sqlite-cpp-modularization.md),
and its retirement timing is constrained by [SRS 003](003-sil4-safety-integrity-validation.md)
§1.5/§9.4 wherever a library is also in SIL4 scope.*

| | |
|---|---|
| **Document status** | Draft v0.1 — requirements capture, not yet reviewed or approved |
| **Subject system** | The legacy C implementation (`src/`, `ext/*.c`) SRS 001 requires to keep working, unmodified, as the acceptance oracle for as long as it coexists with its C++ replacement |
| **Target** | A repository in which every in-scope library and application has fully cut over to its `sqlite-cpp` implementation, and the legacy C source it replaced no longer exists in the build graph |

---
## 1. Introduction

### 1.1 Purpose

SRS 001 builds `sqlite-cpp` as a **strangler-fig migration** (SRS 001 §1.3):
the legacy C implementation and its new C++ replacement coexist, with the
legacy code retained, unmodified, as both the running implementation for
not-yet-migrated pieces and the behavioral oracle for pieces that have
already migrated (SRS 001 §1.2, FR-10). SRS 001 never says the coexistence
ends — its phased plan (§12) describes each phase "retiring" that phase's C
sources but does not specify what retirement actually requires: what gate a
library must pass before its C files can be deleted, what happens to the
build system's references to them, what happens to consumers (applications,
the amalgamation) still compiled against the legacy target, and how this is
sequenced against SRS 003's safety-integrity evidence, which explicitly
depends on the legacy code staying alive longer than a naive reading of
"retire per phase" would allow. This document specifies that retirement
process, so that `sqlite-cpp` has a defined endpoint — a single implementation,
not a permanent fork — rather than an open-ended coexistence that never
actually concludes.

This document uses **the same design approach as SRS 001**: it does not
redefine the library decomposition, namespaces, or dependency order (SRS 001
§3); it does not reopen SRS 001's API-compatibility requirements (SRS 001
§5); it adds retirement-specific requirements (**RR-**) on top of an
SRS 001-conformant, already-accepted library, in the same additive style
SRS 003 uses for safety requirements (**SR-**).

### 1.2 Scope

**In scope:** the process, gates, and build-system requirements for deleting
each piece of legacy C source (`src/*.c`/`.h`, `ext/*.c` for the extensions
listed in SRS 001 §3.6, the generated artifacts in SRS 001's compiler
libraries' legacy form) once its `sqlite-cpp` replacement has been accepted;
updating the CMake build (`CMakeLists.txt`, `cmake/SqliteFeatures.cmake`,
`cmake/SqliteCodegen.cmake`, `libraries/libsqlite3/`) to stop building what
is retired; cutting the applications (SRS 001 FR-20) over to depend solely on
the `sqlite-cpp` implementation; and deciding the fate of the single-file
amalgamation (`sqlite3.c`) distribution, which today is generated directly
from the legacy C sources this document retires (§9.1).

**Out of scope:** re-deciding SRS 001's library boundaries or API-
compatibility guarantees (both stand as-is); performing the SIL4 safety-case
work SRS 003 specifies (this document only constrains *when* retirement may
happen relative to that work, per §1.5); rewriting the code generators
themselves (`lemon`, `mkkeywordhash`, `mkopcodeh`/`mkopcodec`) — §9.2 states
explicitly that retiring "the C code" means retiring **generated C as a
shipped implementation**, not retiring the C-producing generator tools SRS
001's compiler libraries still depend on as build-time codegen.

### 1.3 Definitions, Acronyms, Abbreviations

| Term | Meaning |
|---|---|
| Retirement | Deleting a legacy C source file (or generated-C artifact) from the build graph because a functionally-equivalent, accepted `sqlite-cpp` replacement exists |
| Cutover | The point at which a consumer (an application, another library, the amalgamation) switches from linking/compiling the legacy C implementation to linking/compiling the `sqlite-cpp` implementation |
| Parity gate | The specific, checkable condition (RR-1) a library's C++ replacement must satisfy before its legacy C may be retired |
| Amalgamation | SQLite's traditional single-file (`sqlite3.c`) distribution, historically hand-assembled from `src/`; see §9.1 for its status under this SRS |
| Dead reference | A build-system reference (`target_sources`, `add_subdirectory`, `#include`) to a file this document has retired, left behind by mistake |
| Diversity evidence | SRS 003 SR-12's differential-testing evidence, captured by running legacy C and new C++ against the same inputs while both are live |

### 1.4 References

- [SRS 001](001-sqlite-cpp-modularization.md) — defines the library
  decomposition, namespaces, API-compatibility requirements, and phased plan
  this document's retirement schedule is gated on. This document changes
  none of it.
- [SRS 003](003-sil4-safety-integrity-validation.md) — its SR-12
  ("regression-as-diversity") requires the legacy C implementation to remain
  live, for a library in SIL4 scope, until that library's differential
  evidence has been captured. This document's RR-2 and §10 are written to
  be gated on that evidence for any library so scoped (§9.4).
- `CMakeLists.txt`, `cmake/SqliteFeatures.cmake`, `cmake/SqliteCodegen.cmake`,
  `libraries/libsqlite3/` — the existing CMake conversion this document's
  build-system requirements (§6) modify by removing entries, not by adding a
  new build system.
- `test/` (1,190 `.test` files) and `test/testrunner.tcl` — SRS 001's
  unmodified acceptance oracle; this document's parity gate (RR-1) is
  defined in terms of this suite continuing to pass with the legacy code
  path physically removed, not merely disabled.

### 1.5 Relationship to SRS 001 and SRS 003

This document is a third, additive layer: SRS 001 says what `sqlite-cpp` is;
SRS 003 says what qualifying it at SIL4 requires; this document says how the
legacy C implementation SRS 001 builds *alongside* stops being necessary. A
library is eligible for retirement under this document only once it has met
SRS 001's acceptance criteria (SRS 001 §10) — retirement is a step that
follows functional acceptance, never precedes or substitutes for it. Where a
library is also in SIL4 scope under SRS 003, this document's RR-2 defers to
SRS 003 SR-12: retirement for that library waits on SRS 003's differential
evidence being captured, not just on SRS 001 acceptance. This is not a
conflict between the two documents — SRS 003 §1.5 states the same
constraint from its own side, so the two are mutually consistent by
construction, not by coincidence.

### 1.6 Overview

§2 gives the end state this document works toward and the constraints on
getting there. §3 restates SRS 001 §3's library table as a retirement map —
which legacy files retire when each library is retired. §4 gives general
retirement requirements (RR-1–RR-10) applying to every library. §5 gives
per-library retirement notes where a library has a wrinkle general
requirements do not cover. §6 gives build-system requirements. §7 gives
testing requirements. §8 gives acceptance criteria. §9 gives constraints,
risks, and open issues — most importantly the amalgamation's fate (§9.1) and
this document's single highest-risk item, premature retirement (§9.3). §10
gives a phased plan gated on SRS 001 §12. §11 gives a traceability skeleton.
§12 is a glossary.

---
## 2. Overall Description

### 2.1 Product Perspective — End State

Once every library and application in scope has been retired under this
document: `src/` and the retired portions of `ext/` no longer exist in the
repository's build graph (§1.3's "dead reference" is explicitly disallowed,
RR-3); `libraries/libsqlite3` (the legacy CMake C target, SRS 001 §1.4) is
removed or repurposed as a thin re-export of the `sqlite-cpp` façade, never
as an independent build of legacy sources; every application (SRS 001
FR-20) links against the `sqlite-cpp` `extern "C"` façade (SRS 001 FR-7) or
a namespaced library directly; and, per §9.1's resolution, the amalgamation
distribution — if retained at all — is generated from the C++ implementation,
never hand-maintained or generated from now-deleted C.

### 2.2 Design Goals

This document deliberately introduces **no new architecture**. It uses the
same library decomposition, namespace scheme, header-only convention, and
per-library testing/documentation pattern SRS 001 already establishes (SRS
001 §2.2, §3, §4). The only thing this document adds is a *removal* process:
when it is safe to delete legacy C for a library, what has to happen to the
build system and consumers when that occurs, and how that is sequenced
against SRS 001's phased delivery and SRS 003's safety evidence. Where SRS
003 introduces a new requirement-ID namespace (`SR-`) for the same reason —
extending SRS 001 without redesigning it — this document does the same with
`RR-` (Retirement Requirement).

### 2.3 User Classes

- **Library implementers** — perform retirement once their library's parity
  gate (RR-1) is met; the same audience as SRS 001's "Contributors."
- **Build maintainers** — own `CMakeLists.txt` and the `cmake/*.cmake`
  files this document's RR-3/§6 require updating in lockstep with file
  deletion.
- **Application integrators** — cut applications over per RR-8 once the
  façade covers their full dependency surface.
- **Downstream consumers of the amalgamation** — a user class SRS 001 does
  not separately name, introduced here because §9.1's open item concerns
  them specifically: people who vendor `sqlite3.c` directly rather than
  linking a library.

### 2.4 Design and Implementation Constraints

- No behavioral or ABI change may result from retirement (RR-6); retirement
  is a build-graph and source-tree change, never a functional one — if a
  functional gap surfaces during retirement, that is evidence the parity
  gate (RR-1) was not actually met, not a license to accept a regression.
- Legacy source history is never deleted from git (RR-9), even once removed
  from the working tree, since SRS 003 SR-12's diversity evidence needs to
  remain reconstructable after cutover.
- Retirement for a library in SIL4 scope (SRS 003) may not occur before
  that library's SRS 003 differential evidence is captured (§1.5, §9.4).
- The code-generation toolchain (`lemon`, `mkkeywordhash`,
  `mkopcodeh`/`mkopcodec`) is retained as a build dependency for as long as
  any `sqlite-cpp` library still consumes its output as an input to a C++
  build step (§9.2) — this document retires generated-C-as-shipped-
  implementation, not the generators themselves.

### 2.5 Assumptions and Dependencies

- Every SRS 001 §12 phase this document schedules retirement against has
  independently reached SRS 001's acceptance criteria (SRS 001 §10) before
  this document's RR-1 gate is even evaluated for that phase's libraries.
- SRS 003, where applicable to a library, has stated (or explicitly waived)
  a differential-evidence requirement for that library before RR-2 permits
  its retirement.
- A decision on the amalgamation's fate (§9.1) will be made by whoever
  approves this SRS before the phase in which it would first become
  relevant (§10 phase 5, once `sqlite-core-interface` — the composition
  root — is retirement-eligible).

---
## 3. Legacy Retirement Map

Restates SRS 001 §3's library table as a retirement map: for each library,
the legacy files that are deleted once that library is retired. This is not
a new mapping — it is SRS 001 §3's existing "Legacy source" columns, read as
a deletion list rather than a migration-source list.

| Library (SRS 001 §3) | Legacy files retired | Notes |
|---|---|---|
| `sqlite-utils` | `mem0.c`–`mem5.c`, `malloc.c`, `util.c`, `printf.c`, `utf.c`, `hash.c`/`.h`, `mutex.c`/`.h`, `random.c`, `status.c`, `fault.c` | Lowest-risk, first-retired set (§10 phase 1) |
| `sqlite-backend-os` | `os.c`/`.h`, `os_common.h`, `os_setup.h`, `os_unix.c`, `mutex_unix.c`, `os_win.c`/`.h`, `mutex_w32.c`, `os_kv.c`, `memdb.c` | Both platform variants (FR-6) retire together — see §5.1 |
| `sqlite-backend-pager` | `pager.c`/`.h`, `wal.c`/`.h`, `pcache.c`/`.h`, `pcache1.c`, `memjournal.c`, `bitvec.c` | Gated on SRS 003 SR-3(a)'s formal evidence where in SIL4 scope |
| `sqlite-backend-tree` | `btree.c`/`.h`, `btreeInt.h`, `dbpage.c`, `dbstat.c` | Gated on SRS 003 SR-3(b) where in SIL4 scope |
| `sqlite-core-virtual-machine` | `vdbe.c`/`.h`, `vdbeInt.h`, generated `opcodes.h`/`.c`, `vdbeapi.c`, `vdbeaux.c`, `vdbemem.c`, `vdbesort.c`, `vdbeblob.c`, `vdbetrace.c`, `vdbevtab.c` | `opcodes.h`/`.c` generation (`mkopcodeh`/`mkopcodec`) retained per §9.2 even after `.c` output retires — see §5.2 |
| `sqlite-core-command-processor` | `prepare.c`, `pragma.c`/`.h`, `insert.c`, `update.c`, `delete.c`, `upsert.c`, `select.c`, `where.c`, `wherecode.c`, `whereexpr.c`, `whereInt.h`, `expr.c`, `resolve.c`, `walker.c`, `attach.c`, `alter.c`, `analyze.c`, `vacuum.c`, `table.c`, `rowset.c`, `build.c` (schema-mutation portions), `trigger.c`, `window.c`, `fkey.c`, `auth.c`, `callback.c`, `treeview.c` | Not retirement-eligible until SRS 001 §11.2's IR design lands and the command-processor/code-generator split is implemented — see §5.3 |
| `sqlite-core-interface` | `main.c`, `legacy.c`, `loadext.c`, `sqlite3ext.h` (implementation, not the ABI header itself — see §5.4), `vtab.c`, `backup.c`, `notify.c`, `threads.c`, `complete.c` | Composition root; retires last among the core libraries, since every other library's façade routes through it |
| `sqlite-compiler-tokenizer` | `tokenize.c`, generated `keywordhash.h` | `mkkeywordhash` generation retained per §9.2 |
| `sqlite-compiler-parser` | `parse.y`/`parse.c`/`.h` (generated), AST-construction portions of `build.c` | `lemon` retained per §9.2; see §5.3 for the `build.c` split with command-processor |
| `sqlite-compiler-code-generator` | Opcode-emission portions of `select.c`, `expr.c`, `insert.c`, `update.c`, `delete.c`, `trigger.c`, `where.c`/`wherecode.c` | Same IR-design gate as command-processor (§5.3) |
| Extensions (SRS 001 §3.6) | `ext/fts3`, `ext/fts4`, `ext/fts5`, `ext/rtree`, `ext/geopoly`, `ext/session`, `ext/rbu`, `ext/icu` C sources | Retire independently, per extension, once that extension's C++ port is accepted (RR-7) |

---
## 4. General Retirement Requirements

Apply to every library in §3 unless a §5 per-library note states otherwise.

- **RR-1. Parity gate.** A library's legacy C files may be deleted only
  after: (a) the library has met SRS 001's acceptance criteria (SRS 001
  §10); and (b) the full oracle suite (SRS 001 FR-10, `sqlite_fulltest`)
  passes with that library's legacy code path **physically removed** from
  the build — not merely disabled behind a flag — so the pass result is
  evidence about the C++ replacement alone, not about a fallback path that
  happens not to have been exercised.
- **RR-2. No indefinite dual-maintenance.** Strangler-fig coexistence (SRS
  001 §12) for a given library ends at that library's own phase boundary
  once RR-1 is met, except where SRS 003 SR-12 requires holding retirement
  open longer for differential-evidence capture (§1.5, §9.4) — in which
  case retirement waits on that evidence explicitly, not indefinitely.
  Coexistence continuing past either gate is a defect to be scheduled for
  cleanup, not a stable end state.
- **RR-3. Build system deletion in the same change.** Every `target_sources()`
  entry, file glob, and `add_subdirectory` reference to a retired file is
  removed in the same change that deletes the file — `cmake/SqliteFeatures.cmake`
  and `cmake/SqliteCodegen.cmake` updated accordingly. A retirement change
  that deletes source files but leaves a dead build reference is incomplete
  (§7's CI gate catches this).
- **RR-4. Codegen dependency decision recorded per artifact.** For every
  generated-C artifact in §3 (`parse.c`, `keywordhash.h`, `opcodes.h`/`.c`),
  the retirement change states explicitly whether the generator
  (`lemon`/`mkkeywordhash`/`mkopcodeh`/`mkopcodec`) is retained as a
  build-time dependency of the `sqlite-cpp` replacement (the default, per
  §9.2) or whether the C++ library has fully subsumed that generation step
  itself — this is a per-artifact decision, not a blanket assumption either
  way.
- **RR-5. Amalgamation regeneration, not hand-maintenance.** If the
  amalgamation (`sqlite3.c`) is retained as a deliverable at all (§9.1, an
  open item), it is produced by a build step that assembles it from the
  `sqlite-cpp` header-only libraries plus the `extern "C"` façade — never
  hand-edited, and never generated from a retired legacy source tree.
- **RR-6. No behavior or ABI change during cutover.** Restates SRS 001
  FR-7–FR-9: retirement changes source location and build wiring, never
  observable behavior, the on-disk format, or the public C API/ABI. This is
  restated here, not just inherited, because retirement is precisely where
  a silent regression is easiest to introduce (deleted code produces no
  compiler diagnostic for a missing behavior — only a test gap does) and
  hardest to notice after the fact.
- **RR-7. Extensions retire independently.** Each extension in SRS 001 §3.6
  retires on its own schedule once its own C++ port is accepted; extension
  retirement is not blocked on core retirement completing, nor does core
  retirement wait on any extension.
- **RR-8. Applications cut over per-dependency, not all at once.** An
  application (SRS 001 FR-20) moves from the legacy `libsqlite3` C target
  to the `sqlite-cpp` façade once the façade's coverage reaches that
  application's actual dependency surface — a shell command that only needs
  functionality already retired-and-replaced can cut over before every
  library is retired; §5.5 gives the concrete case (the `sqlite3` shell).
- **RR-9. Historical retention.** Retired legacy files remain in git
  history unconditionally; a tagged pre-retirement reference (branch or
  release tag) is created before a library's first retirement change lands,
  so SRS 003 SR-12's diversity evidence and any later audit can reconstruct
  exactly what was retired and when.
- **RR-10. Documentation updated, not left stale.** Every place SRS 001 §3,
  this document's §3, `docs/index.md`, or a library's own docs names a
  legacy `.c` file as "the" current implementation is updated, at
  retirement time, to say it has been retired and by what it was replaced
  — a stale "implemented in `pager.c`" note after `pager.c` no longer exists
  in the tree is exactly the kind of silent drift RR-6's spirit is meant to
  prevent, applied to documentation rather than code.

---
## 5. Per-Library Retirement Notes

Only libraries with a wrinkle beyond §4's general requirements are covered
here; every other library in §3 retires under §4 alone.

### 5.1 `sqlite-backend-os`

The two platform variants (`os_unix.c`/`mutex_unix.c` and
`os_win.c`/`mutex_w32.c`, SRS 001 FR-6's paradigm case) retire **together**,
as a single retirement event covering both `sqlite::backend::os::unix` and
`::windows`, even though only one variant is exercised on any given build
host — RR-1's oracle-suite parity gate must be satisfied on both platforms
independently (via CI on each) before either variant's legacy C is deleted,
since deleting one without cross-platform confidence in the other leaves an
asymmetric, unverified state.

### 5.2 `sqlite-core-virtual-machine`

`opcodes.h`/`.c` are generated from `vdbe.c`'s opcode comment annotations by
`mkopcodeh`/`mkopcodec`. Once the C++ opcode-dispatch implementation (SRS
001 §3.4.1) exists, this document requires (RR-4) an explicit decision:
either the C++ library defines its own opcode enumeration directly (per SRS
001 FR-5, as a scoped `enum class`) and the legacy generator retires along
with `vdbe.c`, or the generator's output is retained as an input the C++
enum is generated from, to avoid the two implementations drifting during
any remaining coexistence. The former is the expected default once
`sqlite-compiler-code-generator` (which emits against this enum) has also
migrated; the latter is acceptable only as an explicitly time-boxed interim
state.

### 5.3 `sqlite-core-command-processor` and `sqlite-compiler-code-generator`

Per SRS 001 §11.2/§3.5.3, these two libraries are not retirement-eligible
independently: the legacy `select.c`/`insert.c`/`update.c`/`delete.c`/
`where.c`/`wherecode.c` files simultaneously implement both "decide" and
"emit," so their legacy C cannot be partially retired without leaving a
functional gap. RR-1's parity gate for either library is only met once
**both** C++ replacements exist, cover the same statement forms, and pass
the full oracle suite with the corresponding legacy files removed together.
The same joint-retirement treatment applies to `build.c`'s split between
`sqlite-compiler-parser::ast` and `command_processor::schema` (SRS 001
§3.5.2): `build.c` retires once both halves' C++ replacements exist, not
when either alone does.

### 5.4 `sqlite-core-interface`

`sqlite3ext.h` is retired here only as a *legacy implementation detail*
(whatever `loadext.c` does internally); the header itself, as the public
extension ABI, is permanently preserved per SRS 001 FR-8 and is never a
retirement target under this document — RR-6 already forbids any ABI
change, and this note exists so "retire `sqlite-core-interface`'s legacy
files" is not misread as touching the ABI header.

### 5.5 Applications

The `sqlite3` shell, `sqldiff`, and `sqlite3-rsync` are, per SRS 001 §11.5,
written against the public C API rather than the namespaced libraries
directly; each cuts over (RR-8) from linking `libraries/libsqlite3` to
linking the `sqlite-cpp` `extern "C"` façade once the façade's functional
coverage reaches that specific tool's usage of the API — the shell, using
the broadest surface, is realistically the last of the three to cut over.
`speedtest1` cuts over last of all, since its purpose is comparing
performance against the legacy build, which requires the legacy build to
keep existing as a comparison target for as long as any performance
question (SRS 001 acceptance criterion 4) remains open — `speedtest1`'s own
cutover is therefore an explicit, separate decision, not implied by every
library it benchmarks having retired.

---
## 6. Build System Requirements

- **BR-1.** `CMakeLists.txt`'s `SQLITE_CPP_BUILD_*` options (one per
  library, e.g. `SQLITE_CPP_BUILD_BACKEND_TREE`) exist, per SRS 001, to let
  a library be built optionally during coexistence. Once a library retires
  (RR-1) and its legacy alternative no longer exists, that option is
  removed entirely — there is no longer a choice to gate, and leaving a
  no-op toggle in place misrepresents the build as still offering a legacy
  path.
- **BR-2.** `libraries/libsqlite3` (the legacy C CMake target) is removed
  once every library in §3 has retired, or repurposed as a thin
  compatibility target that simply depends on the `sqlite-cpp` façade, if a
  consumer needs the exact target name `libsqlite3` to keep resolving.
  Which of the two is chosen is recorded as part of §10 phase 5's exit
  criteria, not left implicit.
- **BR-3.** `cmake/SqliteCodegen.cmake`'s generator invocations
  (`lemon`, `mkkeywordhash`, `mkopcodeh`/`mkopcodec`) are retained exactly
  where §5.2/§9.2 require them and removed exactly where a library has
  fully subsumed its own generation — this file's contents after full
  retirement are not "empty," they are "exactly what RR-4's per-artifact
  decisions require," which may be nonzero.

---
## 7. Testing Requirements

- **TR-1.** RR-1's parity gate is itself the primary test requirement: the
  full `sqlite_fulltest` TCL suite must pass with the legacy path physically
  absent, not disabled, before a retirement change is accepted.
- **TR-2.** A CI check greps the build graph (not just the source tree) for
  references to any file this document has retired; the build fails if a
  `target_sources()`, `add_subdirectory`, or generated-file dependency still
  names a retired file, catching RR-3 violations mechanically rather than
  relying on manual review.
- **TR-3.** Where a library is in SIL4 scope (SRS 003), that library's
  differential-testing evidence (SRS 003 SR-12) is captured and archived
  **before** its retirement change is merged, not concurrently with it —
  once the legacy file is deleted, capturing that evidence is no longer
  possible.

---
## 8. Acceptance Criteria

1. For every library in §3 marked retired, its legacy C files (per that
   row) no longer exist in the working tree, and TR-2's CI check finds no
   dead build reference to them.
2. `sqlite_fulltest` (SRS 001's full 1,190-file TCL suite) passes against a
   build containing only the `sqlite-cpp` implementation for every retired
   library, with no new failures relative to the pre-retirement baseline.
3. Every application in §5.5 that has cut over links and runs against the
   `sqlite-cpp` façade exclusively, with no remaining dependency on
   `libraries/libsqlite3`'s legacy build.
4. RR-9's historical-retention requirement is met: retired files are
   present in git history and reachable from a pre-retirement tag for each
   library's retirement event.
5. For every library also in SRS 003 scope, that library's differential
   evidence (SRS 003 SR-12) was captured and archived before its
   retirement change merged (TR-3).
6. The amalgamation's fate (§9.1) has been explicitly decided (not left
   ambiguous) by the time `sqlite-core-interface` retires (§10 phase 5).

---
## 9. Constraints, Risks, and Open Issues

### 9.1 The amalgamation's fate is an open decision, not resolved here

Many real-world SQLite consumers vendor the single-file `sqlite3.c`
amalgamation directly, compiling it into their own build rather than
linking a library. Today that file is produced from `src/` — the very
sources this document retires. Whoever approves this SRS must decide, before
§10 phase 5: (a) keep offering an amalgamation, mechanically regenerated
from the `sqlite-cpp` header-only libraries plus the `extern "C"` façade
(RR-5), accepting the build-step complexity that requires; or (b) declare
amalgamation-based consumption unsupported once retirement completes,
accepting that this breaks a real, currently-working consumption pattern for
existing users. This document does not choose between them — exactly the
kind of explicit, flagged-not-assumed open item SRS 001 §11.6 models for its
own performance-threshold question.

### 9.2 Retiring "the C code" does not mean retiring the code generators

`lemon` (the LALR parser generator producing `parse.c` from `parse.y`),
`mkkeywordhash`, and `mkopcodeh`/`mkopcodec` are themselves C programs that
*produce* C, but their role is generating build-time input, not shipping as
part of the running implementation. This document's scope is retiring
generated C **as a shipped implementation** (§1.2); it does not require
rewriting these generator tools, and a reader who takes "complete C
retirement" to mean the generators must also disappear has misread this
document's scope. §5.2 and RR-4 make the per-artifact decision explicit
specifically to prevent that misreading from becoming a build break.

### 9.3 Premature retirement is this document's single highest risk

RR-1's parity gate is the only safeguard against deleting legacy C before
its replacement is actually equivalent. Because retirement removes code
rather than adding it, a functional gap introduced by premature retirement
shows up as *silence* — a missing error, a subtly wrong result, a behavior
nobody wrote a test for because nobody knew it existed — not as a build
failure. This should be treated with at least the rigor SRS 001 §11.7 gives
the refactor's overall effort, and arguably more: SRS 001's risk is scope
and schedule; this document's risk, done wrong, is a shipped regression with
no compiler diagnostic pointing at it.

### 9.4 Interaction with SRS 003's diversity evidence

SRS 003 SR-12 explicitly requires the legacy C implementation to remain live
for differential testing during a library's SIL4 qualification. This
document's RR-2 is written to defer to that requirement rather than override
it (§1.5): for any library in SIL4 scope, "retirement-eligible per RR-1"
and "retirement-permitted per RR-2" are not the same moment, and treating
them as interchangeable would silently remove SRS 003's evidence source
before that evidence exists.

### 9.5 Effort and schedule reality check

Retirement touches every one of the ten libraries in §3 plus up to eight
extensions plus four applications, each requiring its own parity-gate
verification (RR-1), build-system change (RR-3/§6), and — for a subset —
coordination with SRS 003's evidence capture (§9.4). This is a real,
non-trivial tail of work after SRS 001's functional delivery completes, not
a mechanical cleanup pass; §10's phasing reflects that it should be
scheduled with the same seriousness SRS 001 §12 gives the migration itself.

---
## 10. Phased Plan

Gated 1:1 on SRS 001 §12's phases, one step behind: a phase's legacy C
retires only once that phase's `sqlite-cpp` library has met SRS 001's
acceptance criteria **and** (where applicable) SRS 003's differential
evidence for that phase has been captured (§9.4).

1. **Scaffolding retirement.** `sqlite-utils`'s legacy files retire once
   SRS 001 phase 1 is accepted — lowest risk, and the template retirement
   change every later phase's retirement follows.
2. **Backend retirement.** `sqlite-backend-os` (both variants together,
   §5.1), then `-pager`, then `-tree`, each once its SRS 001 phase 2
   milestone is accepted and, where in SIL4 scope, its SRS 003 evidence is
   captured.
3. **Virtual machine retirement.** `sqlite-core-virtual-machine`'s legacy
   files retire once SRS 001 phase 3 is accepted; §5.2's opcode-generation
   decision is made explicitly as part of this phase, not deferred.
4. **Compiler and command-processor retirement, together.** Tokenizer and
   parser retire once SRS 001 phase 4's first half is accepted; the
   code-generator/command-processor pair retires jointly (§5.3) once SRS
   001 §11.2's IR design has landed and both replacements are accepted
   together — never one without the other.
5. **Interface façade retirement.** `sqlite-core-interface` retires once
   SRS 001 phase 5 is accepted; BR-2's `libraries/libsqlite3` decision and
   §9.1's amalgamation decision are both made explicitly by the end of this
   phase, since every later phase assumes an answer to both.
6. **Extension retirement.** Each extension retires independently (RR-7)
   once its SRS 001 phase 6 port is accepted; not blocked on, and does not
   block, core retirement.
7. **Application cutover.** Per RR-8/§5.5, each application cuts over once
   the façade's coverage reaches its dependency surface; `speedtest1` cuts
   over last, and only once its comparison purpose no longer requires the
   legacy build to exist (§5.5).

---
## 11. Traceability Matrix Skeleton

| SRS 001 library (§3) | This document's retirement row (§3) | RR-1 gate additionally requires SRS 003 evidence? |
|---|---|---|
| `sqlite-utils` | §3 row 1 | No |
| `sqlite-backend-os` | §3 row 2, §5.1 | No (environment-bounded per SRS 003 §8.5, not a formal-methods target) |
| `sqlite-backend-pager` | §3 row 3 | Yes — SRS 003 SR-3(a) |
| `sqlite-backend-tree` | §3 row 4 | Yes — SRS 003 SR-3(b) |
| `sqlite-core-virtual-machine` | §3 row 5, §5.2 | No (WCET/SR-7 instead, not gated as differential evidence) |
| `sqlite-core-command-processor` | §3 row 6, §5.3 | Yes — SRS 003 SR-3(c), joint with code-generator |
| `sqlite-core-interface` | §3 row 7, §5.4 | No |
| `sqlite-compiler-tokenizer` | §3 row 8 | No |
| `sqlite-compiler-parser` | §3 row 9, §5.3 | No |
| `sqlite-compiler-code-generator` | §3 row 10, §5.3 | Yes — shared gate with command-processor |
| Extensions | §3 row 11 | Only if individually brought into SRS 003 scope (SRS 003 §5) |

The full RTM is a generated, evidence-linked artifact (consistent with SRS
003 §11's approach to its own traceability matrix), keyed to this skeleton's
row correspondence.

---
## 12. Glossary

See §1.3 for acronyms and terms specific to this document. SRS 001 §13
remains authoritative for library, namespace, and legacy-file terminology
this document reuses without redefining.
