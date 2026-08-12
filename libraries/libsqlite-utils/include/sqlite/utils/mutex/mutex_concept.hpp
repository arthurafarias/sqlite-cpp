#pragma once

#include <concepts>

// Legacy source: mutex.c, mutex.h (SRS S3.2, sqlite::utils::mutex). This
// header is the *interface* half of the split the SRS's sqlite-utils table
// calls out explicitly: "Mutex interface and dispatch only -- OS-specific
// bodies live in sqlite-backend-os". A conforming mutex type satisfies this
// concept structurally; noop_mutex.hpp is the one body this phase provides
// (it needs no OS support), and pthread/Win32-backed bodies are deferred to
// sqlite-backend-os per SRS S3.3.1.
namespace sqlite::utils::mutex {

template<class M>
concept mutex_type = requires(M m) {
    { m.enter() } -> std::same_as<void>;
    { m.try_enter() } -> std::same_as<bool>;
    { m.leave() } -> std::same_as<void>;
    { m.held() } -> std::same_as<bool>;
    { m.not_held() } -> std::same_as<bool>;
};

} // namespace sqlite::utils::mutex
