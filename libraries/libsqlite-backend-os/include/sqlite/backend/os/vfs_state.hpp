#pragma once

struct sqlite3_vfs;

namespace sqlite::backend::os {

class VfsState {
public:
    explicit VfsState(sqlite3_vfs* vfs = nullptr) noexcept : vfs_(vfs) {}

    [[nodiscard]] VfsState find(const char* name = nullptr);

    int register_vfs(bool make_default = false);
    int unregister_vfs();

    [[nodiscard]] sqlite3_vfs* get() const noexcept { return vfs_; }
    [[nodiscard]] explicit operator bool() const noexcept { return vfs_ != nullptr; }

private:
    sqlite3_vfs* vfs_;
};

} // namespace sqlite::backend::os
