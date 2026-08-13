#pragma once

namespace sqlite::utils {

class RuntimeState {
public:
    explicit RuntimeState(int operation) noexcept : operation_(operation) {}

    int refresh(bool reset = false);

    [[nodiscard]] int operation() const noexcept { return operation_; }
    [[nodiscard]] int current() const noexcept { return current_; }
    [[nodiscard]] int highwater() const noexcept { return highwater_; }

private:
    int operation_;
    int current_ = 0;
    int highwater_ = 0;
};

} // namespace sqlite::utils
