# Software Requirements Specification

## Complete C Implementation Retirement — `sqlite-cpp`

*SRS 002 — see the [SRS index](index.md) for the full list of SRS documents.
This document is additive to, and assumes, [SRS 001](001-sqlite-cpp-modularization.md),
and interacts with [SRS 003](003-sil4-safety-integrity-validation.md)'s
diversity-evidence requirement — see §1.5.*

| | |
|---|---|
| **Document status** | Draft v0.3 — requirements capture, not yet reviewed or approved. v0.2 revised v0.1's delete-per-library-as-you-go model (see §9.2) with a rename-and-compare model; v0.3 unifies every application — including the code generators — under `applications/`, split by whether it depends on the legacy `sqlite3` C API (§1.1). |
| **Subject system** | The legacy C implementation (`src/`, `ext/*.c`, and the `tool/` code generators) SRS 001 requires to keep working, unmodified, as the acceptance oracle for as long as it coexists with its C++ replacement |
| **Target** | A repository in which the legacy implementation — the core library, every application, and every code generator — is restructured into a clearly named, equally well-packaged `-legacy` track under `applications/`/`libraries/`, standing permanently alongside the `sqlite-cpp` track for direct comparison, with full deletion left as an explicit, later, system-wide decision rather than assumed |

---
## 1. Introduction

### 1.1 Purpose

SRS 001 builds `sqlite-cpp` as a **strangler-fig migration** (SRS 001 §1.3):
the legacy C implementation and its new C++ replacement coexist, with the
legacy code retained, unmodified, as both the running implementation for
not-yet-migrated pieces and the behavioral oracle for pieces that have
already migrated (SRS 001 §1.2, FR-10). This document specifies how that
coexistence is made **structurally explicit and permanently comparable**,
rather than left as an implicit, temporary arrangement with an assumed
delete-it-later endpoint:

- The legacy build target (`libsqlite3`) is renamed to `libsqlite3-legacy`
  and given the same per-library CMake packaging SRS 001 already requires
  of every new library (FR-14/FR-15) — legacy becomes a first-class,
  equally well-structured target, not an unmarked leftover.
- **Every application lives under `applications/`** — including the four
  code generators SRS 001's compiler libraries depend on at build time
  (`lemon`, `mkkeywordhash`, `mkopcodeh`, `mkopcodec`), which move there
  from `tool/`. This document splits applications into two categories,
  because they are blocked on different things and their legacy/new
  relationship is not the same shape:
  - **Sqlite3-dependent applications** (`sqlite3-shell`, `sqldiff`,
    `sqlite3-rsync`, `speedtest1`) link the legacy public C API. Each is
    renamed with a `-legacy` suffix, reserving its original, unsuffixed
    name for the eventual `sqlite-cpp`-linked replacement — which cannot
    exist until the interface façade (SRS 001 FR-7) covers that
    application's dependency surface (§5.4.1). Their C++ replacement is
    necessarily gradual and façade-gated.
  - **Generator applications** (`lemon`, `mkkeywordhash`, `mkopcodeh`,
    `mkopcodec`) do not depend on `sqlite3` at all — they process grammar,
    keyword-list, and opcode-table text at build time. Each is likewise
    renamed with a `-legacy` suffix, but because nothing blocks them (no
    façade dependency, no engine dependency beyond a stable input format),
    each gets a **complete** C++ replacement — full functional parity, not
    a partial or indefinitely-deferred one — built immediately rather than
    gated on any other component's progress (§5.4.2). This revises this
    document's own v0.1 draft, which argued these tools didn't need
    porting at all — see §9.2.

Deleting any of this legacy material outright is **not** a requirement this
document imposes per-component as each replacement lands. It is instead an
explicit, later, system-wide decision (§9.1), made once comparison evidence
(§4, §7) shows the `sqlite-cpp` track has reached parity across everything
the legacy track does — not before, and not piecemeal. The generator
applications are the one place this document requires *complete* parity
rather than merely tracking toward it, precisely because nothing external
blocks them from reaching it.

### 1.2 Scope

**In scope:** renaming and re-packaging `libraries/libsqlite3` as
`libraries/libsqlite3-legacy`; unifying every application — sqlite3-dependent
and generator alike — under `applications/`, with the sqlite3-dependent ones
renamed `-legacy` and their original names reserved for a future,
façade-gated `sqlite-cpp`-linked build, and the generator applications
(`lemon`, `mkkeywordhash`, `mkopcodeh`, `mkopcodec`) renamed `-legacy`
alongside an immediate, complete C++ replacement; and establishing the
comparison infrastructure (differential testing, generator-output diffing)
that makes the legacy and new tracks mechanically comparable rather than
comparable in principle only.

