#pragma once

struct sqlite3;

namespace sqlite::backend::pager {

enum class CheckpointMode : int {
    passive = 0,
    full = 1,
    restart = 2,
    truncate = 3,
};

class WalCheckpointState {
public:
    explicit WalCheckpointState(sqlite3* database = nullptr) noexcept
        : database_(database) {}

    int checkpoint(CheckpointMode mode, const char* schema = nullptr);

    [[nodiscard]] sqlite3* database() const noexcept { return database_; }
    [[nodiscard]] int log_frames() const noexcept { return log_frames_; }
    [[nodiscard]] int checkpointed_frames() const noexcept {
        return checkpointed_frames_;
    }

private:
    sqlite3* database_;
    int log_frames_ = -1;
    int checkpointed_frames_ = -1;
};

} // namespace sqlite::backend::pager
