#pragma once

#include "registry.hpp"
#include "test_case.hpp"

#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace sqlite::utils::testing {

// SRS FR-11: a test group is named after the element under test (e.g. the
// group covering sqlite::utils::hash::hash_table is named "hash_table", not
// "HashTableTests"). Constructing one self-registers it (FR-2's "self-register
// their test group when included").
struct test_group {
    test_group(std::string name, std::initializer_list<test_case> tests)
        : name_(std::move(name)), tests_(tests) {
        registry().push_back(this);
    }
    virtual ~test_group() = default;

    [[nodiscard]] std::string_view name() const noexcept { return name_; }
    [[nodiscard]] const std::vector<test_case>& tests() const noexcept { return tests_; }

private:
    std::string name_;
    std::vector<test_case> tests_;
};

} // namespace sqlite::utils::testing