**Out of scope:** re-deciding SRS 001's library boundaries or API-
compatibility guarantees (both stand as-is); performing the SIL4 safety-case
work SRS 003 specifies (this document only ensures the legacy track SRS 003
SR-12 relies on stays available, per §1.5); and the final system-wide
deletion decision itself (§9.1) — this document builds the state that
decision would act on, it does not make the decision.

### 1.3 Definitions, Acronyms, Abbreviations

| Term | Meaning |
|---|---|
| Legacy track | `libsqlite3-legacy` and every `applications/*-legacy` target (both sqlite3-dependent and generator applications) — collectively, everything renamed and frozen under this document |
| New track | The `sqlite-cpp` libraries (SRS 001 §3), the unsuffixed sqlite3-dependent applications once they exist, and the unsuffixed generator applications (which exist immediately, §5.4.2) |
| Sqlite3-dependent application | An application linking the legacy public C API: `sqlite3-shell`, `sqldiff`, `sqlite3-rsync`, `speedtest1`. Its new-track replacement is façade-gated (§5.4.1) |
| Generator application | An application that generates build-time source artifacts and has no `sqlite3` dependency: `lemon`, `mkkeywordhash`, `mkopcodeh`, `mkopcodec`. Its new-track replacement is a complete, unblocked C++ port (§5.4.2) |
| Dual-track | The state of having both a legacy and a new implementation of the same component built, by default, side by side |
| Comparison harness | The differential-testing infrastructure (§4 RR-8/RR-9, §7) that runs both tracks against the same input and reports divergence |
| Generator parity | Byte-for-byte or semantically-equivalent agreement between a legacy generator application's output and its C++ replacement's output on the same input (RR-7) |
| Frozen | A legacy-track component whose sources are not modified except for the rename/build-glue changes this document requires (RR-6) |
| Diversity evidence | SRS 003 SR-12's differential-testing evidence, which this document's comparison harness (RR-8/RR-9) is the concrete mechanism for producing |

### 1.4 References

- [SRS 001](001-sqlite-cpp-modularization.md) — defines the library
  decomposition, namespaces, API-compatibility requirements, and phased plan
  this document's dual-track restructuring is built on top of. This
  document changes none of it.
- [SRS 003](003-sil4-safety-integrity-validation.md) — its SR-12
  ("regression-as-diversity") requires the legacy implementation to remain
  live for differential testing; this document's default posture (keep the
  legacy track permanently, until an explicit deletion decision, §9.1) is,
  if anything, more generous to that requirement than v0.1's per-library
  deletion model was (§9.4).
- `CMakeLists.txt`, `cmake/SqliteFeatures.cmake`, `cmake/SqliteCodegen.cmake`,
  `libraries/libsqlite3/` — the existing CMake conversion this document's
  build-system requirements (§6) rename and re-package, not replace.
- `tool/lemon.c`, `tool/mkkeywordhash.c`, `tool/mkopcodeh.tcl`,
  `tool/mkopcodec.tcl` — the existing generator applications §4 RR-4/RR-5
  require relocating to `applications/` and completely replacing in C++,
  respectively. (`mkopcodeh`/`mkopcodec` are TCL scripts, not C — "complete
  C++ replacement" means re-implementing their generation logic, not a
  literal C-to-C++ translation of an already-C source, which is what
  applies to `lemon.c` and `mkkeywordhash.c` specifically.)
- `test/` (1,190 `.test` files) and `test/testrunner.tcl` — SRS 001's
  unmodified acceptance oracle; §4 RR-8's sqlite3-dependent application
  comparison harness reuses this suite's inputs where applicable (e.g.,
  driving both `sqlite3-shell` and `sqlite3-shell-legacy` the same way
  `testfixture` drives the legacy shell today).

### 1.5 Relationship to SRS 001 and SRS 003

This document is a third, additive layer: SRS 001 says what `sqlite-cpp` is;
SRS 003 says what qualifying it at SIL4 requires; this document says how the
legacy C implementation SRS 001 builds *alongside* it is structured so the
two can be told apart, built together, and compared — on purpose, not by
accident of an unfinished migration. A sqlite3-dependent component's
dual-track comparison (RR-8/RR-9) becomes meaningful once its `sqlite-cpp`
replacement has met SRS 001's acceptance criteria (SRS 001 §10) for its
declared scope (§9.3 — many replacements today cover a deliberately
narrower scope than the legacy code, e.g. the parser's "expressions + single
SELECT," and comparison results outside that declared scope are not
evidence of anything); a generator application's comparison (RR-7) is held
to a stricter, complete-parity bar instead, since RR-5 requires it. Where a
library is also in SIL4 scope under SRS 003, this document's
permanent-by-default retention of the legacy track (§2.1) is what SRS 003
SR-12's differential evidence actually runs against — this document does
not need a special case for SIL4-scoped libraries the way v0.1's
deletion-gated model did, because nothing here is deleted by default in the
first place.

### 1.6 Overview

