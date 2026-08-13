#pragma once

namespace sqlite::backend::tree {

class SharedCacheState {
public:
    explicit SharedCacheState(bool enabled = false) noexcept
        : enabled_(enabled) {}

    int apply();
    int set_enabled(bool enabled);

    [[nodiscard]] bool enabled() const noexcept { return enabled_; }

private:
    bool enabled_;
};

} // namespace sqlite::backend::tree
