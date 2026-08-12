#pragma once

#include <sqlite/utils/mutex/mutex_concept.hpp>
#include <sqlite/utils/mutex/mutex_id.hpp>

#include <atomic>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Legacy source: mutex_w32.c (SRS S3.3.1). Win32 CRITICAL_SECTION is
// reentrant by the owning thread by construction, unlike pthread_mutex_t --
// so unlike unix::pthread_mutex, there is no separate recursive/non-recursive
// initialization path to select; the `id` is retained only for parity with
// the mutex_type interface and for debug bookkeeping, exactly as
// mutex_w32.c's sqlite3_mutex.id field is.
namespace sqlite::backend::os::windows {

class win32_mutex {
public:
    explicit win32_mutex(sqlite::utils::mutex::mutex_id id = sqlite::utils::mutex::mutex_id::fast)
        : id_(id) {
        InitializeCriticalSection(&section_);
    }

    win32_mutex(const win32_mutex&) = delete;
    win32_mutex& operator=(const win32_mutex&) = delete;
    win32_mutex(win32_mutex&&) = delete;
    win32_mutex& operator=(win32_mutex&&) = delete;

    ~win32_mutex() { DeleteCriticalSection(&section_); }

    void enter() {
        EnterCriticalSection(&section_);
        ++ref_count_;
        owner_ = GetCurrentThreadId();
    }

    bool try_enter() {
        if (!TryEnterCriticalSection(&section_)) return false;
        ++ref_count_;
        owner_ = GetCurrentThreadId();
        return true;
    }

    void leave() {
        --ref_count_;
        LeaveCriticalSection(&section_);
    }

    // winMutexHeld()/winMutexNotheld(): intended for use only inside
    // assert()-style checks.
    [[nodiscard]] bool held() const noexcept {
        return ref_count_ != 0 && owner_ == GetCurrentThreadId();
    }
    [[nodiscard]] bool not_held() const noexcept {
        return ref_count_ == 0 || owner_ != GetCurrentThreadId();
    }

    [[nodiscard]] sqlite::utils::mutex::mutex_id id() const noexcept { return id_; }

private:
    CRITICAL_SECTION section_{};
    sqlite::utils::mutex::mutex_id id_;
    std::atomic<int> ref_count_{0};
    std::atomic<DWORD> owner_{0};
};

static_assert(sqlite::utils::mutex::mutex_type<win32_mutex>);

} // namespace sqlite::backend::os::windows