§2 gives the end state and design goals of the dual-track restructuring. §3
maps every legacy component to its renamed and new-track counterparts. §4
gives general restructuring requirements (RR-1–RR-11). §5 gives
per-component notes, including the applications section split into
sqlite3-dependent (§5.4.1) and generator (§5.4.2) subsections. §6 gives
build-system requirements. §7 gives testing/comparison requirements. §8
gives acceptance criteria. §9 gives constraints, risks, and open issues —
including the still-open amalgamation question and this draft's revision of
its own v0.1 stance on the code generators. §10 gives a phased plan — note
that several phases here do not wait on SRS 001's phased delivery, because
renaming, packaging, and generator-application replacement do not require
the rest of the new implementation to exist yet. §11 gives a traceability
skeleton. §12 is a glossary.

---
## 2. Overall Description

### 2.1 Product Perspective — End State

Two structurally parallel, equally well-packaged sets of targets exist side
by side, by default, in every build: the **legacy track**
(`libsqlite3-legacy` and every `applications/*-legacy` target) and the **new
track** (the `sqlite-cpp` libraries; the unsuffixed sqlite3-dependent
applications, as each gains sufficient façade coverage; and the unsuffixed
generator applications, which exist immediately and completely). Nothing is
deleted as individual components reach parity — that was v0.1's model, and
§9.2 explains why it is superseded. Instead, reaching parity makes a
component's entry in the comparison harness (§4 RR-8/RR-9) start passing
instead of diverging. Full deletion of the legacy track, if and when it
happens, is a single, explicit, system-wide decision (§9.1), made once —
not an accumulation of per-component deletions.

### 2.2 Design Goals

This document uses **the same design approach as SRS 001** for the legacy
track itself, not just for the new one: `libsqlite3-legacy` gets the
identical per-library CMake packaging pattern (`cmake/<Name>Config.cmake.in`,
`CMakePackageConfigHelpers` export, a `sqlite_cpp_require_<name>()` resolver
function in `cmake/SqliteCppDependency.cmake`) that SRS 001 FR-14/FR-15
already mandates for every new library — legacy is packaged as well as new,
not worse. The sqlite3-dependent `-legacy` applications keep their existing
build wiring, relinked to `libsqlite3-legacy` under their new names and
relocated to `applications/` (already their home). The generator
applications relocate from `tool/` into `applications/` as well (RR-4), and
their C++ replacements (RR-5) follow SRS 001's own conventions —
namespaced, header-only, one file per function/type, with their own test
group and documentation — precisely so that, like every other legacy
component, their C originals can be named and treated as legacy rather than
silently assumed permanent.

### 2.3 User Classes

- **Library implementers** — SRS 001's "Contributors," additionally
  responsible for wiring their library's comparison-harness entry (RR-9)
  once their library reaches its declared scope.
- **Build maintainers** — own `CMakeLists.txt` and `cmake/*.cmake`; this
  document's RR-1/RR-2/RR-4 are primarily their responsibility to execute.
- **Sqlite3-dependent application integrators** — own the `-legacy`/
  unsuffixed application pairs (§5.4.1) and the differential harness that
  compares them (RR-8).
- **Generator application maintainers** — a class this document
  introduces: owners of `lemon`/`mkkeywordhash`/`mkopcodeh`/`mkopcodec`'s
  complete C++ replacements (RR-5) and the generator-parity comparison
  (RR-7) between old and new output.
- **Whoever makes the final deletion decision (§9.1)** — not a role this
  document assigns; flagged as an open item for whoever approves this SRS.

### 2.4 Design and Implementation Constraints

- Legacy-track sources are **frozen** (RR-6) once renamed: no behavioral
  change, ever, except the rename and the minimal build-glue changes
  RR-1/RR-2/RR-4 require — a frozen legacy track is what makes the
  comparison harness's results meaningful over time.
- Both tracks build by default wherever both exist (RR-11) — comparison
  evidence must be a normal build-time by-product, not an opt-in extra step
  someone has to remember to run.
- No change to SRS 001's public API/ABI guarantees (FR-7–FR-9) results from
  any renaming in this document — renaming build targets and directories is
  not renaming the C API surface those targets expose.
- Generator applications' C++ replacements (RR-5) follow SRS 001 §4's
  naming/namespacing/header-only conventions exactly, even though the
  generators themselves are build-time tools rather than runtime libraries
  — consistency of convention matters more here than any argument that
  build tools are a special case. Unlike sqlite3-dependent applications,
  their replacement is required to reach full, not partial, parity (RR-5).

### 2.5 Assumptions and Dependencies

- Renaming and re-packaging the legacy track (RR-1/RR-2/RR-4) does **not**
  require any `sqlite-cpp` library or the interface façade to exist first —
  this is a key difference from v0.1's model, which was entirely blocked on
  the façade (§9.2). §10's phased plan reflects that this work, and the
  generator-application replacements (RR-5), can start immediately.
