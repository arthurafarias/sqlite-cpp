#pragma once

#include <vector>

namespace sqlite::utils::testing {

struct test_group;

inline std::vector<const test_group*>& registry() {
    static std::vector<const test_group*> groups;
    return groups;
}

} // namespace sqlite::utils::testing
