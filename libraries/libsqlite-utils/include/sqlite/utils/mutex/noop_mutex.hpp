#pragma once

#include "mutex_concept.hpp"
#include "mutex_id.hpp"

#include <cassert>

// Legacy source: mutex_noop.c's SQLITE_DEBUG variant (SRS S3.2). Provides no
// mutual exclusion -- suitable only for single-threaded use, exactly as
// mutex_noop.c documents -- but does the same entry-count bookkeeping the
// legacy debug build does, so held()/not_held() remain meaningful inside
// assert()-style checks elsewhere in a single-threaded build. A real
// multi-threaded body (pthread/Win32-backed) is deferred to
// sqlite-backend-os, per this header's note in mutex_concept.hpp.
namespace sqlite::utils::mutex {

class noop_mutex {
public:
    explicit noop_mutex(mutex_id id = mutex_id::fast) : id_(id) {}

    void enter() {
        assert(id_ == mutex_id::recursive || not_held());
        ++count_;
    }

    bool try_enter() {
        enter();
        return true;
    }

    void leave() {
        assert(held());
        --count_;
    }

    [[nodiscard]] bool held() const noexcept { return count_ > 0; }
    [[nodiscard]] bool not_held() const noexcept { return count_ == 0; }
    [[nodiscard]] mutex_id id() const noexcept { return id_; }

private:
    mutex_id id_;
    int count_ = 0;
};

static_assert(mutex_type<noop_mutex>);

} // namespace sqlite::utils::mutex