- The comparison harness (RR-8/RR-9) produces meaningful results only
  within each new-track component's declared scope (§9.3) — this document
  assumes each library's own documentation continues to state that scope
  accurately (SRS 001 FR-19), since the harness has no independent way to
  know it. Generator-application parity (RR-7) is held to full agreement
  instead, since RR-5 does not permit a declared partial scope for those.
- A decision on final legacy-track deletion (§9.1) is explicitly deferred;
  this document does not assume it will ever happen, only that if it does,
  it happens system-wide and is recorded as a deliberate decision.

---
## 3. Legacy → New Component Map

| Component | Category | Legacy (renamed) target | New-track target | New-track status |
|---|---|---|---|---|
| Core C library | — | `libsqlite3-legacy` (was `libsqlite3`) | The ten `sqlite-cpp` libraries (SRS 001 §3) collectively, plus the FR-7 façade once built | Partial — see SRS 001 §3 per-library status |
| `sqlite3` shell | Sqlite3-dependent | `applications/sqlite3-shell-legacy` (was `applications/sqlite3-shell`) | `applications/sqlite3-shell` (reserved name, not yet built) | Not started — blocked on façade coverage (§5.4.1) |
| `sqldiff` | Sqlite3-dependent | `applications/sqldiff-legacy` (was `applications/sqldiff`) | `applications/sqldiff` (reserved name, not yet built) | Not started |
| `sqlite3-rsync` | Sqlite3-dependent | `applications/sqlite3-rsync-legacy` (was `applications/sqlite3-rsync`) | `applications/sqlite3-rsync` (reserved name, not yet built) | Not started |
| `speedtest1` | Sqlite3-dependent | `applications/speedtest1-legacy` (was `applications/speedtest1`) | `applications/speedtest1` (reserved name, not yet built) | Not started; also the ongoing performance-comparison tool between tracks (§5.4.1) |
| LALR parser generator | Generator | `applications/lemon-legacy` (moved from `tool/lemon.c`) | `applications/lemon` — complete C++ replacement, unblocked (§5.4.2) | Not started |
| Keyword-hash generator | Generator | `applications/mkkeywordhash-legacy` (moved from `tool/mkkeywordhash.c`) | `applications/mkkeywordhash` — complete C++ replacement, unblocked | Not started |
| Opcode-header generator | Generator | `applications/mkopcodeh-legacy` (moved from `tool/mkopcodeh.tcl`) | `applications/mkopcodeh` — complete C++ replacement, sequenced with VM work (§5.4.2) | Not started |
| Opcode-code generator | Generator | `applications/mkopcodec-legacy` (moved from `tool/mkopcodec.tcl`) | `applications/mkopcodec` — complete C++ replacement, sequenced with VM work | Not started |
| Extensions (SRS 001 §3.6) | — | Remain inside `libsqlite3-legacy` until ported | Namespaced per extension, per SRS 001 §3.6 | Not started |

Every application-category row now lives under `applications/`, split by
whether it depends on `sqlite3` — that dependency is exactly what
determines whether its new-track counterpart is façade-gated
(sqlite3-dependent) or immediate and complete (generator). This table is
the retirement map's successor: where v0.1 listed "legacy files retired,"
this lists "legacy target renamed to" and "new-track target this component
is compared against," since deletion is no longer the per-row event.

---
## 4. Restructuring Requirements

- **RR-1. Legacy library rename and packaging.** `libraries/libsqlite3` is
  renamed to `libraries/libsqlite3-legacy`; its CMake target becomes
  `libsqlite3-legacy`. It receives the same packaging SRS 001 FR-14/FR-15
  requires of new libraries: `cmake/Libsqlite3LegacyConfig.cmake.in`,
  `CMakePackageConfigHelpers`-based export, and a `sqlite_cpp_require_legacy()`
  function in `cmake/SqliteCppDependency.cmake` alongside the existing
  `sqlite_cpp_require_<library>()` functions.
- **RR-2. Sqlite3-dependent application rename.** `applications/sqlite3-shell`,
  `applications/sqldiff`, `applications/sqlite3-rsync`, and
  `applications/speedtest1` are renamed to `applications/sqlite3-shell-legacy`,
  `applications/sqldiff-legacy`, `applications/sqlite3-rsync-legacy`, and
  `applications/speedtest1-legacy` respectively (CMake targets renamed to
  match), each continuing to depend on `libsqlite3-legacy` (RR-1),
  unmodified in behavior.
- **RR-3. Reserved new-track names for sqlite3-dependent applications.**
  The unsuffixed names (`sqlite3-shell`, `sqldiff`, `sqlite3-rsync`,
  `speedtest1`) are reserved for the eventual `sqlite-cpp`-linked
  application and must not be reused for anything else; they do not exist
  as build targets until §5.4.1's façade-coverage condition is met for that
  application.
