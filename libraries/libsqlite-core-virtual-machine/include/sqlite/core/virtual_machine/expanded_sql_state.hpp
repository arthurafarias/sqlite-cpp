#pragma once

struct sqlite3_stmt;

namespace sqlite::core::virtual_machine {

class ExpandedSqlState {
public:
    explicit ExpandedSqlState(sqlite3_stmt* statement = nullptr) noexcept
        : statement_(statement) {}

    [[nodiscard]] char* expand() const;

    [[nodiscard]] sqlite3_stmt* statement() const noexcept { return statement_; }

private:
    sqlite3_stmt* statement_;
};

} // namespace sqlite::core::virtual_machine
