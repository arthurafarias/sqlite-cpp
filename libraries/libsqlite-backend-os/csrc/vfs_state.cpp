#include <sqlite/backend/os/vfs_state.hpp>

#include "sqlite3.h"

namespace sqlite::backend::os {

VfsState VfsState::find(const char* const name) {
    return VfsState{sqlite3_vfs_find(name)};
}

int VfsState::register_vfs(const bool make_default) {
    if (vfs_ == nullptr) {
        return SQLITE_MISUSE;
    }
    return sqlite3_vfs_register(vfs_, make_default ? 1 : 0);
}

int VfsState::unregister_vfs() {
    if (vfs_ == nullptr) {
        return SQLITE_MISUSE;
    }
    return sqlite3_vfs_unregister(vfs_);
}

} // namespace sqlite::backend::os
