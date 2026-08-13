#pragma once

#include <sqlite/core/virtual_machine/aux/mem.hpp>

#include <string>
#include <vector>

// Legacy source: the schema-lookup portions of build.c/pragma.c's `Table`/
// `Column` structs (SRS S3.4.2, sqlite::core::command_processor::schema --
// not built this pass). A real schema needs a catalog backed by
// sqlite-backend-tree, which needs a real on-disk row-record
// (de)serialization format that doesn't exist anywhere in this workspace
// yet. table_descriptor is an explicit, deliberate stand-in: an in-memory
// table (name, ordered columns, in-memory rows), the same kind of
// substitution sqlite-compiler-code-generator's column_resolver/
// variable_resolver callbacks already are for a real Parse context. Wiring
// this up to real B-tree-backed storage is follow-up work, not attempted
// here.
namespace sqlite::core::command_processor {

namespace vm = sqlite::core::virtual_machine;

struct table_descriptor {
    std::string name;
    std::vector<std::string> columns;
    std::vector<std::vector<vm::aux::mem>> rows; // each row: one mem per `columns` entry, same order
};

} // namespace sqlite::core::command_processor
