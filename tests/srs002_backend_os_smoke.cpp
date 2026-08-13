#include <sqlite/backend/os/vfs_state.hpp>

int main() {
    const auto vfs = sqlite::backend::os::VfsState::find();
    return vfs ? 0 : 1;
}
