#pragma once

#include <stdexcept>

// Legacy source: none (new -- SRS FR-2/FR-11 test-harness scaffolding, modeled
// on opheap's include/.../testing/*_test.hpp convention per SRS S1.4).
namespace sqlite::utils::testing {

struct test_failure : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace sqlite::utils::testing
