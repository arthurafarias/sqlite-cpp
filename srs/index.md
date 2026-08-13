# SRS Index

This directory holds every Software Requirements Specification (SRS) governing
the `sqlite-cpp` refactor of `sqlite-src-3530400`. Each SRS is a standalone
document with its own status, scope, and acceptance criteria; this index is
the authoritative list of which SRS documents exist, in what order they were
adopted, and how they relate to one another.

| # | Document | Status | Subject |
|---|---|---|---|
| 1 | [SRS 001 — SQLite C++ Modularization](001-sqlite-cpp-modularization.md) | Draft v0.1 — requirements capture, not yet reviewed or approved | Functional/architectural requirements for refactoring the SQLite C core into namespaced, header-only C++ libraries (`sqlite::utils`, `sqlite::backend::*`, `sqlite::core::*`, `sqlite::compiler::*`) while preserving the existing public C API and on-disk format exactly. |
| 2 | [SRS 002 — Complete C Implementation Retirement](002-full-c-retirement.md) | Draft v0.3 — requirements capture, not yet reviewed or approved | Restructures the legacy C implementation into a permanent track under `libraries/`/`applications/`, compared directly against `sqlite-cpp` via a differential-testing harness: `libsqlite3` → `libsqlite3-legacy`; sqlite3-dependent applications (shell, sqldiff, rsync, speedtest1) → `*-legacy`, façade-gated new builds; and generator applications (lemon, mkkeywordhash, mkopcodeh, mkopcodec), which don't depend on `sqlite3`, → `*-legacy` plus an immediate, complete C++ replacement. Final deletion of the legacy track is left an explicit, later, system-wide decision, not a per-component gate. |
| 3 | [SRS 003 — SIL4 Safety Integrity Validation](003-sil4-safety-integrity-validation.md) | Draft v0.1 — requirements capture, not yet reviewed or approved | Safety-integrity (IEC 61508, SIL4) validation requirements applied on top of SRS 001's library decomposition: the verification, evidence, and assessment steps needed to qualify each library component, and then the applications built on them, for use as SIL4-rated software. |

## How the documents relate

All three documents are additive, not alternatives: SRS 001 defines *what*
the `sqlite-cpp` libraries are and *what functionality* they must preserve.
SRS 002 defines how the legacy C implementation SRS 001 keeps running
alongside it is renamed, re-packaged, and permanently compared against its
`sqlite-cpp` replacement — deletion, if it ever happens, is a single,
later, explicit, system-wide decision, not a per-library gate. SRS 003
defines the additional *safety-integrity process and evidence* required
before any of that functionality can be claimed to meet SIL4. SRS 002 and
SRS 003 both key their library-level sections directly to SRS 001 §3's
library table, so all three should be read together — SRS 001 first, for
the architecture; then SRS 002 and SRS 003, which apply to that same
architecture largely independently of each other: SRS 003's SR-12 relies on
the legacy implementation staying live for differential testing, which SRS
002's default (retain the legacy track until an explicit deletion decision)
already provides without SRS 002 needing library-by-library gating on SRS
003's evidence, unlike SRS 002's earlier draft (see SRS 002 §9.2's
design-history note and SRS 003 §1.5).

Numbering is stable once a document is adopted (moved out of Draft status).
Before adoption, numbers may still be adjusted to keep related documents
contiguous — this happened once already: the SIL4 document was drafted
first as SRS 002 and renumbered to SRS 003 when SRS 002's current subject
(C retirement) was added, so that SRS 002 and SRS 003 would sit next to the
SRS 001 material they most directly extend, in delivery order. If a document
is ever superseded outright after adoption, this table's **Status** column
will say so and link to its replacement; it will not be removed or
renumbered at that point.

## Adding a new SRS

1. Add the file as `srs/NNN-short-slug.md`, `NNN` being the next unused
   three-digit number.
2. Add a row to the table above, in numeric order.
3. State, in the new document's own introduction, how it relates to any SRS
   it depends on or amends (see SRS 002's §1.5 or SRS 003's §1.5 for the
   pattern).
