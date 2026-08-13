# sqlite-backend-tree

`sqlite::backend::tree` -- a real table B-tree over SQLite's actual on-disk
page format: the data structure every table's rows (and, in real SQLite,
every index) ultimately live in. Part of phase 2 of the
[phased delivery plan](../../../srs/001-sqlite-cpp-modularization.md#12-phased-delivery-plan)
([SRS 001](../../../srs/001-sqlite-cpp-modularization.md)), picked up after phases 3-4 (VM, tokenizer,
parser, code-generator) because that's the order the work actually happened
in this session, not because the SRS's own phase numbering says so -- see
the root [docs/index.md](../../../docs/index.md) for the true build order.

## Position in the dependency graph

```
sqlite-utils
     |
sqlite-backend-os
     |
sqlite-backend-tree      <- you are here
     |
sqlite-backend-pager     (not yet built -- see "The pager stand-in" below)
```

Links `sqlite::backend::os::unix` directly (through `page_store.hpp`) and
`sqlite::utils` (test harness only).

## Namespaces and legacy source mapping

| Namespace | Header(s) | Legacy source |
|---|---|---|
| `sqlite::backend::tree` | `varint.hpp`, `page_layout.hpp`, `cell.hpp`, `btree.hpp`, `cursor.hpp` | `btree.c`, `btree.h`, `btreeInt.h`; `util.c`'s `sqlite3GetVarint`/`sqlite3PutVarint` |

Not implemented this pass: `sqlite::backend::tree::introspection`
(`dbpage.c`/`dbstat.c`'s `sqlite_dbpage`/`dbstat` virtual tables) -- those
are read-only introspection views over an already-working B-tree, a natural
follow-up once a schema layer exists to register them as virtual tables.

## What's real here

This is not a simplified stand-in structure -- it is SQLite's actual
on-disk format, verified by round-tripping real bytes, not just by
inspection:

- **The exact page header layout** (`page_layout.hpp`): page type byte
  (`0x05`/`0x0d` for interior/leaf table b-tree pages), first-freeblock,
  cell count, cell-content-start, fragmented-free-bytes, and the
  interior-page right-most-pointer, all at their real byte offsets.
- **SQLite's variable-length integer encoding** (`varint.hpp`), ported
  bit-for-bit from `sqlite3GetVarint`/`sqlite3PutVarint` -- every cell's
  payload-length and rowid fields use it.
- **The real leaf/interior cell formats** (`cell.hpp`), including **overflow
  pages** for payloads too large to store locally: a payload that doesn't
  fit gets `min_local_payload()` bytes stored in the cell itself plus a
  4-byte pointer to a linked chain of overflow pages, each carrying a
  4-byte "next page" pointer of its own -- real multi-page payload storage,
  tested with a 5000-byte payload on 512-byte pages
  (`testing/btree_test.hpp`).
- **A real, working 100-byte database header** (`page_layout.hpp`'s
  `database_header`) for page 1: magic string, page size, page count --
  enough that a `page_store`-created file's page 1 starts with genuine
  SQLite header bytes.
- **Real multi-page insert with split-based balancing** (`btree.hpp`):
  inserting past what a single leaf page can hold splits it into two pages
  and promotes a divider key into the parent; when a parent overflows too,
  it splits the same way, recursively, up to and including growing a brand
  new root when the current root itself overflows. Tested with 500 rows
  (both in-order and randomly shuffled insertion, `testing/btree_test.hpp`)
  on deliberately small 512-byte pages, so real splitting and multi-level
  growth are actually exercised, not just theoretically possible -- and
  every row is confirmed independently findable afterward.
- **A real bug this library's own tests caught**: the first working version
  of `write_leaf_page()`/`write_interior_page()` rebuilt page 1's buffer
  from scratch on every write (this library never edits a page in place --
  see "No merge-on-delete" below) without re-populating the database header
  bytes it starts from zero, silently zeroing the page-size/page-count
  fields the moment any row was ever inserted. The very first "insert, close
  the file, reopen it, and check the data is still there" test caught this
  immediately (`page_store::page_size()` came back `0` after reopening).
  Fixed by having `page_store::format_header()` re-stamp the database
  header whenever page 1's content buffer is rebuilt. Concrete evidence for
  why this library's own persistence test isn't a formality: a change that
  passed every insert/find/split/cursor test still corrupted the file the
  moment it was closed and reopened.
- **Real cursor iteration** (`cursor.hpp`) in ascending rowid order across
  multiple pages, tested with 400 randomly-inserted rows checked for no
  gaps and no duplicates.

## Deliberate simplifications, each documented at the point it matters

- **2-way splits, not `balance_nonroot()`'s 3-sibling redistribution.**
  Legacy's balance algorithm considers up to 3 sibling pages and
  redistributes cells among them to minimize how often a new page has to be
  allocated (keeping the tree denser). This library always splits an
  overflowing page exactly in half into two pages. Both produce a
  correctly balanced tree (every leaf at the same depth, O(log n)
  operations) -- this port's tree is just somewhat less full than stock
  SQLite's would be for the same insert sequence.
- **No merge-on-delete.** `erase()` removes a cell and rewrites its page,
  but never merges an under-full page with a sibling or shrinks the tree's
  height back down. A tree with many deletions stays as tall as its
  largest-ever size.
- **The local/overflow payload split point** uses a simpler cutoff (see
  `cell.hpp`'s header comment) than legacy's "surplus" redistribution
  formula. Both are valid instances of the same format; the exact byte
  where a large payload's local/overflow boundary falls can differ from
  what stock SQLite would produce for identical data.
- **The root page number can change** when the tree grows a new level
  (`propagate_split()`). Real SQLite keeps a table's root page number fixed
  forever (copying the *old* root's content to a new page instead, since a
  `sqlite_master` schema entry elsewhere stores that fixed number). There is
  no schema layer yet to hold such a reference, so this simplification is
  currently free; a caller must track `btree::root_page()` itself if it
  needs stability across inserts.
- **Table b-trees only** -- no index b-trees (arbitrary-key, not
  rowid-keyed). A second, structurally different B-tree variant, deferred.
- **No record format.** This library stores and retrieves opaque byte
  payloads keyed by rowid; turning SQL column values into those bytes (and
  back) is `sqlite::core::virtual_machine::aux`'s job (SRS S3.4.1's Mem-cell
  handling), consistent with how real SQLite splits this same
  responsibility between `btree.c` and `vdbemem.c`.

## The pager stand-in

Real SQLite's `Btree` never touches the VFS directly -- it goes through
`Pager` for the page cache, rollback-journal durability, and multi-writer
locking those provide. `sqlite-backend-pager` doesn't exist yet, so
`page_store.hpp` is a deliberately minimal stand-in: real file I/O via
`sqlite::backend::os::unix` directly (itself a stand-in, per that library's
own docs, for eventually going through a portable VFS selection point),
with no page cache (every read/write is a real syscall) and no journaling
(a crash mid-write leaves the file in whatever state the completed writes
left it in). Replacing `page_store`'s internals with a real
`sqlite-backend-pager` dependency, once that library exists, should not
require changing `btree.hpp`'s calls into it -- that was a deliberate design
goal, not just a hope.

One consequence of hardcoding `sqlite::backend::os::unix`: this library's
tests currently only run natively on POSIX hosts (see
`tests/CMakeLists.txt`), unlike `sqlite-backend-os` itself, which is
verified on both platforms (native + Wine). Making `page_store` select
`unix`/`windows` structurally, the way `sqlite-backend-os`'s own consumers
do, is one of the more natural near-term follow-ups.

## Benchmark: algorithm-preservation check against `libsqlite3`

`benchmarks/` compares this library's raw insert/lookup throughput and
*scaling shape* against the legacy C `libsqlite3` doing the equivalent raw
table operations (via `sqlite3_exec`, on an `INTEGER PRIMARY KEY` table with
a `BLOB` column, WITHOUT an index, since this library implements table
b-trees only). The point isn't to prove this port is as fast as 20+ years of
tuned C -- it almost certainly isn't -- it's to catch an
accidentally-quadratic regression: if this library's insert time grew O(n)
per operation instead of O(log n) (e.g. from an algorithmic mistake in
`propagate_split()`), the *shape* of the timing curve as row count scales up
would reveal it even if the *absolute* numbers still looked plausible at
small N. See `benchmarks/README.md` for how to run it and read the output.

## Building and running tests independently

```sh
cmake -S libraries/libsqlite-backend-tree -B build-tree
cmake --build build-tree
ctest --test-dir build-tree
```

Or, as part of the workspace build: `ctest --test-dir build -R sqlite-backend-tree`.

## Consuming this library

```cmake
include(cmake/SqliteCppDependency.cmake)
sqlite_cpp_require_backend_tree()
target_link_libraries(your_target PRIVATE sqlite::backend::tree)
```