- **RR-4. Generator application rename and relocation.** `tool/lemon.c`,
  `tool/mkkeywordhash.c`, `tool/mkopcodeh.tcl`, and `tool/mkopcodec.tcl`
  move into `applications/lemon-legacy`, `applications/mkkeywordhash-legacy`,
  `applications/mkopcodeh-legacy`, and `applications/mkopcodec-legacy`
  respectively, each becoming a proper `applications/` target per SRS 001
  FR-20's own convention, unmodified in behavior — this is a relocation and
  rename, not a rewrite (contrast with RR-5).
- **RR-5. Generator applications get a complete C++ replacement,
  unblocked.** Unlike RR-3's sqlite3-dependent applications, each generator
  application's unsuffixed new-track counterpart (`applications/lemon`,
  `applications/mkkeywordhash`, `applications/mkopcodeh`,
  `applications/mkopcodec`) is built immediately, as a **complete** C++
  port — full functional parity with its legacy counterpart, not a
  scoped-down or partial one — since a generator application has no
  `sqlite3` dependency and nothing else in this document blocks it. See
  §5.4.2 for per-tool detail.
- **RR-6. Legacy track is frozen.** Once renamed, no source file under
  `libraries/libsqlite3-legacy/` or any `applications/*-legacy/` directory
  is modified except for the rename and the minimal build-glue changes
  RR-1/RR-2/RR-4 themselves require. This is what keeps the comparison
  harness (RR-8/RR-9) meaningful over time — a legacy track that drifts is
  not a fixed baseline.
- **RR-7. Generator-output parity.** For each generator application, a
  comparison step runs both the legacy tool and its complete C++
  replacement (RR-5) on the same input (the `parse.y` grammar for `lemon`,
  the keyword list for `mkkeywordhash`, the opcode table for
  `mkopcodeh`/`mkopcodec`) and diffs the output. This is a cheaper, more
  tractable parity signal than full-engine differential testing, since
  generator output is deterministic text — it should be wired as an early,
  high-confidence comparison, not deferred to the same timeline as
  engine-level comparisons, and because RR-5 requires completeness here
  (unlike the gradual sqlite3-dependent applications), this comparison
  should reach full, unqualified agreement, not a partial one.
- **RR-8. Sqlite3-dependent application comparison harness.** For each
  sqlite3-dependent application pair where both `X` and `X-legacy` exist, a
  differential-testing harness runs both against the same input corpus and
  reports divergence; wired into CTest as its own target(s) so it runs on
  every build, not as a manual step.
- **RR-9. Library-level comparison.** For each of SRS 001 §3's ten
  libraries, a comparison between `libsqlite3-legacy`'s relevant internal
  behavior and the corresponding `sqlite-cpp` library's public behavior, to
  the extent both can be exercised with equivalent inputs within the new
  library's declared scope (§9.3). This is the concrete mechanism behind
  SRS 003 SR-12's "regression-as-diversity" requirement, made an explicit,
  checkable requirement here rather than left implicit.
- **RR-10. Deletion is a single, later, explicit, system-wide decision.**
  This document does not require deleting any `-legacy` component,
  including generator applications despite RR-5's completeness requirement
  — reaching complete parity makes deletion *possible*, not automatic. If
  and when the legacy track is deleted, it is deleted as a whole (§9.1),
  never as a series of per-component deletions — a partial deletion would
  remove exactly the baseline the still-remaining comparisons depend on.
- **RR-11. Both tracks build by default.** Wherever both a legacy and a
  new-track target exist for a component, both are built by default
  (`ON` in CMake) so comparison evidence (RR-7/RR-8/RR-9) is a routine
  build by-product, not something a developer has to opt into remembering
  to generate.

---
## 5. Per-Component Notes

### 5.1 `libsqlite3-legacy`

Beyond RR-1's rename/packaging: this target's build **must not** be
weakened or feature-reduced relative to today's `libsqlite3` — it remains
the full legacy implementation, since its entire purpose going forward is
serving as an unmodified comparison baseline (RR-6) and, for as long as
needed, SRS 003 SR-12's diversity-evidence source.

### 5.2 `sqlite-backend-os`'s two platform variants

Unaffected by this document directly (`libsqlite3-legacy` still compiles
both `os_unix.c` and `os_win.c` internally, selected the same way it is
today); this note exists only to point out that RR-9's comparison for
`sqlite-backend-os` must be run on both platforms independently, mirroring
SRS 001 FR-6's paradigm-case treatment.

### 5.3 `sqlite-core-command-processor` and `sqlite-compiler-code-generator`

Per SRS 001 §11.2, these two libraries' replacements land together, not
independently. RR-9's comparison for either is only meaningful once **both**
exist and cover the same statement forms — a partial comparison (new
code-generator against legacy `select.c`, say, without the corresponding
command-processor replacement) is not evidence of anything and should not
be reported as if it were.

