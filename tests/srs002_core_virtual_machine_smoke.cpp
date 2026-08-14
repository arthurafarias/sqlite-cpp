#include <sqlite/core/virtual_machine/expanded_sql_state.hpp>

#include "sqlite3.h"

#include <cstring>

int main() {
    sqlite3* database = nullptr;
    if (sqlite3_open(":memory:", &database) != SQLITE_OK) {
        sqlite3_close(database);
        return 1;
    }

    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(
            database, "SELECT ?1", -1, &statement, nullptr)
        != SQLITE_OK) {
        sqlite3_close(database);
        return 1;
    }
    sqlite3_bind_int(statement, 1, 42);

    const sqlite::core::virtual_machine::ExpandedSqlState expansion{statement};
    char* const expanded = expansion.expand();
    const bool expansion_is_valid =
        expanded != nullptr && std::strcmp(expanded, "SELECT 42") == 0;
    const bool state_is_valid = expansion.statement() == statement;
    sqlite3_free(expanded);

    const int finalize_result = sqlite3_finalize(statement);
    const int close_result = sqlite3_close(database);

    return expansion_is_valid && state_is_valid
            && finalize_result == SQLITE_OK && close_result == SQLITE_OK
        ? 0
        : 1;
}
