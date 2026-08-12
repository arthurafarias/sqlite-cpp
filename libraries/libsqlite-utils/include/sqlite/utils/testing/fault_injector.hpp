#pragma once

#include <functional>

// Legacy source: fault.c (SRS S3.2). Ports sqlite3BenignMallocHooks /
// sqlite3BeginBenignMalloc / sqlite3EndBenignMalloc: a pair of hooks a test
// harness installs to be notified when the code under test is entering or
// leaving a region where an allocation failure is expected and recoverable
// (e.g. an opportunistic hash-table resize), as opposed to a fatal OOM. Per
// the SRS S3.2 table, this is reclassified under sqlite::utils::testing
// rather than shipped as production surface, since its only consumer is
// fault-injection test infrastructure (SRS S1.3, `testing` namespace).
namespace sqlite::utils::testing {

class fault_injector {
public:
    void set_hooks(std::function<void()> on_begin, std::function<void()> on_end) {
        on_begin_ = std::move(on_begin);
        on_end_ = std::move(on_end);
    }

    void begin_benign() {
        if (on_begin_) on_begin_();
    }

    void end_benign() {
        if (on_end_) on_end_();
    }

private:
    std::function<void()> on_begin_;
    std::function<void()> on_end_;
};

inline fault_injector& global_fault_injector() {
    static fault_injector instance;
    return instance;
}

} // namespace sqlite::utils::testing
