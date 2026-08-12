#pragma once

#include <sqlite/utils/mutex/mutex_concept.hpp>
#include <sqlite/utils/mutex/mutex_id.hpp>

#include <atomic>
#include <pthread.h>

#ifdef unix
#undef unix
#endif

// Legacy source: mutex_unix.c (SRS S3.3.1). The real, multi-threaded body
// for sqlite::utils::mutex::mutex_type (SRS S3.2's noop_mutex is the
// single-threaded placeholder that phase's own tests use; this is the body
// SRS S3.2's mutex table says belongs here). A `fast` mutex wraps a plain
// (non-recursive) pthread_mutex_t, exactly as pthreadMutexAlloc() does for
// SQLITE_MUTEX_FAST; a `recursive` one is created with
// PTHREAD_MUTEX_RECURSIVE, matching SQLITE_MUTEX_RECURSIVE.
namespace sqlite::backend::os::unix {

class pthread_mutex {
public:
    explicit pthread_mutex(sqlite::utils::mutex::mutex_id id = sqlite::utils::mutex::mutex_id::fast)
        : id_(id) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        if (id_ == sqlite::utils::mutex::mutex_id::recursive) {
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        }
        pthread_mutex_init(&mutex_, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    pthread_mutex(const pthread_mutex&) = delete;
    pthread_mutex& operator=(const pthread_mutex&) = delete;
    pthread_mutex(pthread_mutex&&) = delete;
    pthread_mutex& operator=(pthread_mutex&&) = delete;

    ~pthread_mutex() { pthread_mutex_destroy(&mutex_); }

    void enter() {
        pthread_mutex_lock(&mutex_);
        ++ref_count_;
        owner_ = pthread_self();
    }

    bool try_enter() {
        if (pthread_mutex_trylock(&mutex_) != 0) return false;
        ++ref_count_;
        owner_ = pthread_self();
        return true;
    }

    void leave() {
        --ref_count_;
        pthread_mutex_unlock(&mutex_);
    }

    // Intended for use only inside assert()-style checks, matching
    // pthreadMutexHeld()/pthreadMutexNotheld()'s documented caveat: on a
    // platform where pthread_equal() is not atomic, these can race. Fine for
    // debug-build assertions, not for real synchronization decisions.
    [[nodiscard]] bool held() const noexcept {
        return ref_count_ != 0 && pthread_equal(owner_, pthread_self());
    }
    [[nodiscard]] bool not_held() const noexcept {
        return ref_count_ == 0 || !pthread_equal(owner_, pthread_self());
    }

    [[nodiscard]] sqlite::utils::mutex::mutex_id id() const noexcept { return id_; }

private:
    pthread_mutex_t mutex_{};
    sqlite::utils::mutex::mutex_id id_;
    std::atomic<int> ref_count_{0};
    std::atomic<pthread_t> owner_{};
};

static_assert(sqlite::utils::mutex::mutex_type<pthread_mutex>);

} // namespace sqlite::backend::os::unix
