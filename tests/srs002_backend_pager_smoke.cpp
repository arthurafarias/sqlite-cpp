#include <sqlite/backend/pager/wal_checkpoint_state.hpp>

#include "sqlite3.h"

int main() {
    sqlite3* database = nullptr;
    if (sqlite3_open(":memory:", &database) != SQLITE_OK) {
        sqlite3_close(database);
        return 1;
    }

    sqlite::backend::pager::WalCheckpointState checkpoint{database};
    const int checkpoint_result =
        checkpoint.checkpoint(sqlite::backend::pager::CheckpointMode::passive);
    const bool state_is_valid = checkpoint.database() == database;
    const int close_result = sqlite3_close(database);

    return checkpoint_result == SQLITE_OK && state_is_valid
               && close_result == SQLITE_OK
        ? 0
        : 1;
}
