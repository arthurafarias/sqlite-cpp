# Software Requirements Specification

## File Organization Based on C++ Namespacing

*SRS 003 — see the [SRS index](index.md) for the full list of SRS documents.*

| | |
|---|---|
| **Document status** | Draft v0.1 — requirements capture, not yet reviewed or approved |
| **Subject system** | A `libraries/<name>` library converted per [SRS 002](002-cpp-conversion.md): namespaced C++, state containers, dynamically linked, but still organized as one (or a few) large `.cpp`/`.hpp` files per library |
| **Target system** | The same library, its state containers and free functions split one-per-file across a directory tree mirroring the namespace path SRS 002 FR-4 established, with residual preprocessor conditionals, C-linkage artifacts, and free `static` functions removed |

---

## 1. Introduction

### 1.1 Purpose

This document specifies how a library converted per [SRS 002](002-cpp-conversion.md) is
reorganized so its file layout mirrors its C++ namespace structure, and so the C-era
artifacts SRS 002 didn't target — preprocessor conditionals, remaining C-linkage
declarations, and free `static` functions — are removed. It applies **per library**,
once that library has landed SRS 002's conversion, using the same incremental,
library-at-a-time application as SRS 001 and SRS 002.

This review has the goal of:

1. eliminating all preprocessor defines from the code (eliminating preprocessor
   conditional code paths).
2. removing C linkage externs from the code.
3. removing C-linkage name-mangling dependencies.
4. removing static (file-local) free functions.
5. splitting headers that declare multiple state containers and their functions into
   one header (and one source file) per state container.

### 1.2 Scope

**In scope:** physically reorganizing a library's already-converted (SRS 002)
`.cpp`/`.hpp` files so each state container — and any free function not owned by one —
lives in its own file, in a directory path matching its namespace (e.g.
`sqlite::backend::tree` under `backend/tree/`); removing preprocessor conditionals
(`#ifdef`/`#if defined(...)` build variants), any remaining `extern "C"`/C-linkage
declaration, and any free `static` function left over from SRS 002's conversion.

**Out of scope:** replacing internal data structures with STL equivalents — that's
[SRS 004](004-stl-based-architecture.md), applied only after this document's
reorganization lands for a library. This document changes *file layout and residual C
idioms*, not *implementation strategy*. No change to a state container's public
surface, on-disk format, or SQL-visible behavior is in scope.

### 1.3 References

- [SRS 002](002-cpp-conversion.md) — produces this document's starting point (a
  namespaced C++ library, one or a few large files per library) and defines the
  namespace-name mapping (§1.3/FR-4 of that document) reused here unchanged.
- [SRS 004](004-stl-based-architecture.md) — the next pass, applied to this document's
  output.

---

## 2. Functional Requirements

- **FR-1 (No preprocessor conditionals).** No converted source in the library contains
  a preprocessor conditional (`#if`, `#ifdef`, `#ifndef`, and their `#elif`/`#else`
  branches) governing which code is compiled. A build-time choice previously expressed
  this way is either resolved permanently for this workspace's target configuration
  (the dead branch is deleted) or, if genuinely still a runtime choice, expressed as
  ordinary C++ (a runtime branch, a template parameter, a `constexpr` constant) instead
  of a preprocessor directive. Include guards are not conditionals in this sense and
  are unaffected.

- **FR-2 (No C linkage).** No converted source declares `extern "C"` or otherwise
  requests C linkage for any symbol. This continues SRS 002 FR-2's narrowing — any
  C-linkage declaration left over from an incomplete SRS 002 pass, or reintroduced
  since, is removed.

- **FR-3 (No C-linkage-name dependencies).** No converted source relies on a symbol's C
  (unmangled) name being stable: no `__asm__("...")` symbol-name overrides, no
  `.def`/export-list entries keyed to a flat C name, no dynamic-loading code
  (`dlsym`/`GetProcAddress`) looking up a library's own converted symbols by an
  unmangled name. Callers reach a converted API through its namespaced C++ declaration.

- **FR-4 (No free `static` functions).** No file-local (`static`) free function remains
  in a converted library. Every such function is either promoted to a member function
  of the state container whose data it operates on (SRS 002 FR-3's grouping, applied
  retroactively to functions SRS 002 left as free functions because they weren't the
  paradigm handle-threading case) or, if it mutates no persistent state, kept as a
  non-member function in the library's namespace with internal linkage expressed via an
  anonymous namespace instead of `static`.

- **FR-5 (One state container per file).** No header declares more than one state
  container, and no source file defines more than one state container's member
  functions. Each state container gets its own `.hpp`/`.cpp` pair. Free functions
  (FR-4's non-member case) are grouped by cohesion into their own file rather than
  appended to a state container's file.

- **FR-6 (Namespace-mirrored directory layout).** A file's path under
  `libraries/<name>/csrc` mirrors its namespace, dropping the library's own namespace
  root already implied by `libraries/<name>`: a type in `sqlite::backend::tree` lives
  under `libraries/sqlite-backend-tree/csrc/backend/tree/`, matching SRS 001 §1.3's
  namespace mapping and SRS 002 FR-4 unchanged.

---

## 3. Open Item: Free-Function Grouping Convention

FR-5's "grouped by cohesion" for non-member free functions is deferred to when each
library's pass is actually executed, since the right grouping (one file per logical
concern vs. a single shared `<namespace>_util` file) depends on how many such functions
a given library actually has left after FR-4 is applied — this document states the
constraint (one state container per file) without prescribing a free-function
file-naming scheme for every library in advance.

---

## 4. Acceptance Criteria

1. `grep -rnE '#\s*(if|ifdef|ifndef)\b'` restricted to `libraries/<name>/csrc` returns
   nothing outside include guards.
2. `grep -rn 'extern "C"' libraries/<name>` returns nothing.
3. No file-scope function definition inside `libraries/<name>/csrc` is declared
   `static`.
4. Every header in the library declares exactly one state container (or zero, if it
   only holds free functions or constants), and its path matches FR-6's namespace
   mapping.
5. The library's existing tests (per-library unit tests plus the TCL oracle, where
   applicable) pass unmodified against the reorganized layout.
6. No change to any on-disk file produced by the library, verified by comparing output
   against the pre-reorganization (SRS 002 end-state) library on the same input.
