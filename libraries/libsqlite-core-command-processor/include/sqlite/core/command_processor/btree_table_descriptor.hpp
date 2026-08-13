#pragma once

#include "record.hpp"

#include <sqlite/backend/tree/btree.hpp>
#include <sqlite/core/virtual_machine/aux/mem.hpp>

#include <string>
#include <vector>

// A real, on-disk-backed counterpart to table_descriptor.hpp's in-memory
// stand-in: schema (name, ordered columns) is still caller-supplied -- no
// sqlite_master-equivalent catalog exists yet, table/root-page lookup by
// name is not this pass's job -- but rows are read live from a real
// sqlite::backend::tree::btree via a cursor, decoded through record.hpp,
// instead of an in-memory vector.
namespace sqlite::core::command_processor {

namespace tree = sqlite::backend::tree;
namespace vm = sqlite::core::virtual_machine;

struct btree_table_descriptor {
    std::string name;
    std::vector<std::string> columns;
    tree::btree& tree_ref; // caller-owned page_store+btree; must outlive this descriptor
};

// Not a SQL INSERT (sqlite::core::command_processor::dml, real statement
// execution, is out of this pass's scope) -- a small programmatic
// convenience for populating a real table in tests/demos, playing the same
// role manually building table_descriptor::rows already does for the
// in-memory path.
inline void insert_row(tree::btree& t, std::int64_t rowid, const std::vector<vm::aux::mem>& values) {
    std::vector<unsigned char> payload = record::encode_record(values);
    t.insert(rowid, payload.data(), payload.size());
}

} // namespace sqlite::core::command_processor