### 5.4 Applications

Every application lives under `applications/`, split into two categories
that are blocked on different things (§1.1) and therefore have a different
relationship between their `-legacy` and unsuffixed builds.

#### 5.4.1 Sqlite3-dependent applications

The unsuffixed application names (RR-3) become real build targets once
SRS 001's `sqlite-core-interface` façade (FR-7) covers that application's
actual dependency surface: the shell, using the broadest API surface, is
realistically the last to gain its unsuffixed counterpart; `sqldiff` and
`sqlite3-rsync` likely gain theirs earlier, once the façade covers their
narrower surfaces. `speedtest1` is a special case: **both** `speedtest1`
and `speedtest1-legacy` existing simultaneously is the actual point, since
their comparison is SRS 001 acceptance criterion 4's performance check —
`speedtest1-legacy` is not a stepping stone to be superseded, it is a
permanent comparison fixture for as long as performance comparisons matter.

#### 5.4.2 Generator applications

Unlike §5.4.1, these are **not blocked on the façade or any engine
library** — they can be relocated (RR-4), completely replaced (RR-5), and
compared (RR-7) as soon as someone implements them, which is why §10
schedules them early:

- **`lemon` → complete C++ replacement.** `lemon.c` is a self-contained
  LALR(1) parser generator; its C++ replacement lives at
  `applications/lemon` (internal namespacing, e.g. under
  `sqlite::compiler::tooling::lemon`, is an implementation detail for
  whoever builds this, not fixed by this SRS) and must consume the same
  `parse.y` grammar file unmodified, producing `parse.c`/`parse.h` output
  diffable against `applications/lemon-legacy`'s output (RR-7) — with full
  agreement required, not a declared partial scope.
- **`mkkeywordhash` → complete C++ replacement.** Consumes the same
  keyword list; produces `keywordhash.h`; same full-agreement requirement.
- **`mkopcodeh`/`mkopcodec` → complete C++ replacements.** These are TCL
  scripts today, not C — "complete C++ replacement" means re-implementing
  their generation logic (reading `vdbe.c`'s opcode comment annotations and
  producing `opcodes.h`/`.c`) in C++, not translating existing C syntax.
  Functionally, this is the smallest of the four replacements in terms of
  input complexity, but it is tightly coupled to
  `sqlite-core-virtual-machine`'s opcode enumeration (SRS 001 §3.4.1) and
  should be sequenced alongside that library's own work, not treated as
  fully independent of it — "unblocked" (RR-5) means unblocked by the
  façade and by `sqlite3`, not unblocked by every other consideration.

---
## 6. Build System Requirements

- **BR-1.** `CMakeLists.txt` is updated so `add_subdirectory(libraries/libsqlite3)`
  becomes `add_subdirectory(libraries/libsqlite3-legacy)`, and the
  `applications/sqlite3-shell` / `sqldiff` / `sqlite3-rsync` / `speedtest1`
  subdirectory references become their `-legacy` equivalents (RR-1/RR-2).
- **BR-2.** `CMakeLists.txt` gains `add_subdirectory` entries for
  `applications/lemon-legacy`, `applications/mkkeywordhash-legacy`,
  `applications/mkopcodeh-legacy`, and `applications/mkopcodec-legacy`
  (RR-4), and, as each complete C++ replacement lands, for
  `applications/lemon`, `applications/mkkeywordhash`,
  `applications/mkopcodeh`, `applications/mkopcodec` (RR-5) — both on by
  default (RR-11).
- **BR-3.** New `SQLITE_CPP_BUILD_*` options are added for each new-track
  sqlite3-dependent application once it exists (RR-3), following the same
  on-by-default pattern (RR-11) already used for the libraries (e.g.
  `option(SQLITE_CPP_BUILD_SHELL "..." ON)` alongside a
  `SQLITE_CPP_BUILD_SHELL_LEGACY` option that stays on by default too).
- **BR-4.** `cmake/SqliteCppDependency.cmake` gains `sqlite_cpp_require_legacy()`
  (RR-1) and, once the generator-application replacements exist, resolver
  functions for them following the same naming pattern as every other
  `sqlite_cpp_require_<name>()` function.
- **BR-5.** The comparison harness (RR-7/RR-8/RR-9) is wired as its own
  CTest target(s) (e.g. `ctest -R comparison`), separate from each track's
  own unit/oracle tests, so comparison failures are distinguishable from
  ordinary test failures in CI output.

---
## 7. Testing Requirements

- **TR-1. Generator parity (RR-7).** Runs both the legacy and complete C++
  replacement of each generator application on identical input and fails if
  output diverges outside an explicitly documented, reviewed set of
  accepted differences (e.g., a timestamp comment the legacy tool emits
  that is deliberately omitted by the replacement) — since RR-5 requires
  completeness, this check should trend toward zero accepted differences,
  not stabilize around a permanent list of them.
