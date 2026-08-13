#include <sqlite/utils/runtime_state.hpp>

#include "sqlite3.h"

int main() {
    sqlite::utils::RuntimeState memory{SQLITE_STATUS_MEMORY_USED};
    if (memory.refresh() != SQLITE_OK) {
        return 1;
    }

    if (memory.operation() != SQLITE_STATUS_MEMORY_USED) {
        return 2;
    }
    if (memory.current() < 0 || memory.highwater() < memory.current()) {
        return 3;
    }
    return 0;
}
