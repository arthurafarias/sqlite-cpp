#include <sqlite/core/virtual_machine/expanded_sql_state.hpp>

#include "sqlite3.h"

namespace sqlite::core::virtual_machine {

char* ExpandedSqlState::expand() const {
    return sqlite3_expanded_sql(statement_);
}

} // namespace sqlite::core::virtual_machine
