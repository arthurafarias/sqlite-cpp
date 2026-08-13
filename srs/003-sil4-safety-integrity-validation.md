# Software Requirements Specification

## SIL4 Safety Integrity Validation — `sqlite-cpp`

*SRS 003 — see the [SRS index](index.md) for the full list of SRS documents.
This document is additive to, and assumes, [SRS 001](001-sqlite-cpp-modularization.md),
and interacts with [SRS 002](002-full-c-retirement.md)'s retirement timing — see §1.5.*

| | |
|---|---|
| **Document status** | Draft v0.1 — requirements capture, not yet reviewed or approved |
| **Subject system** | The `sqlite-cpp` libraries and applications defined by SRS 001's decomposition (§3), once each has reached SRS 001's own acceptance criteria (SRS 001 §10) |
| **Target** | Evidence, process, and verification sufficient to qualify each `sqlite-cpp` library — and then the applications built on them — as SIL4-rated software per IEC 61508 |

---
## 1. Introduction

### 1.1 Purpose

SRS 001 specifies *what* `sqlite-cpp` is: a namespaced, header-only C++
decomposition of SQLite that preserves the existing C API and on-disk format.
It does not specify what is required to trust that implementation inside a
system whose failure can kill or injure people — Safety Integrity Level 4
(SIL4), the highest integrity level defined by IEC 61508. This document
specifies that additional layer: the verification techniques, independence
requirements, coverage targets, and evidence artifacts needed to make a
credible SIL4 claim for each `sqlite-cpp` library individually, and then for
the applications assembled from them.

This document does not itself certify anything. IEC 61508 SIL4 is a claim
that must be assessed by a competent, sufficiently independent assessor
(§6.5); this SRS specifies the engineering work that assessment would need to
examine.

### 1.2 Scope

**In scope:** safety-integrity requirements for every library in SRS 001 §3
(`sqlite-utils`, `sqlite-backend-os/-pager/-tree`,
`sqlite-core-virtual-machine/-command-processor/-interface`,
`sqlite-compiler-tokenizer/-parser/-code-generator`), and for the applications
in SRS 001 §9/FR-20 (`sqlite3` shell, `sqldiff`, `sqlite3-rsync`,
`speedtest1`) to the extent each is part of a delivered safety function.
Extensions (SRS 001 §3.6) are in scope only as an opt-in, separately-qualified
addition (§5).

**Out of scope:** performing the system-level Hazard and Risk Analysis (HARA)
that allocates SIL4 to some enclosing system in the first place — that
analysis belongs to whoever integrates `sqlite-cpp` into a specific product
(a railway interlocking, an avionics parameter store, a medical device log),
and only that analysis can say which of `sqlite-cpp`'s functions are actually
safety-related for them. §7 gives representative, SQLite-shaped hazards to
make this document's requirements concrete and testable, but they are
illustrative, not a substitute for a real system HARA. Also out of scope:
engaging and paying for the independent assessor itself (§6.5) — a real
external dependency this document cannot discharge on its own.

### 1.3 Definitions, Acronyms, Abbreviations

| Term | Meaning |
|---|---|
| SIL | Safety Integrity Level (IEC 61508), 1 (lowest) to 4 (highest) |
| SC | Systematic Capability (IEC 61508-3), the software-specific analogue of SIL; SC4 is required to support a SIL4 claim |
| PFH / PFD | Probability of dangerous Failure per Hour (continuous/high-demand mode) / on Demand (low-demand mode) — IEC 61508's quantitative target measures |
| HARA | Hazard and Risk Analysis — identifies hazards and allocates required SIL |
| HFT | Hardware Fault Tolerance — not directly applicable to software, but IEC 61508-3 imports an analogous "techniques and measures" table (Table A.1–A.9) this document draws on |
| MC/DC | Modified Condition/Decision Coverage — structural test-coverage criterion required at SIL4 |
| WCET | Worst-Case Execution Time |
| FSA | Functional Safety Assessment (IEC 61508-1 §8) — the independent review that grants or withholds a SIL claim |
| RTM | Requirements Traceability Matrix |
| T3 tool | IEC 61508-3 §7.4.4 tool classification: a tool whose output could introduce an undetected fault in the safety function and is not itself checked (compilers, build systems) |
| Diverse programming | Independently developed implementations of the same requirement, used to bound common-cause failure — IEC 61508-3 Table A.2 |
| Oracle | Here, the unmodified legacy C implementation plus the TCL test suite (SRS 001 §1.2, §10), used as ground truth for differential testing |

### 1.4 References

- [SRS 001](001-sqlite-cpp-modularization.md) — the functional/architectural
  SRS this document extends; every library, namespace, and file mapping this
  document refers to is defined there, not repeated here.
- [SRS 002](002-full-c-retirement.md) — restructures the legacy C
  implementation into a permanently-retained, clearly-named `-legacy` track
  compared against the new `sqlite-cpp` track; its comparison harness is the
  concrete mechanism behind this document's SR-12 (§1.5).
- IEC 61508:2010, parts 1–7 — generic functional safety of
  electrical/electronic/programmable electronic safety-related systems; part
  3 (software) is this document's primary normative reference. Used as the
  base standard because it is sector-generic; §8 flags that the actual
  target sector's derivative standard (EN 50128 for rail, ISO 26262 for
  automotive — via ASIL D as the nearest analogue to SIL4 — IEC 62304 for
  medical, DO-178C/ED-12C for airborne software via DAL A) is not yet chosen
  and changes some specifics.
- `doc/wal-lock.md`, `doc/pager-invariants.txt`, `doc/vdbesort-memory.md` —
  existing invariant documentation SRS 001 §1.4 already identifies as
  binding on the new implementation; this document treats the invariants
  they state as the starting point for the formal specifications required
  by §3's SR-3.
