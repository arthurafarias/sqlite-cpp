# Software Requirements Specification

## STL-Based Architecture Conversion

*SRS 003 — see the [SRS index](index.md) for the full list of SRS documents.*

| | |
|---|---|
| **Document status** | Draft v0.1 — requirements capture, not yet reviewed or approved |
| **Subject system** | A `libraries/<name>` library converted per [SRS 002](002-cpp-conversion.md): namespaced C++, state containers, dynamically linked |
| **Target system** | The same library, its state containers' internals rebuilt on the C++ Standard Library instead of SQLite's own hand-rolled data structures |

---

## 1. Introduction

### 1.1 Purpose

This document specifies converting each [SRS 002](002-cpp-conversion.md) state
container's internal representation, and its mutators, to a purely C++/STL-based
architecture. It applies **per library**, once that library has landed SRS 002's
conversion — same incremental, library-at-a-time application as SRS 002 itself.

#### TODO: Interpret and make changes in this SRS to accomplish with this design goal.

The implementation should begin from the repl application. It is necessary to begin for the external design to the internal. Because if the utilities library changes, all C code will stop working or it is necessary maintain two interfaces of utilities and down libraries. What is not necessary at this moment. So, the OS library will be the last one to be converted. Including, some core features are already supperseded by stl itself and the aren't necessary.

#### TODO: Remove all code that belongs to sqlite3 library once this repository were converted directly from the source tree from the library

All sqlite3 library code should be in the legacy folder, everything else that belongs to the sqlite3 legacy library should be removed.

### 1.2 Scope

**In scope:** replacing SQLite's own hand-rolled data structures and manual memory
management (inside a state container's data members and the bodies of its member
functions) with STL containers, algorithms, and RAII ownership. The container's
*public* member-function surface (as established by SRS 002 FR-3/FR-4) is not expected
to change shape by this document — this is an internals swap, not a re-architecture of
what each state container exposes.

**Out of scope:** any change to on-disk format, SQL semantics, or observable behavior.
Any change to a state container's public surface that a clean STL-based internal design
turns out to need is a design decision for whoever executes this document against a
specific library, not something this document decides in advance for all of them.

### 1.3 References

- [SRS 002](002-cpp-conversion.md) — produces this document's starting point.

---

## 2. Functional Requirements

- **FR-1 (STL data structures).** Hand-rolled data structures — SQLite's own hash table
  (`Hash`), manually managed growable arrays, intrusive linked lists, and similar — are
  replaced by the STL equivalent matching the structure's actual access pattern:
  `std::unordered_map`/`std::map` for the hash table, `std::vector` for growable arrays,
  `std::list`/`std::vector` for lists depending on whether intrusive links are actually
  load-bearing elsewhere, `std::string` for byte-string handling, `std::variant`/
  `std::optional` for SQLite's tagged-union value types (e.g. `Mem`/`sqlite3_value`),
  and `std::unique_ptr`/`std::shared_ptr` for owned pointers.

- **FR-2 (RAII ownership).** Every state container owns its resources via constructors
  and destructors — no manual `sqlite3_malloc`/`sqlite3_free` (or `malloc`/`free`)
  pairing is left in a converted container's implementation. Resource lifetime is
  expressed by C++ object lifetime, not by explicit alloc/free call pairs.

- **FR-3 (STL-idiomatic mutators).** Where a converted mutator's logic has a direct STL
  algorithm equivalent (iteration replaced by range-`for`, hand-written linear search
  replaced by `std::find`/`std::find_if`, hand-written sort replaced by `std::sort`,
  etc.), the STL form replaces the hand-written loop.

- **FR-4 (Behavior preservation).** On-disk format (page layout, WAL format, journal
  format) and SQL-visible behavior are unchanged — this document is an internal
  architecture change to a library's implementation, not a format or semantics
  redesign.

---

## 3. Open Item: Per-Type STL Mapping

The concrete STL type chosen for a specific SQLite structure (what replaces `Hash`,
what replaces `Mem`'s tagged union, what replaces `Vdbe`'s operand stack) is deferred to
when each library's SRS 002 pass has actually landed, since the right mapping depends on
the shape that pass produces — this document states the *kind* of replacement (FR-1)
without prescribing every library's specific mapping table in advance.

---

## 4. Acceptance Criteria

1. `grep` for the library's own hand-rolled equivalents of `Hash`, manual
   `malloc`/`free` pairs, and similar, inside a converted state container's
   implementation returns nothing.
2. The library's existing tests (per-library unit tests plus the TCL oracle, where
   applicable) pass unmodified against the STL-based internals.
3. No change to any on-disk file produced by the library, verified by comparing output
   against the pre-conversion (SRS 002 end-state) library on the same input.
