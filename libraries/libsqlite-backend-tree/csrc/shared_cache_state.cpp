#include <sqlite/backend/tree/shared_cache_state.hpp>

#include "sqlite3.h"

namespace sqlite::backend::tree {

int SharedCacheState::apply() {
    return sqlite3_enable_shared_cache(enabled_ ? 1 : 0);
}

int SharedCacheState::set_enabled(const bool enabled) {
    enabled_ = enabled;
    return apply();
}

} // namespace sqlite::backend::tree
