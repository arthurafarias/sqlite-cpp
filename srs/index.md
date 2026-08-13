# SRS Index

This directory holds every Software Requirements Specification (SRS) governing
the `sqlite-cpp` refactor of `sqlite-src-3530400`. Each SRS is a standalone
document with its own status, scope, and acceptance criteria; this index is
the authoritative list of which SRS documents exist, in what order they were
adopted, and how they relate to one another.

| # | Document | Status | Subject |
|---|---|---|---|
| 1 | [SRS 001 — Legacy Restructure & Dynamic Library Split](001-legacy-restructure-and-dynamic-linking.md) | **Complete** (FR-1–FR-6 all done) | Reorganizes the original SQLite C source into a single `legacy/` reference copy, splits it into per-purpose libraries (still plain C), and converts the whole workspace — libraries and legacy executables alike — to dynamic linking. |
| 2 | [SRS 002 — C to C++ Conversion](002-cpp-conversion.md) | **In progress** (first four dependency leaves, through `sqlite-backend-tree`, complete) | Converts each split library from plain C to namespaced C++17 in dependency-leaf order. |
| 3 | [SRS 003 — File Organization Based on C++ Namespacing](003-file-organization-based-on-cpp-namespacing.md) | Draft v0.1 — requirements capture, not yet reviewed or approved | Reorganizes a library's already-converted (SRS 002) files one-state-container-per-file across a directory tree mirroring its namespace, and removes remaining preprocessor conditionals, C-linkage artifacts, and free `static` functions. |
| 4 | [SRS 004 — STL-Based Architecture Conversion](004-stl-based-architecture.md) | Draft v0.1 — requirements capture, not yet reviewed or approved | Rebuilds a library's state containers, once reorganized per SRS 003, on the C++ Standard Library — STL containers, RAII ownership, STL algorithms — in place of SQLite's hand-rolled data structures and manual memory management. |

## How the documents relate

Unlike this project's previous SRS set (three independent, additive documents), these
four form a **strict linear pipeline applied per library**: a library starts as part of
the original SQLite C source, is populated and dynamically linked per SRS 001, then
converted to namespaced C++ per SRS 002, then reorganized one-state-container-per-file
per SRS 003, then rebuilt on the STL per SRS 004. Different libraries may sit at
different stages of this pipeline at the same time — there is no requirement that the
whole workspace advance through a stage together — but a given library must finish each
stage before the next one starts on it.

This SRS set supersedes an earlier one entirely: a prior `srs/001-sqlite-cpp-modularization.md`
specified a header-only, fully-inlined C++ rewrite done in a single pass; a prior
`srs/002-full-c-retirement.md` specified a permanent legacy-vs-new comparison track kept
in lockstep; a prior `srs/003-sil4-safety-integrity-validation.md` specified IEC 61508
SIL4 safety-integrity validation on top of both. All three were abandoned, and every
artifact produced under them was removed from the tree (SRS 001 FR-1) rather than
carried forward. SIL4 validation has no replacement in this SRS set and is out of scope
going forward.

## Adding a new SRS

1. Add the file as `srs/NNN-short-slug.md`, `NNN` being the next unused
   three-digit number.
2. Add a row to the table above, in numeric order.
3. State, in the new document's own introduction, how it relates to any SRS
   it depends on or amends.
