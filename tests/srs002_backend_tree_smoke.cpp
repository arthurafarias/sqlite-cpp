#include <sqlite/backend/tree/shared_cache_state.hpp>

#include "sqlite3.h"

int main() {
    sqlite::backend::tree::SharedCacheState shared_cache;
    if (shared_cache.apply() != SQLITE_OK || shared_cache.enabled()) {
        return 1;
    }
    if (shared_cache.set_enabled(true) != SQLITE_OK
        || !shared_cache.enabled()) {
        return 1;
    }
    return shared_cache.set_enabled(false) == SQLITE_OK
            && !shared_cache.enabled()
        ? 0
        : 1;
}