- **TR-2. Sqlite3-dependent application differential testing (RR-8).** Runs
  both applications in each existing pair against a shared input corpus
  (reusing SRS 001's TCL suite inputs where the harness can drive an
  application the same way `testfixture` drives `sqlite3-shell-legacy`
  today) and reports divergence.
- **TR-3. Library-level comparison (RR-9).** For each library with a
  new-track replacement reaching its declared scope, a differential test
  exercises both the legacy internal behavior and the new library's public
  behavior on shared inputs restricted to that declared scope (§9.3) — the
  harness must not silently run inputs outside that scope and report the
  resulting divergence as a defect.
- **TR-4. Legacy freeze check.** A CI check verifies no file under
  `libraries/libsqlite3-legacy/` or any `applications/*-legacy/` directory
  has changed since its renaming commit, other than through an explicitly
  reviewed, documented exception — enforcing RR-6 mechanically rather than
  by convention alone.

---
## 8. Acceptance Criteria

1. `libraries/libsqlite3-legacy` exists, builds, is packaged per RR-1, and
   is bit-for-bit behaviorally identical to today's `libsqlite3` (a rename,
   not a rewrite).
2. `applications/sqlite3-shell-legacy`, `sqldiff-legacy`,
   `sqlite3-rsync-legacy`, and `speedtest1-legacy` exist, build, and are
   behaviorally identical to their pre-rename counterparts.
3. `applications/lemon-legacy`, `mkkeywordhash-legacy`, `mkopcodeh-legacy`,
   and `mkopcodec-legacy` exist under `applications/` (relocated from
   `tool/`, RR-4), and their complete C++ replacements
   (`applications/lemon`, `mkkeywordhash`, `mkopcodeh`, `mkopcodec`, RR-5)
   exist and pass TR-1's generator-parity check with full agreement (or
   only explicitly-reviewed, documented divergences pending closure).
4. The comparison harness (RR-7/RR-8/RR-9, TR-1/TR-2/TR-3) is wired into
   CTest and runs by default, reporting results — generator parity (TR-1)
   expected to trend toward fully passing per RR-5's completeness bar;
   sqlite3-dependent and library-level comparisons (TR-2/TR-3) not
   necessarily all passing yet, since those new-track components' declared
   scopes are still partial, but running and reporting.
5. TR-4's freeze check passes: no unreviewed legacy-track modification has
   occurred since renaming.
6. No decision to delete any part of the legacy track has been made
   implicitly — §9.1 remains an open, explicitly-flagged item, not a
   silently-assumed outcome.

---
## 9. Constraints, Risks, and Open Issues

### 9.1 Final deletion is an open decision, not resolved here

This document deliberately does not require deleting the legacy track,
including the generator applications despite their completeness
requirement (RR-5/RR-10). If and when whoever owns this SRS decides
comparison evidence (§4, §7) shows system-wide parity sufficient to retire
the legacy track, that deletion applies to the whole track at once (RR-10)
and should itself be recorded as an explicit decision — this document only
builds the state that decision would act on. The amalgamation (`sqlite3.c`)
question v0.1 raised in its own §9.1 is subsumed here: `libsqlite3-legacy`
can continue producing it for as long as the legacy track exists, and the
same system-wide-decision logic applies to discontinuing it.

### 9.2 Design history: this revises v0.1's stance on the code generators

v0.1 of this document argued that "retiring the C code" did not require
retiring `lemon`/`mkkeywordhash`/`mkopcodeh`/`mkopcodec`, treating them as
acceptable permanent build-time dependencies distinct from the shipped
implementation. That call is **reversed** in this revision, per explicit
instruction: these generator applications are relocated to `applications/`
and completely replaced in C++ (RR-4/RR-5), and their originals are marked
legacy rather than left as an unexamined exception. This turned out to be a
good trade even independent of the instruction that prompted it: generator
output is deterministic and cheaply diffable (RR-7), so generator parity is
some of the earliest, highest-confidence, and — uniquely among this
document's comparisons — *complete* evidence this whole document can
produce, cheaper than anything in §4 RR-8/RR-9, and available before any
engine library reaches parity.

### 9.3 Comparison evidence is only meaningful within declared scope

Several `sqlite-cpp` libraries currently cover a deliberately narrower
scope than their legacy counterpart (per `docs/index.md`: the parser
handles "expressions + single SELECT," the code generator handles
"expressions only"). RR-9's library-level comparison, and RR-8's
sqlite3-dependent application comparison by extension, must be restricted
to each component's declared scope — running it against out-of-scope input
and reporting the resulting (expected) divergence as a defect would be
noise, not evidence, and risks the comparison harness being distrusted or
ignored once it does matter. This caveat does **not** apply to RR-7's
generator-application parity, which is required to reach full agreement
(§1.5).

### 9.4 Interaction with SRS 003's diversity evidence

