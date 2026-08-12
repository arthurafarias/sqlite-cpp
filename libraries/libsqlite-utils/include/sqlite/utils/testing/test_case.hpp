#pragma once

#include "test_context.hpp"

#include <functional>
#include <string>

namespace sqlite::utils::testing {

struct test_case {
    std::string name;
    std::function<void(test_context&)> run;
};

} // namespace sqlite::utils::testing
