#include <sqlite/backend/pager/wal_checkpoint_state.hpp>

#include "sqlite3.h"

namespace sqlite::backend::pager {

int WalCheckpointState::checkpoint(
    const CheckpointMode mode,
    const char* const schema) {
    log_frames_ = -1;
    checkpointed_frames_ = -1;
    if (database_ == nullptr) {
        return SQLITE_MISUSE;
    }
    return sqlite3_wal_checkpoint_v2(
        database_,
        schema,
        static_cast<int>(mode),
        &log_frames_,
        &checkpointed_frames_);
}

} // namespace sqlite::backend::pager
