# Software Requirements Specification

## C to C++ Conversion

*SRS 002 — see the [SRS index](index.md) for the full list of SRS documents.*

| | |
|---|---|
| **Document status** | Draft v0.1 — requirements capture, not yet reviewed or approved |
| **Subject system** | A `libraries/<name>` library built per [SRS 001](001-legacy-restructure-and-dynamic-linking.md): plain C, dynamically linked, populated from `legacy/` |
| **Target system** | The same library, same shared-object boundary, converted to namespaced C++ |

---

## 1. Introduction

### 1.1 Purpose

This document specifies how a library is converted from the plain-C form
[SRS 001](001-legacy-restructure-and-dynamic-linking.md) produces into C++. It applies
**per library**, once that library builds and links dynamically per SRS 001 FR-4 — it is
not a whole-workspace big-bang conversion. A library may be at SRS 001's end state while
its siblings are already through this document, or vice versa; §1.3's dependency order
determines a sensible sequence but this document does not mandate one.

### 1.2 Scope

**In scope:** renaming a library's `.c`/`.h` files to `.cpp`/`.hpp` in place, removing
`extern "C"` linkage, restructuring each function to be a member of the class/struct
that owns the state it mutates (a "state container"), namespacing the result, and
re-wiring that library's CMake target so it still builds and links dynamically.

**Out of scope:** replacing internal data structures with STL equivalents — that's
[SRS 003](003-stl-based-architecture.md), applied only after this document's conversion
lands for a library. This document changes *language and organization*, not
*implementation strategy*.

### 1.3 References

- [SRS 001](001-legacy-restructure-and-dynamic-linking.md) — produces this document's
  starting point (a dynamically linked, plain-C library) and defines the namespace-name
  mapping (§1.3 of that document) reused here unchanged.
- [SRS 003](003-stl-based-architecture.md) — the next pass, applied to this document's
  output.

---

## 2. Functional Requirements

- **FR-1 (File rename).** Every `.c` file in the library's `csrc/` becomes a `.cpp`
  file; every `.h` file becomes a `.hpp` file. Same base name, same relative position —
  this is a rename, not a reorganization; §2.3's grouping happens inside the renamed
  files' content, not by moving files around.

- **FR-2 (No `extern "C"`).** No converted source in the library declares `extern "C"`
  linkage anywhere. This is a deliberate scope narrowing from the previous SRS 001's
  requirement that the flat `sqlite3.h` C API be preserved byte-for-byte via an
  `extern "C"` façade — that requirement is dropped. **Open question, not decided by
  this document:** whether any C-callable public API is still wanted for this workspace
  at all. If one is, it should be a single, explicitly separate façade module scoped
  outside this document's per-library conversion, not something reintroduced piecemeal
  inside converted library internals.

- **FR-3 (State containers).** Every function in the library is grouped with the state
  it mutates into a single type — a class or struct owning that state as data members,
  with the function becoming a member function of that type. A function that mutates no
  persistent state (a pure computation) does not need a container; a function that
  mutates state shared by several other functions in the same file (SQLite's C code
  routinely threads a `sqlite3*`, `Btree*`, `Vdbe*`, or similar handle through many
  functions as an explicit first argument) is the paradigm case this requirement
  targets — the handle's pointee becomes the container's member data, and every
  function taking that handle becomes a method on it. No free functions are left
  operating on state passed in as a raw pointer once this requirement is satisfied for
  a given file.

- **FR-4 (Namespacing).** Every state container and every remaining free function lives
  in a namespace matching the library's position in SRS 001 §1.3's table, e.g.
  `sqlite::backend::tree`, `sqlite::core::command_processor`. This reuses SRS 001's
  namespace mapping exactly — no new namespace names are introduced by this document.

- **FR-5 (Rewiring).** The library's `CMakeLists.txt` is updated so it still compiles as
  a `SHARED` target (SRS 001 FR-4 is unaffected by this document — a library stays
  dynamically linked before and after C++ conversion) and links with `LANGUAGES CXX`
  instead of `C`. Any downstream consumer (another library, an application, a test)
  is updated to call the new namespaced API instead of the old flat C entry points.

---

## 3. Acceptance Criteria

1. The library's `csrc/` directory contains no `.c`/`.h` files, only `.cpp`/`.hpp`.
2. `grep -rn 'extern "C"' libraries/<name>` returns nothing.
3. Every public entry point is reachable only through a namespaced type or function
   matching FR-4.
4. The library's shared object still builds, and everything that linked against its old
   C entry points builds against the new C++ API (or, for anything not yet converted,
   continues linking against the library's still-unconverted dependents unaffected).

---

## 4. Glossary

| Term | Meaning |
|---|---|
| State container | A class/struct grouping a set of functions with the state they mutate, replacing a C handle-plus-free-functions pattern (§2 FR-3) |