- SRS 001 §11 ("Constraints, Risks, and Open Issues") — several tensions
  flagged there (§11.1 on `inline`, §11.2 on the code-generator seam, §11.3
  on file-per-symbol) recur here as safety-relevant, not just style,
  concerns; this document cross-references rather than restates them.

### 1.5 Relationship to SRS 001 and SRS 002

SRS 003 is **additive**: it does not change any SRS 001 functional
requirement (FR-1–FR-20), library boundary, namespace, or acceptance
criterion. It adds a second, independent set of requirements — call them
**SR-** (Safety Requirement) and, per library, **SR-\<lib\>-n** — that apply
on top of an SRS 001-conformant implementation. Where the two documents
appear to conflict (for example, SRS 001 §11.1 discouraging forced inlining
of the VDBE loop for performance reasons, versus this document's WCET
determinism requirement, §3 SR-7), §8 states the resolution explicitly rather
than leaving it implicit. A library cannot be SIL4-qualified under this
document until it has met SRS 001's own acceptance criteria (SRS 001 §10) —
safety qualification is a second pass over a functionally-complete,
already-tested library, not a parallel track that could finish first.

SRS 003 also **interacts** with SRS 002 (the legacy/new dual-track
restructuring plan): this document's SR-12 ("regression-as-diversity")
relies on the legacy C implementation remaining live, for a given library,
until that library's differential evidence has been captured. As of SRS
002 v0.2+, this is no longer a source of tension the way it was under that
document's earlier, per-library-deletion draft: SRS 002 now keeps the
entire legacy track (`libsqlite3-legacy` and every `applications/*-legacy`
target) alive by default, with full deletion deferred to a single, later,
explicit, system-wide decision (SRS 002 §9.1) rather than made per library
as each replacement lands. SR-12's evidence is therefore captured against
SRS 002's library-level comparison harness (SRS 002 RR-9) as a matter of
course; the only remaining constraint this document places on SRS 002 is
that its §9.1 system-wide deletion decision, if and when made, must not
occur for any library still relying on this document's SR-12 evidence — a
constraint on that one future decision, not on SRS 002's ordinary phased
plan.

### 1.6 Overview

§2 states the safety context and the standards/targets this document works
against. §3 gives safety requirements that apply to every library uniformly.
§4 gives library-specific validation steps, one subsection per SRS 001 §3
library — the core of "validate each library component." §5 covers
extensions. §6 covers system/application-level validation once every
constituent library is individually qualified. §7 gives a representative
hazard list used to make §3's and §6's requirements testable. §8 states
constraints, risks, and open issues, in the same spirit as SRS 001 §11. §9
gives system-level acceptance criteria. §10 gives a phased plan aligned to
SRS 001 §12. §11 gives a traceability-matrix skeleton. §12 is a glossary.

---
## 2. Overall Description

### 2.1 Safety Context

SQLite is a general-purpose embeddable database library; it has no intrinsic
"safety function." SIL4 is a property of a *system's* ability to perform a
specific safety function within a stated failure-rate target — it is not a
property software has in isolation. This document therefore treats
`sqlite-cpp` as a **software component** that some enclosing system,
elsewhere, allocates a role in performing a SIL4 safety function (for
example: durable, order-preserving storage of interlocking state; a
configuration store consulted before an actuation decision). That allocation,
and the HARA behind it, is the enclosing system's responsibility (§1.2). What
this document owns is: given that allocation, what does `sqlite-cpp` have to
do, and what evidence does it have to produce, for that system's assessor to
accept `sqlite-cpp`'s contribution to the claim.

### 2.2 Applicable Standard and SIL4 Targets

IEC 61508-3 defines required and recommended techniques per SIL via its
Annex A/B tables, and requires a **Systematic Capability** of SC4 for any
software element contributing to a SIL4 function (IEC 61508-3 §7.4.2.9,
Table 3). SC4 requires the highest column of "Highly Recommended"/"Required"
techniques across defect avoidance, defect detection, and independence of
verification — §3's SR-1 through SR-15 are this document's instantiation of
that table for `sqlite-cpp` specifically.

| Mode | IEC 61508-1 Table target for SIL4 |
|---|---|
| Continuous / high-demand | PFH < 10⁻⁹ per hour |
| Low-demand | PFD < 10⁻⁵ to < 10⁻⁴ (per demand, on average) |

These are system-level quantitative targets; software does not have an
independent failure rate in the hardware-reliability sense (IEC 61508-3
Annex D), so `sqlite-cpp`'s contribution to them is qualitative: the
Systematic Capability evidence in §3–§6 is what stands in for a quantitative
PFH figure for the software component.

### 2.3 Safety Lifecycle

This document assumes the IEC 61508-3 software safety lifecycle (a V-model:
software safety requirements → architecture → module design → coding →
module testing → integration testing → validation, with verification
feeding back at every stage). §10's phased plan maps `sqlite-cpp`'s existing
SRS 001 §12 delivery phases onto this V, one phase at a time, rather than
proposing a separate schedule.

### 2.4 User Classes

- **System integrators** — own the system HARA and SIL allocation (§1.2);
  consumers of this document's evidence, not producers of it.
- **Independent assessor** — performs the Functional Safety Assessment
  (§6.5); must have no line responsibility for `sqlite-cpp`'s implementation.
- **Independent V&V team** — performs SR-9's independent verification;
  distinct from, though may overlap in employer with, the implementers, but
  must not verify their own code (§3 SR-9).
- **Library implementers** — SRS 001's "Contributors" (SRS 001 §2.3),
  additionally bound by this document's coding, coverage, and evidence
  requirements once a library enters safety-case scope.
