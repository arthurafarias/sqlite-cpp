#pragma once

#include "allocator.hpp"

#include <cassert>
#include <cstdint>
#include <cstdlib>

// Legacy source: mem1.c (the SQLITE_SYSTEM_MALLOC allocator -- the default
// build's allocator; SRS S3.2). Ports mem1.c's portable code path: the one
// taken on any platform without Apple's zone allocator or a malloc_usable_size
// equivalent, i.e. plain malloc/realloc/free with an 8-byte size prefix so
// `size()` can report an allocation's length later. The Apple zone-allocator
// and malloc_usable_size/_msize fast paths are legacy-only optimizations of
// the same observable behavior and are deferred rather than ported, per this
// SRS phase's scope (system_allocator's behavior is unaffected either way).
namespace sqlite::utils::memory {

// mem1.c's ROUND8: round a byte count up to the next multiple of 8, the
// alignment sqlite3_malloc() guarantees its callers.
inline constexpr int round8(int n) noexcept {
    return (n + 7) & ~7;
}

// The default allocator: wraps the C library's malloc/realloc/free. Satisfies
// the `allocator` concept (allocator.hpp) so it can be used wherever an
// allocator is required, without inheriting from anything.
class system_allocator {
public:
    void* malloc(int n_bytes) {
        assert(n_bytes > 0);
        auto* p = static_cast<std::int64_t*>(std::malloc(static_cast<std::size_t>(n_bytes) + sizeof(std::int64_t)));
        if (p == nullptr) return nullptr;
        p[0] = n_bytes;
        return p + 1;
    }

    void free(void* prior) {
        assert(prior != nullptr);
        auto* p = static_cast<std::int64_t*>(prior) - 1;
        std::free(p);
    }

    void* realloc(void* prior, int n_bytes) {
        assert(prior != nullptr && n_bytes > 0);
        assert(n_bytes == round8(n_bytes));
        auto* p = static_cast<std::int64_t*>(prior) - 1;
        auto* resized = static_cast<std::int64_t*>(
            std::realloc(p, static_cast<std::size_t>(n_bytes) + sizeof(std::int64_t)));
        if (resized == nullptr) return nullptr;
        resized[0] = n_bytes;
        return resized + 1;
    }

    [[nodiscard]] int size(const void* prior) const {
        assert(prior != nullptr);
        const auto* p = static_cast<const std::int64_t*>(prior) - 1;
        return static_cast<int>(p[0]);
    }

    [[nodiscard]] int roundup(int n) const noexcept { return round8(n); }

    bool init() noexcept { return true; }
    void shutdown() noexcept {}
};

static_assert(allocator<system_allocator>);

} // namespace sqlite::utils::memory
