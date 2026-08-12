#pragma once

#include "fault_injector.hpp"
#include "test_group.hpp"

namespace sqlite::utils::testing {

inline const test_group fault_injector_tests("fault_injector", {
    {"begin_benign/end_benign are no-ops when no hooks are installed", [](auto& ctx) {
        fault_injector injector;
        injector.begin_benign();
        injector.end_benign();
        ctx.check(true);
    }},
    {"begin_benign/end_benign invoke the installed hooks", [](auto& ctx) {
        fault_injector injector;
        int begins = 0, ends = 0;
        injector.set_hooks([&] { ++begins; }, [&] { ++ends; });
        injector.begin_benign();
        injector.begin_benign();
        injector.end_benign();
        ctx.equal(begins, 2);
        ctx.equal(ends, 1);
    }},
    {"global_fault_injector returns the same instance across calls", [](auto& ctx) {
        ctx.check(&global_fault_injector() == &global_fault_injector());
    }},
});

} // namespace sqlite::utils::testing
