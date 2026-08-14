# Software Requirements Specification

## C to C++ Conversion

*SRS 002 — see the [SRS index](index.md) for the full list of SRS documents.*

| | |
|---|---|
| **Document status** | **In progress — five dependency leaves implemented** |
| **Subject system** | A `libraries/<name>` library built per [SRS 001](001-legacy-restructure-and-dynamic-linking.md): plain C, dynamically linked, populated from `legacy/` |
| **Target system** | The same library, same shared-object boundary, converted to namespaced C++ |

---

## 1. Introduction

### 1.1 Purpose

This document specifies how a library is converted from the plain-C form
[SRS 001](001-legacy-restructure-and-dynamic-linking.md) produces into C++. It applies
**per library**, once that library builds and links dynamically per SRS 001 FR-4 — it is
not a whole-workspace big-bang conversion. A library may be at SRS 001's end state while
its siblings are already through this document, or vice versa. Conversion advances from
the leaves of §1.3's dependency graph; the first conversion is therefore the
`sqlite-utils` leaf.

### 1.2 Scope

**In scope:** renaming a library's `.c`/`.h` files to `.cpp`/`.hpp` in place, removing
`extern "C"` linkage, restructuring each function to be a member of the class/struct
that owns the state it mutates (a "state container"), namespacing the result, and
re-wiring that library's CMake target so it still builds and links dynamically.

**Out of scope:** splitting a library's files one-state-container-per-file along
namespace lines — that's [SRS 003](003-file-organization-based-on-cpp-namespacing.md).
Replacing internal data structures with STL equivalents — that's
[SRS 004](004-stl-based-architecture.md), applied only after SRS 003 lands for a
library. This document changes *language*, not *file organization* or *implementation
strategy*.

### 1.3 References

- [SRS 001](001-legacy-restructure-and-dynamic-linking.md) — produces this document's
  starting point (a dynamically linked, plain-C library) and defines the namespace-name
  mapping (§1.3 of that document) reused here unchanged.
- [SRS 003](003-file-organization-based-on-cpp-namespacing.md) — the next pass, applied
  to this document's output.

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

---

## 5. Implementation Status

SRS 002 is implemented through the first five dependency leaves, ending with
`sqlite-core-virtual-machine`:

- Every static file in all five libraries' `csrc` directories is now `.cpp` or
  `.hpp` (`sqlite-core-virtual-machine`'s generated `opcodes.c` stays plain C,
  compiled separately from the facade -- see below), and all five shared
  targets require C++17. The other five split libraries remain C targets at
  the SRS 001 end state.
- `sqlite::utils::RuntimeState` is the installed namespaced state-container API
  for a runtime-status operation and its current/high-water values.
- `sqlite::backend::os::VfsState` is the installed namespaced state-container
  API for finding, registering, and unregistering VFS state.
- `sqlite::backend::pager::WalCheckpointState` is the installed namespaced
  state-container API for running a WAL checkpoint and retaining its frame
  counts.
- `sqlite::backend::tree::SharedCacheState` is the installed namespaced
  state-container API for applying and tracking the process-wide shared-cache
  setting owned by the B-tree layer.
- `sqlite::core::virtual_machine::ExpandedSqlState` is the installed namespaced
  state-container API for expanding a prepared statement's bound SQL text
  (`sqlite3_expanded_sql()`).
- `sqlite::core::virtual_machine::utils::find_next_host_parameter` is this
  leaf's first example of FR-3's other case: a free function namespaced per
  FR-4 but not a container method, because it's a pure computation over its
  arguments rather than a mutator of shared state.
- The unconverted C dependents temporarily consume their existing symbols
  through the single external `cmake/SqliteConvertedCFacade.hpp` compatibility
  boundary. No converted library declares `extern "C"` in owned source.
  `cmake/SqliteCppLibrary.cmake`'s facade `-include` and `-fpermissive` are
  now scoped to `$<COMPILE_LANGUAGE:CXX>` specifically, so a library's
  remaining/generated plain-C sources (like `opcodes.c`) keep compiling
  alongside its newly converted `.cpp` files without seeing C++-only syntax
  they can't parse -- a latent gap the first four (all-`.cpp`, no leftover
  `.c`) never exercised.
- Dedicated smoke tests exercise all five namespaced APIs while linking the
  mixed C/C++ ten-library graph.

The next dependency leaf is not yet claimed as converted.
