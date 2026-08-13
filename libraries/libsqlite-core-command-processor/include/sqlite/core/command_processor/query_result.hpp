#pragma once

#include <sqlite/core/virtual_machine/aux/mem.hpp>

#include <string>
#include <vector>

// Legacy source: sqlite3_column_*()/sqlite3_column_name()'s observable
// result shape (vdbeapi.c), here returned as a plain value rather than
// pulled from a live sqlite3_stmt -- there is no statement-lifecycle API
// (sqlite-core-interface) yet for this pass to hand a live cursor back
// through (SRS S3.4.2, sqlite::core::command_processor::query).
namespace sqlite::core::command_processor {

namespace vm = sqlite::core::virtual_machine;

struct query_result {
    std::vector<std::string> column_names;
    std::vector<std::vector<vm::aux::mem>> rows; // each row: one mem per `column_names` entry, same order
};

} // namespace sqlite::core::command_processor