Because this document's default is to retain the legacy track indefinitely
(§2.1), rather than v0.1's per-library deletion-once-ready model, it is, if
anything, more accommodating of SRS 003 SR-12's requirement that legacy
code remain live for differential testing — there is no timing race between
"library retirement" and "evidence capture" the way there was in v0.1,
because retirement (in the deletion sense) does not happen per-library at
all under this revision. SRS 003 §1.5 should be read accordingly: its
constraint on this document's timing is now automatically satisfied by
default, and becomes actively relevant only if/when §9.1's system-wide
deletion decision is ever made.

### 9.5 Effort and schedule reality check

Beyond the ten-library, eight-extension, four-sqlite3-dependent-application
effort v0.1 already flagged, this revision adds four generator-application
relocations plus complete C++ replacements (§5.4.2) and a comparison
harness (RR-7/RR-8/RR-9) that has to be built and maintained alongside
every other track. The harness itself is new, ongoing infrastructure — not
a one-time gate — and should be resourced as such. RR-5's completeness bar
for generator applications is a real, non-deferrable scope commitment, not
a "when convenient" item, precisely because this document argues nothing
blocks it.

### 9.6 Dual-track build cost is a real, accepted cost

RR-11's "both tracks build by default" means CI and local build times
roughly double for every component with both a legacy and new-track target.
This is an accepted, deliberate cost of keeping comparison evidence current
without requiring an opt-in step — it should be stated as such rather than
discovered as a surprise the first time someone notices the build got
slower.

---
## 10. Phased Plan

Unlike v0.1's plan, several phases here do **not** wait on SRS 001's phased
delivery, because renaming/packaging the legacy track and replacing the
generator applications require nothing from the rest of the new track to
exist first.

1. **Immediate: legacy rename and packaging.** RR-1/RR-2/BR-1: rename
   `libsqlite3` → `libsqlite3-legacy` and the four sqlite3-dependent
   applications to their `-legacy` names, with full CMake packaging.
   Blocked on nothing.
2. **Immediate: generator-application relocation and replacement.**
   RR-4/RR-5/RR-7/§5.4.2: move `lemon`, `mkkeywordhash`, `mkopcodeh`,
   `mkopcodec` from `tool/` into `applications/*-legacy`, build their
   complete C++ replacements, and stand up generator-parity comparison.
   Blocked only on implementer availability — though `mkopcodeh`/
   `mkopcodec` benefit from being sequenced alongside
   `sqlite-core-virtual-machine` work (§5.4.2).
3. **Library-level comparison, per SRS 001 §12 phase.** As each library
   reaches its SRS 001 acceptance criteria for its declared scope, RR-9's
   comparison harness entry for it is activated (§9.3's scope caveat
   applies throughout).
4. **Interface façade and sqlite3-dependent application cutover.** Once
   SRS 001's `sqlite-core-interface` (phase 5) covers a given application's
   dependency surface, that application's unsuffixed, new-track target is
   built for the first time (RR-3), and RR-8's differential harness
   activates for that pair.
5. **Extensions.** Ported per SRS 001 §3.6's own schedule; RR-9-style
   comparison extended to each as it lands.
6. **System-wide parity review and deletion decision (§9.1).** Once every
   comparison entry passes within its (by then, hopefully full) scope —
   with generator-application parity (RR-7) expected to lead, having no
   scope caveat to wait out — whoever owns this SRS makes the explicit,
   system-wide decision on whether and when to delete the legacy track.
   Not scheduled against a specific date by this document.

---
## 11. Traceability Matrix Skeleton

| Component | This document's section | Comparison mechanism | Blocked on façade? |
|---|---|---|---|
| `libsqlite3-legacy` | §3, §5.1 | RR-9, per-library | No (rename itself); yes (comparison activation, per library) |
| `sqlite3-shell` / `-legacy` | §5.4.1 | RR-8 | Yes — broadest API surface |
| `sqldiff` / `-legacy` | §5.4.1 | RR-8 | Yes — narrower surface, likely earlier |
| `sqlite3-rsync` / `-legacy` | §5.4.1 | RR-8 | Yes |
| `speedtest1` / `-legacy` | §5.4.1 | RR-8 (performance) | Yes, but both intended to coexist permanently |
| `lemon` / `-legacy` | §5.4.2 | RR-7 (generator parity, complete) | No |
| `mkkeywordhash` / `-legacy` | §5.4.2 | RR-7 (complete) | No |
| `mkopcodeh` / `-legacy`, `mkopcodec` / `-legacy` | §5.4.2 | RR-7 (complete) | No (but sequenced with VM work) |
| Extensions | §3 | RR-9, per extension | No (independent per SRS 001 §3.6) |

---
## 12. Glossary

See §1.3 for acronyms and terms specific to this document. SRS 001 §13
remains authoritative for library, namespace, and legacy-file terminology
this document reuses without redefining.
