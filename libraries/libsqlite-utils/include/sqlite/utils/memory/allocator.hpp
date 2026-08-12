#pragma once

#include <concepts>
#include <cstddef>

// Legacy source: mem0.c, mem1.c, mem2.c, mem3.c, mem5.c, malloc.c (SRS S3.2).
// This header replaces the `sqlite3_mem_methods` C vtable with a C++20
// `concept`, per the SRS S2.4 constraint that concepts stand in for the
// preprocessor interface contracts the legacy code expressed only informally
// (a struct of function pointers with no compile-time shape checking).
namespace sqlite::utils::memory {

// Mirrors sqlite3_mem_methods' seven entry points (xMalloc, xFree, xRealloc,
// xSize, xRoundup, xInit, xShutdown). A conforming allocator type satisfies
// this concept structurally -- no base class or registration step required.
template<class A>
concept allocator = requires(A a, const A ca, void* p, int n) {
    { a.malloc(n) } -> std::same_as<void*>;
    { a.free(p) } -> std::same_as<void>;
    { a.realloc(p, n) } -> std::same_as<void*>;
    { ca.size(p) } -> std::same_as<int>;
    { ca.roundup(n) } -> std::same_as<int>;
    { a.init() } -> std::same_as<bool>;
    { a.shutdown() } -> std::same_as<void>;
};

} // namespace sqlite::utils::memory