- **Application integrators** — build the applications in §6 on top of
  qualified libraries; responsible for not silently widening a library's
  qualified-use envelope (§3 SR-15's safety manual) without re-assessment.

### 2.5 Design and Implementation Constraints (safety-specific, additive to SRS 001 §2.4)

- No dynamic memory allocation after each library's stated initialization
  phase, or a bounded-allocation arena with a proven upper bound (§3 SR-6).
- No use of C++ exceptions across a library's public API boundary; each
  public function's failure modes are part of its contract (§3 SR-2, SR-10),
  not signaled by an uncaught-exception path whose propagation through a
  header-only, multiply-included implementation is hard to bound.
- MISRA C++ 2023 (or an equivalent, explicitly justified rule set) as the
  static-analysis baseline (§3 SR-4).
- Recursion depth and call-stack depth must be statically bounded on any
  path reachable from the SIL4-qualified surface (§3 SR-5, SR-6; §8 flags
  why this is a real tension with SQLite's recursive-descent parser and
  expression evaluator).
- The toolchain (GCC 16.2, CMake 4.4.2 — SRS 001 §2.4) must be qualified or
  replaced with one that can be (§3 SR-13) before any SIL4 evidence produced
  by it is admissible.

### 2.6 Assumptions and Dependencies

- A system-level HARA exists (or will exist) elsewhere, allocating SIL4 to a
  function that uses `sqlite-cpp`; this document does not perform that
  analysis and its representative hazards (§7) are not a substitute for it.
- An independent assessor will be engaged by whoever owns the SIL4 claim;
  this document cannot obtain that engagement itself (§1.2).
- SRS 001's oracle (unmodified legacy C plus the 1,190-file TCL suite)
  remains available and executable for the duration of the strangler-fig
  coexistence period (SRS 001 §12); §3 SR-12 depends on this.
- The target sector/standard (§1.4) will be confirmed before formal
  certification is attempted; this document is written against generic
  IEC 61508 and flags, rather than silently resolves, points where a
  sector-specific derivative would differ (§8.1).

---
## 3. General Safety Requirements

These apply to every library named in SRS 001 §3, in addition to that
library's SRS 001 functional requirements. §4 adds library-specific steps on
top of this baseline.

- **SR-1. Bidirectional traceability.** Every SRS 001 functional requirement
  (FR-#) and every safety requirement in this document (SR-#, SR-\<lib\>-#)
  is traceable, in an explicit RTM, to the design element(s) that implement
  it, the file(s) that contain it (SRS 001 FR-4's one-header-per-symbol rule
  makes this granular by construction), the test case(s) that verify it, and
  the coverage/analysis evidence that closes it. No safety requirement is
  considered met without an unbroken chain across all four.
- **SR-2. Machine-checkable contracts.** Every public function or type
  carries a precondition/postcondition/invariant contract, checked in a
  dedicated "checked" build configuration (assertions compiled in), in
  addition to its ordinary documentation (SRS 001 FR-19). A contract
  violation in the checked build is a defined, safety-visible failure
  (SR-10), not a silent pass.
- **SR-3. Formal methods on the highest-risk seams.** IEC 61508-3 Table A.2
  rates formal methods "Highly Recommended" at SC4. This SRS requires them,
  specifically, on: (a) the WAL/journal crash-consistency protocol
  (`sqlite::backend::pager`/`::wal`, formalizing the invariants already
  written informally in `doc/wal-lock.md` and `doc/pager-invariants.txt`);
  (b) the B-tree structural invariants (`sqlite::backend::tree`); and (c)
  the command-processor/code-generator seam SRS 001 §11.2 already calls "the
  sharpest boundary in the whole decomposition." Elsewhere, formal methods
  are not required by default (§8.2 explains why applying them uniformly
  across ~154 files' worth of logic is not a credible plan, mirroring SRS
  001 §11.7's effort argument) — but a library may add them voluntarily, and
  §4 flags any further per-library exception to this default.
- **SR-4. Static analysis.** MISRA C++ 2023 (or an equivalent ruleset,
  chosen and justified before work begins) compliance with zero unresolved
  violations; every documented deviation is independently reviewed and
  recorded, not silently suppressed. Every test run — unit and TCL-oracle —
  is also run under UBSan, ASan, and (where applicable to that library's
  concurrency model, e.g. `sqlite::backend::os`, `sqlite::utils::mutex`)
  TSan, clean.
- **SR-5. Structured programming and complexity bounds.** No `goto`
  (barring the narrow, already-idiomatic SQLite cleanup pattern, which must
  be reviewed case-by-case, not blanket-permitted); cyclomatic complexity
  ≤ 10 per function as a default ceiling, exceptions individually justified;
  no unbounded recursion reachable from the SIL4-qualified surface (§8.3).
- **SR-6. Memory and stack determinism.** No dynamic allocation after a
  library's initialization phase, or a proven upper bound on allocation if
  allocation cannot be eliminated; a proven or conservatively measured
  stack-depth bound for every call chain reachable from the qualified
  surface, with a stated margin.
- **SR-7. WCET budget.** Every function on a path reachable within the
  enclosing SIL4 function's real-time budget has an established
  worst-case-execution-time bound (static analysis where tractable,
  measurement-based with justified margin otherwise). This applies with
  particular force to `sqlite::core::virtual_machine`'s opcode-dispatch
  loop — SRS 001 §11.1 already flags that loop as deliberately *not*
  force-inlined for performance reasons; §8.4 states how that decision and
  this WCET requirement are reconciled.
- **SR-8. Structural coverage.** 100% MC/DC of every statement, branch, and
  boolean condition in every header, gathered from the union of the new
  per-library unit tests (SRS 001 FR-11) and an instrumented replay of the
  TCL oracle suite (SRS 001 FR-10). Code not reachable by either is either
  removed or given a test that reaches it — it is not left uncovered with a
  waiver as the default outcome.
- **SR-9. Independence of verification.** Per SC4, the person(s) producing a
  library's SIL4 verification evidence (test authorship, static-analysis
  disposition, code review for safety purposes) must be independent of that
  library's implementer(s) — distinct people, not merely a distinct pull
  request. This is stricter than, and additional to, SRS 001 FR-11's
  per-library testing requirement, which is about functional coverage, not
  reviewer independence.
- **SR-10. Defensive API boundaries.** Every public entry point validates
  its documented preconditions explicitly and, on violation, fails to a
  defined, observable error state — never undefined behavior, and never a
  silent best-effort continuation. This binds SRS 001 FR-7's `extern "C"`
  façade in particular: a malformed call arriving through the legacy C API
  must not be able to reach undefined behavior in the namespaced C++
  implementation underneath it.
- **SR-11. Fault injection and robustness.** Systematic, coverage-tracked
  injection of I/O errors, allocation failures, corrupted pages/WAL frames,
  interrupted ("torn") writes, and truncated/malformed reads at every layer
  boundary. This elevates SRS 001's existing `sqlite::utils::testing`
  fault-injection helpers (legacy `fault.c`, SRS 001 §3.2) from an optional
  developer aid to a required, coverage-tracked safety test suite.
- **SR-12. Regression-as-diversity.** For as long as SRS 001 §12's
  strangler-fig coexistence holds (legacy C and new C++ both present), the
  legacy implementation is retained and used for differential ("back to
  back") testing against the C++ implementation on identical inputs. This is
  claimed as partial evidence toward IEC 61508-3 Table A.2's "diverse
  programming" recommendation. A phase's legacy C path must not be retired
  until this differential evidence for that phase has been captured — SRS
  001 §12's cutover ordering is amended accordingly for any phase in safety
  scope.
- **SR-13. Toolchain qualification.** GCC 16.2 and CMake 4.4.2 (SRS 001
  §2.4) are treated as IEC 61508-3 §7.4.4 T3 tools and qualified before
  their output is used as SIL4 evidence, via one of: a vendor qualification
  kit, a documented "proven in use" dossier, or back-to-back object-code
  comparison against a second, independently developed compiler on
  safety-scoped translation units.
- **SR-14. Configuration management and change impact.** Once a library has
  an issued safety case (§6.6), every subsequent change to it carries a
  documented impact analysis identifying which safety requirements,
  evidence, and MC/DC coverage the change affects, and re-verification is
  scoped to exactly that affected set — never less, and not a blanket full
  re-run by default either.
- **SR-15. Per-library safety manual.** Each library ships a safety manual,
  distinct from SRS 001 FR-17/FR-19's ordinary documentation, stating: its
  safe-use constraints, its assumed operating environment (e.g., which VFS
  implementations it has been validated against), residual risks not
  eliminated by this SRS's verification, and integration obligations placed
  on any caller who wants to preserve the library's SIL4 claim.

---
## 4. Per-Library Validation Steps

Each subsection states library-specific hazards and validation techniques on
top of §3's baseline. Library names and legacy-source mappings are as defined
in SRS 001 §3; they are not repeated here except where needed for context.

### 4.1 `sqlite-utils` (SRS 001 §3.2)

- `sqlite::utils::memory`: prove the allocator's bookkeeping cannot itself
  corrupt adjacent allocations under fault injection (SR-11); if a
  SIL4-qualified caller uses the pluggable-allocator interface with a
  custom allocator, that allocator is explicitly outside this library's
  safety case (state so in its safety manual, SR-15).
- `sqlite::utils::text`: differential-test `mprintf`-style formatting and
  UTF-8/16 conversion against the legacy implementation (SR-12) across a
  fuzz corpus including malformed/overlong encodings, since text conversion
  bugs are a classic source of silent truncation or misinterpretation
  (relevant to hazard H4, §7).
- `sqlite::utils::hash`: prove no unbounded probe-sequence length (ties to
  SR-7's WCET requirement) under adversarial key sequences.
- `sqlite::utils::mutex`: the interface/dispatch layer itself (OS-specific
  bodies are `sqlite-backend-os`'s concern, §4.2) — verify dispatch cannot
  select an unimplemented or mismatched backend silently; this is a
  defensive-boundary case for SR-10.
- `sqlite::utils::random`: PRNG quality is not itself a SIL4 property unless
  the enclosing system's HARA says otherwise, but reproducibility under
  fault injection (does a failed reseed silently degrade to a predictable
  sequence?) is checked under SR-11.
- `sqlite::utils::status`: verify counters cannot overflow silently within
  the qualified operating envelope (bounded per SR-6/SR-7's determinism
  requirements) — an overflowed status counter that a caller uses for a
  safety-relevant threshold decision is a defensive-boundary gap (SR-10).

### 4.2 `sqlite-backend-os` (SRS 001 §3.3.1)

The FR-6 "paradigm case" (two CMake-selected implementations, unix and
Windows) is also this document's paradigm case for **environment-boundary**
hazards: this library's safety evidence is fundamentally bounded by the
qualification of the underlying OS/filesystem, which is out of this
document's scope (§8.5) and must be an explicit, named assumption in its
safety manual (SR-15), not an implicit one.

- `sqlite::backend::os`: verify VFS dispatch cannot silently select a
  different backend than configured (SR-10); every VFS method's documented
  error contract is exercised under fault injection (SR-11) — short reads,
  short writes, `ENOSPC`, permission failures, and simulated `fsync`
  failure/reordering.
- `sqlite::backend::os::unix` / `::windows`: back-to-back differential
  testing is not meaningful between these two (they are alternative
  implementations of the same interface for different platforms, not
  diverse implementations of the same platform — SR-12 does not apply
  between them); instead, each is differentially tested against its own
  legacy C counterpart (`os_unix.c`/`os_win.c`) on its native platform.
  Power-loss/torn-write simulation (SR-11) is required on both.
- `sqlite::backend::os::memory`: verify the in-memory VFS cannot be
  mistaken, via configuration or dispatch error, for a durable backend by a
  caller relying on persistence — a defensive-boundary case (SR-10)
  specific to this variant, since its very purpose (no persistence) is the
  opposite of what the rest of this library promises.

### 4.3 `sqlite-backend-pager` (SRS 001 §3.3.2)

The primary formal-methods target named by SR-3(a).

- `sqlite::backend::pager`: model the page-cache-to-disk and
  journal/WAL-coordination protocol against `doc/pager-invariants.txt`;
  every invariant that document states informally becomes a checked
  contract (SR-2) and a target of the formal model (SR-3).
- `sqlite::backend::pager::wal`: formally verify (SR-3) the WAL commit
  protocol's crash-consistency property — after a crash at any point in the
  frame-write sequence, recovery reconstructs either the pre-transaction or
  post-transaction state, never a mixture — against `doc/wal-lock.md`.
  Fault injection (SR-11) exhaustively covers crash points at each distinct
  protocol phase (not every byte offset, which is intractable; every
  protocol-significant phase boundary, which is what the formal model
  identifies).
- `sqlite::backend::pager::cache`: verify cache eviction cannot return a
  stale or partially-written page to a caller under any interleaving
  reachable from the pager's public API (ties to hazard H1, §7).
- `sqlite::backend::pager::journal`: rollback-journal crash-consistency is
  verified with the same rigor as WAL (above), since both are valid
  journal-mode configurations a SIL4 caller might select; the safety
  manual (SR-15) must state whether both, or only one, are within the
  qualified envelope.

### 4.4 `sqlite-backend-tree` (SRS 001 §3.3.3)

The second formal-methods target (SR-3(b)).

- `sqlite::backend::tree`: formally specify the B-tree's structural
  invariants (page balance, key ordering, free-space accounting) and verify
  every mutating operation preserves them (SR-3); fault-inject (SR-11)
  mid-mutation crashes and confirm the pager-layer recovery (§4.3) always
  restores a structurally valid tree, never a partially-mutated one visible
  to a caller.
- `sqlite::backend::tree::introspection` (`dbstat`/`dbpage`): since these
  are diagnostic virtual tables, not part of the core read/write path,
  classify them by default as **outside** the SIL4-qualified surface unless
  a system integrator's HARA specifically relies on their output for a
  safety decision — state this exclusion explicitly in the safety manual
  (SR-15) rather than qualifying them by default effort.

### 4.5 `sqlite-core-virtual-machine` (SRS 001 §3.4.1)

The primary WCET target (SR-7) and the library SRS 001 §11.1 already singles
out for a performance/`inline` tension this document inherits (§8.4).

- `sqlite::core::virtual_machine`: every opcode handler gets an individual
  WCET bound (SR-7); the dispatch loop itself is verified to have no
  code path whose latency depends on unbounded input (e.g., a `LIKE`/GLOB
  opcode with adversarial pattern length) without an enforced ceiling.
  MC/DC (SR-8) is gathered per-opcode, since SRS 001 FR-4's
  one-file-per-symbol rule already isolates each opcode handler into its
  own header — the coverage granularity this document requires falls out
  of that structure rather than requiring new tooling.
- `sqlite::core::virtual_machine::api`: defensive-boundary checks (SR-10)
  on every `sqlite3_stmt` step/bind/column call — the highest-traffic
  public surface in the whole system, and the one most exposed to caller
  misuse (use-after-finalize, type-mismatched column access).
- `sqlite::core::virtual_machine::aux`: `vdbesort.c`'s external merge sort
  is a bounded-memory concern (SR-6) under adversarially large result sets;
  verify it degrades to bounded temp-file use rather than unbounded memory
  growth, per `doc/vdbesort-memory.md`.

### 4.6 `sqlite-core-command-processor` (SRS 001 §3.4.2)

- `prepare`, `pragma`, `dml`, `query`, `expr`, `schema`, `trigger`,
  `window`, `fkey`, `auth`: verify, for each, that malformed or
  boundary-case input (SRS 001's oracle TCL suite already exercises much of
  this functionally, SR-12 reuses it as safety evidence) cannot reach an
  unbounded or unchecked path — `expr` and `trigger` in particular, since
  expression trees and trigger cascades are SQLite's primary sources of
  recursion depth (§8.3's stack-bound tension is sharpest here).
- The command-processor/code-generator seam (SRS 001 §11.2, this document's
  SR-3(c)): the intermediate representation SRS 001 §11.2 requires
  (a builder/visitor API rather than direct opcode emission) is exactly the
  interface this document's formal model needs a stable surface to reason
  about — do not schedule SR-3(c)'s formal work before SRS 001 §11.2's IR
  design lands; sequencing this any other way makes the formal model a
  moving target.
- `debug` (`treeview.c`): diagnostic-only, excluded from the SIL4-qualified
  surface by default, same treatment as §4.4's introspection tables.

### 4.7 `sqlite-core-interface` (SRS 001 §3.4.3)

The composition root — where SR-10's defensive-boundary requirement matters
most, since every other library's guarantees are only as good as what this
layer lets through.

- `sqlite::core::interface`: object-lifecycle correctness (open/close,
  handle validity) is verified under fault injection (SR-11) including
  failure during `sqlite3_open` itself, a classically under-tested path.
- `sqlite::core::interface::extension`: loadable-extension entry points are
  a trust boundary this document does not extend a SIL4 claim across by
  default — an extension loaded via `sqlite3_load_extension` is, unless
  separately qualified, capable of violating every invariant this SRS
  establishes for the libraries beneath it; state this limitation in the
  safety manual (SR-15) in the strongest terms, since it is the single
  largest hole a system integrator could otherwise miss.
- `::vtab`, `::backup`, `::notify`, `::threads`, `::text`: each gets
  defensive-boundary verification (SR-10) proportionate to its exposure;
  `::threads` additionally requires TSan-clean verification (SR-4) given
  its inherent concurrency.

### 4.8 `sqlite-compiler-tokenizer` (SRS 001 §3.5.1)

- Bounded-time tokenization (SR-7) under adversarial input (extremely long
  identifiers/string literals, deeply nested comments); fuzz corpus (SR-11
  style, applied to a compiler front end rather than an I/O layer) covering
  malformed UTF-8 and encoding edge cases at the lexical level.

### 4.9 `sqlite-compiler-parser` (SRS 001 §3.5.2)

The sharpest instance of §8.3's recursion tension, since SQL grammars are
naturally expressed recursive-descent.

- `sqlite::compiler::parser`: recursion depth bound (SR-5/SR-6) is not
  optional here — either an explicit, provably bounded recursion limit
  enforced before the call that would exceed it, or restructuring the
  parser's recursive productions into an explicit-stack (iterative) form
  for the SIL4-qualified grammar subset. §8.3 states this as an open,
  high-risk item rather than assuming it away.
- `sqlite::compiler::parser::ast`: the AST-construction/schema-mutation
  split with `command_processor::schema` (SRS 001 §3.5.2, §11.2) is
  verified with the same discipline as §4.6's command-processor/
  code-generator seam — both are places where SRS 001 already flags the
  boundary as non-mechanical, and this document requires that non-mechanical
  boundary to be specified, not just implemented, before it counts as SC4
  evidence.

### 4.10 `sqlite-compiler-code-generator` (SRS 001 §3.5.3)

SRS 001 §3.5.3 already calls this "the single largest and riskiest
extraction in the whole plan"; this document adds that it is also the
riskiest from a safety-evidence standpoint, for the same underlying reason —
opcode emission and planning decisions are interleaved in the legacy source,
so the formal model (SR-3(c)) and the MC/DC target (SR-8) are both harder to
state precisely until SRS 001 §11.2's IR exists. No safety-case work is
scheduled here ahead of that IR (§10).

---
## 5. Extension Components (SRS 001 §3.6)

`fts3`/`fts4`/`fts5`, `rtree`, `geopoly`, `session`, `rbu`, `icu`, and the
`dbstat`/`dbpage` virtual tables are, by default, **excluded** from the
SIL4-qualified surface. SRS 001 §3.6 already treats them as a lighter-weight
porting pass than the core; this document adds that "lighter-weight" and
"SIL4-qualified" are incompatible without separately budgeted effort. A
system integrator who needs a specific extension within their safety
function must request it be qualified individually, at which point §3 and a
tailored version of §4 apply to it in full — there is no partial or implicit
qualification.

---
## 6. Application-Level (System) Validation

Applies once every library an application depends on has individually met
§3 and §4 (or is explicitly excluded per §5/library safety manuals, SR-15).

### 6.1 Per-application safety classification

| Application | Classification | Rationale |
|---|---|---|
| `sqlite3` shell | Safety-related only if a system operator uses it to interact with a SIL4 data store in a way the HARA covers; otherwise a maintenance/diagnostic tool outside the safety case | Interactive tool, not normally in the runtime path of a safety function |
| `sqldiff` | Diagnostic-only by default, same treatment | Offline comparison tool |
| `sqlite3-rsync` | Safety-related if used for safety-relevant data replication in the deployed system; otherwise excluded | Depends entirely on deployment use, must be confirmed per system |
| `speedtest1` | Always excluded | Benchmarking-only; SRS 001 §11.5/§9's FR-20 already treats it as a tooling consumer of the C façade, not a delivered function |

This table is a default, not a determination the integrator is bound by —
§1.2 already states that only the system HARA can make this call for a real
deployment; this table exists so §6.2–§6.4 have something concrete to test
against.

### 6.2 Integration testing across qualified components

Individually-qualified libraries can still interact unsafely at their
boundaries. Integration testing verifies the composed system against SRS
001 §3.1's dependency graph, focused specifically on boundary conditions
each individual library's tests could not exercise alone (e.g., a pager-level
fault surfacing correctly through the full command-processor/VDBE/interface
stack to a caller, rather than being absorbed or misreported partway).

### 6.3 System-level hazards specific to `sqlite-cpp` use

See §7 for the representative hazard list; §6.3 requires that every hazard
in that list (or a documented, HARA-driven superset/subset for the real
deployment) has a closed-loop chain of: hazard → safety requirement (§3/§4)
→ verification method → evidence artifact, before the application-level
safety case (§6.6) can be assembled.

### 6.4 System acceptance criteria

Extends SRS 001 §10, adding, for whichever libraries and applications are in
this document's safety scope:

7. Every SR-1–SR-15 requirement in §3 is met, with evidence, for every
   library in scope, and independently reviewed per SR-9.
8. Every library-specific requirement in §4 (or documented exclusion, §5) is
   met for every library in scope.
9. Every hazard in §7 (or the real deployment's HARA-derived equivalent) has
   a closed hazard→requirement→verification→evidence chain (§6.3).
10. The RTM (SR-1) is complete and independently reviewed with no orphaned
    requirement (no FR-# or SR-# without verification evidence) and no
    orphaned test (no test without a requirement it verifies).
11. Toolchain qualification (SR-13) evidence is current for the exact
    compiler/build-system versions used to produce the evidence bundle
    submitted for assessment.
12. An independent Functional Safety Assessment (§6.5) has been performed
    and has not identified an open, unresolved non-conformance.

### 6.5 Independent Functional Safety Assessment

Per IEC 61508-1 §8, a SIL4 claim requires assessment by a party with a
degree of independence appropriate to SIL4 (organizationally independent of
the development team, at minimum). This document's evidence (§3, §4, §6.1–
§6.4) is the input to that assessment, not a substitute for it — no SIL4
claim is valid on the strength of this SRS's requirements being met and
self-attested alone.

### 6.6 Safety case assembly

The safety case is the assembled argument-plus-evidence package: this
document plus SRS 001, the RTM (SR-1), every library's safety manual
(SR-15), the hazard closure table (§6.3/§7), toolchain qualification
evidence (SR-13), and the FSA outcome (§6.5). It is produced once per
qualified scope (a library, then the application built on qualified
libraries) and re-validated per SR-14 on every subsequent change within that
scope.

---
## 7. Representative Hazard List

Illustrative, not a substitute for a real system HARA (§1.2, §6.3). Each row
gives a SQLite-shaped hazard, its effect, and where this document's
requirements address it.

| ID | Hazard | Effect if unmitigated | Addressed by |
|---|---|---|---|
| H1 | Silent data corruption on read (stale, partially-written, or torn page returned without error) | Caller acts on wrong data with no failure signal — the most dangerous class, since there is nothing to detect | §4.3/§4.4 (pager/tree formal models), SR-3, SR-11 |
| H2 | Non-atomic commit under power loss | Partial transaction visible after restart; a safety-relevant record could appear half-written | §4.3 (WAL/journal formal proof), SR-3, SR-11's crash-point matrix |
| H3 | Unbounded resource consumption (stack overflow via deep expression/trigger/parser recursion, or unbounded temp-file growth in sort) | Crash or watchdog timeout in the enclosing system, at a time the system cannot predict | SR-5, SR-6, SR-7; §4.5's `vdbesort` bound, §4.9's parser recursion bound, §8.3 |
| H4 | Incorrect query result silently returned (wrong answer, no error) | Caller has no signal at all — arguably worse than a crash, since a crash is at least detectable by the enclosing system | SR-8 (MC/DC), SR-12 (differential oracle), §4.1's text-conversion fuzzing |
| H5 | API misuse by caller (use-after-close, concurrent misuse without required serialization) reaches undefined behavior | Undefined behavior in a safety-relevant process — unbounded consequence | SR-10 (defensive contracts fail to a defined state) |
| H6 | Toolchain-introduced fault (miscompilation of a correct source) | Every other mitigation is undermined at the object-code level, invisibly | SR-13 (tool qualification / diverse compilation) |
| H7 | VFS/OS-layer misbehavior (e.g., `fsync` that does not actually flush) | Durability guarantees void regardless of how correct `sqlite-cpp` itself is | §4.2, explicitly bounded as an assumption in `sqlite-backend-os`'s safety manual (SR-15), not something this document can close on its own |
| H8 | Loaded extension violates an invariant the core relies on | Every guarantee established for the core libraries is void for that session | §4.7's extension-loading exclusion, stated explicitly rather than left implicit |

---
## 8. Constraints, Risks, and Open Issues

Mirrors SRS 001 §11's approach: several of this document's requirements, read
literally, are in tension with each other, with SRS 001, or with what SQLite
architecturally is. This section states the tensions and how they are
resolved, rather than leaving them for an implementer to discover.

### 8.1 Target sector/standard is unconfirmed

This document is written against generic IEC 61508. If the real deployment
target is rail (EN 50128), automotive (ISO 26262, whose nearest analogue to
SIL4 is ASIL D — the standards are not a direct numeric mapping), medical
(IEC 62304, which does not use "SIL" terminology at all), or airborne
(DO-178C/ED-12C, DAL A), several specifics here (coverage criteria naming,
required Annex tables, assessment-body requirements) would need to be
re-derived against that sector's document, not just relabeled. This should
be confirmed before formal certification work begins, not assumed.

### 8.2 Formal methods cannot be applied uniformly at this codebase's size

Same shape of argument as SRS 001 §11.7: applying SR-3's formal-methods
requirement to the entire ~154-file core rather than the three named
highest-risk seams (pager/WAL, B-tree, command-processor/code-generator) is
not a credible plan. §3 SR-3 scopes formal methods deliberately; expanding
that scope is a separate, explicitly-budgeted decision, not a default.

### 8.3 Recursion-depth bounding is a real design change, not a verification exercise

SQLite's recursive-descent parser (`sqlite-compiler-parser`) and expression
evaluator (`command_processor::expr`) are naturally recursive; SR-5/SR-6's
stack-bound requirement cannot be satisfied by testing alone if the
recursion is genuinely unbounded by input (deeply nested parenthesized
expressions, deeply nested subqueries). Satisfying it requires either an
enforced, rejected-with-error depth ceiling on the qualified grammar subset,
or restructuring the recursive productions into an explicit-stack iterative
form. This is design work, flagged here with the same weight SRS 001 §11.2
gives the code-generator seam — it should be its own reviewed sub-phase
(§10), not folded into ordinary per-library qualification effort.

### 8.4 SR-7's WCET requirement and SRS 001 §11.1's inlining guidance

SRS 001 §11.1 is correct that forcing real inlining of the VDBE dispatch
loop would likely hurt performance and threaten SRS 001's acceptance
criterion 4. SR-7's WCET requirement does not contradict this: a WCET bound
is a property of the *worst-case path's* execution time, established by
analysis or measurement, not a property that requires inlining to establish.
The two requirements are compatible as stated; an implementer who reads
SR-7 as requiring aggressive inlining has misread it, and this section
exists so that misreading gets caught in review rather than in a
performance regression.

### 8.5 Environment-boundary hazards (H7) are only partially closable

`sqlite-backend-os`'s safety evidence is bounded by the underlying OS and
filesystem's own behavior (does `fsync` actually flush; does the filesystem
preserve write ordering). This document requires that boundary be named
explicitly (§4.2, SR-15) rather than requiring `sqlite-cpp` to somehow prove
properties of an OS it does not control — that would not be a credible
requirement, and stating it as one would create false confidence rather than
real safety margin.

### 8.6 Extension exclusion is a scope decision, not a technical limitation

§5's default exclusion of extensions is a deliberate scope-control decision
mirroring SRS 001's own treatment of them as a lighter-weight pass (SRS 001
§3.6). It should not be read as a statement that extensions are
inherently unqualifiable — only that qualifying them by default, alongside
the core, is not a credible plan at this document's current scope, for the
same reason SRS 001 §11.7 gives for the core as a whole.

### 8.7 Effort and schedule reality check

Every library in §4 requires, beyond its SRS 001 functional work: an
independent verification team (SR-9), MC/DC coverage (SR-8), a fault-
injection suite (SR-11), and — for three named seams — formal methods
(SR-3). This is a substantial multiplier on SRS 001's own effort estimate
(SRS 001 §11.7), and the independent Functional Safety Assessment (§6.5) is
an external dependency with its own schedule this document cannot control.
§10's phased plan sequences this work; it does not shrink it.

---
## 9. Acceptance Criteria

See §6.4 for the full, additive list. Restated at top level for visibility:
no library is SIL4-qualified until §3 and §4 (or documented exclusion) are
met with independently-reviewed evidence (SR-9); no application is
SIL4-qualified until every library it depends on is qualified, §6.1–§6.4 are
met, and an independent Functional Safety Assessment (§6.5) has completed
without an open non-conformance.

---
## 10. Phased Plan

Aligned to SRS 001 §12, one phase behind: safety-case work for a given
library begins only once that library has met its own SRS 001 §12 phase's
acceptance criteria, not before, and not in parallel racing to finish first
(§1.5).

1. **Scaffolding safety baseline.** Establish SR-1 (RTM), SR-4 (static
   analysis baseline/tooling), SR-13 (toolchain qualification) against
   `sqlite-utils` once SRS 001 phase 1 completes — this phase, like SRS 001's
   own phase 1, exists to build the template every later phase reuses.
2. **Backend.** `sqlite-backend-os` first (environment-boundary treatment,
   §4.2/§8.5), then `-pager` and `-tree` together with their formal-methods
   work (SR-3(a), SR-3(b)) once each has met its SRS 001 phase 2 milestone.
3. **Virtual machine.** WCET (SR-7) and per-opcode MC/DC (SR-8) once SRS 001
   phase 3's runtime-only VM lands; deliberately before the compiler, same
   rationale SRS 001 §12 gives.
4. **Compiler and command processor.** Tokenizer/parser safety work
   (including §8.3's recursion-bound design work, scheduled as its own
   sub-phase) once SRS 001 phase 4's tokenizer/parser lands; the
   code-generator/command-processor formal model (SR-3(c)) not before SRS
   001 §11.2's IR design is complete and stable (§4.6, §4.10).
5. **Interface façade.** SR-10's defensive-boundary work concentrated here,
   plus the explicit extension-loading exclusion (§4.7, H8) written into the
   safety manual, once SRS 001 phase 5 lands.
6. **Extensions.** Only if a system integrator requests one qualified (§5);
   not a default part of this phase's exit criteria otherwise.
7. **Application/system-level.** §6 in full, once every library the target
   applications depend on is individually qualified; ends with the
   independent FSA (§6.5) and assembled safety case (§6.6).

---
## 11. Traceability Matrix Skeleton

| SRS 001 library (§3) | This document's section | Formal methods required? (SR-3) | Default safety-case status |
|---|---|---|---|
| `sqlite-utils` | §4.1 | No | In scope |
| `sqlite-backend-os` | §4.2 | No | In scope, environment-bounded (§8.5) |
| `sqlite-backend-pager` | §4.3 | Yes — SR-3(a) | In scope |
| `sqlite-backend-tree` | §4.4 | Yes — SR-3(b) | In scope; introspection sub-namespace excluded by default |
| `sqlite-core-virtual-machine` | §4.5 | No (WCET/SR-7 instead) | In scope |
| `sqlite-core-command-processor` | §4.6 | Yes — SR-3(c), gated on SRS 001 §11.2's IR | In scope; `debug` sub-namespace excluded by default |
| `sqlite-core-interface` | §4.7 | No | In scope; extension-loading excluded by default (H8) |
| `sqlite-compiler-tokenizer` | §4.8 | No | In scope |
| `sqlite-compiler-parser` | §4.9 | No (design change per §8.3 instead) | In scope |
| `sqlite-compiler-code-generator` | §4.10 | Yes — SR-3(c), shared with command-processor | In scope, gated as above |
| Extensions (§3.6) | §5 | No | Excluded by default; opt-in per extension |

The full RTM (SR-1) is a generated, evidence-linked artifact maintained
alongside the codebase, not this static table — this table is its skeleton,
fixing the section/library correspondence the generated RTM is keyed to.

---
## 12. Glossary

See §1.3 for acronyms used in this document. SRS 001 §13 remains the
authoritative source for library, namespace, and legacy-file terminology;
this document introduces no new library or namespace names of its own.
