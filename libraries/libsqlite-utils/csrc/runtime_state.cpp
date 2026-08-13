#include <sqlite/utils/runtime_state.hpp>

#include "sqlite3.h"

namespace sqlite::utils {

int RuntimeState::refresh(const bool reset) {
    return sqlite3_status(operation_, &current_, &highwater_, reset ? 1 : 0);
}

} // namespace sqlite::utils
