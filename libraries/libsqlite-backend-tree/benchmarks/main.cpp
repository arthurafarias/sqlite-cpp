// Test-only support (not a legacy port): a head-to-head throughput and
// scaling-shape comparison between sqlite::backend::tree and legacy
// libsqlite3 doing the equivalent raw table operations. See
// libraries/libsqlite-backend-tree/docs/index.md's "Benchmark:
// algorithm-preservation check against libsqlite3" section for what this
// is (and isn't) evidence of.
//
// Important asymmetry, stated up front so the numbers aren't
// over-interpreted: libsqlite3's inserts run inside a single transaction
// with its normal durability machinery (journal/WAL), while
// sqlite::backend::tree (via this session's page_store stand-in for the
// not-yet-built sqlite-backend-pager) does plain synchronous pwrite()s with
// no journaling at all. This benchmark is about *algorithmic scaling
// shape* (does time-per-row grow with N the way a balanced tree predicts,
// or does it blow up super-linearly), not a claim that either
// implementation is "faster" in an apples-to-apples sense.
#include <sqlite/backend/tree/btree.hpp>

#include <sqlite3.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

namespace {

using clock_type = std::chrono::steady_clock;

double elapsed_ms(clock_type::time_point start) {
    return std::chrono::duration<double, std::milli>(clock_type::now() - start).count();
}

std::vector<unsigned char> make_payload(std::mt19937_64& rng, std::size_t size) {
    std::vector<unsigned char> payload(size);
    std::uniform_int_distribution<int> dist(0, 255);
    for (auto& b : payload) b = static_cast<unsigned char>(dist(rng));
    return payload;
}

struct timings {
    double insert_ms;
    double lookup_ms;
};

timings run_libsqlite3(const std::string& path, int n, const std::vector<std::vector<unsigned char>>& payloads,
                        const std::vector<std::int64_t>& lookup_order) {
    std::remove(path.c_str());
    sqlite3* db = nullptr;
    sqlite3_open(path.c_str(), &db);
    sqlite3_exec(db, "PRAGMA journal_mode=DELETE;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "CREATE TABLE t(id INTEGER PRIMARY KEY, data BLOB);", nullptr, nullptr, nullptr);

    sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr);
    sqlite3_stmt* insert_stmt = nullptr;
    sqlite3_prepare_v2(db, "INSERT INTO t(id, data) VALUES (?, ?);", -1, &insert_stmt, nullptr);
    auto start = clock_type::now();
    for (int i = 0; i < n; ++i) {
        sqlite3_bind_int64(insert_stmt, 1, i);
        sqlite3_bind_blob(insert_stmt, 2, payloads[static_cast<std::size_t>(i)].data(),
                           static_cast<int>(payloads[static_cast<std::size_t>(i)].size()), SQLITE_STATIC);
        sqlite3_step(insert_stmt);
        sqlite3_reset(insert_stmt);
    }
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    double insert_ms = elapsed_ms(start);
    sqlite3_finalize(insert_stmt);

    sqlite3_stmt* select_stmt = nullptr;
    sqlite3_prepare_v2(db, "SELECT data FROM t WHERE id = ?;", -1, &select_stmt, nullptr);
    start = clock_type::now();
    for (std::int64_t id : lookup_order) {
        sqlite3_bind_int64(select_stmt, 1, id);
        sqlite3_step(select_stmt);
        sqlite3_reset(select_stmt);
    }
    double lookup_ms = elapsed_ms(start);
    sqlite3_finalize(select_stmt);

    sqlite3_close(db);
    std::remove(path.c_str());
    return {insert_ms, lookup_ms};
}

timings run_backend_tree(const std::string& path, int n, const std::vector<std::vector<unsigned char>>& payloads,
                          const std::vector<std::int64_t>& lookup_order) {
    std::remove(path.c_str());
    sqlite::backend::tree::page_store store(path, 4096);
    sqlite::backend::tree::btree tree(store);

    auto start = clock_type::now();
    for (int i = 0; i < n; ++i) {
        const auto& p = payloads[static_cast<std::size_t>(i)];
        tree.insert(i, p.data(), p.size());
    }
    double insert_ms = elapsed_ms(start);

    start = clock_type::now();
    for (std::int64_t id : lookup_order) {
        volatile auto found = tree.find(id);
        (void)found;
    }
    double lookup_ms = elapsed_ms(start);

    std::remove(path.c_str());
    return {insert_ms, lookup_ms};
}

} // namespace

int main() {
    const std::vector<int> sizes = {1000, 5000, 20000};
    constexpr std::size_t payload_size = 64;
    std::mt19937_64 rng(42);

    std::printf("%8s | %14s %14s %8s | %14s %14s %8s\n", "rows", "sqlite3 ins ms", "tree ins ms", "ratio",
                "sqlite3 look ms", "tree look ms", "ratio");
    std::printf("---------|--------------------------------------|--------------------------------------\n");

    std::vector<double> tree_insert_per_row;
    for (int n : sizes) {
        std::vector<std::vector<unsigned char>> payloads;
        payloads.reserve(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i) payloads.push_back(make_payload(rng, payload_size));

        std::vector<std::int64_t> lookup_order(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i) lookup_order[static_cast<std::size_t>(i)] = i;
        std::shuffle(lookup_order.begin(), lookup_order.end(), rng);

        timings sqlite3_t = run_libsqlite3("/tmp/backend_tree_bench_sqlite3.db", n, payloads, lookup_order);
        timings tree_t = run_backend_tree("/tmp/backend_tree_bench_tree.db", n, payloads, lookup_order);
        tree_insert_per_row.push_back(tree_t.insert_ms / n);

        std::printf("%8d | %14.2f %14.2f %7.2fx | %14.2f %14.2f %7.2fx\n", n, sqlite3_t.insert_ms, tree_t.insert_ms,
                    tree_t.insert_ms / sqlite3_t.insert_ms, sqlite3_t.lookup_ms, tree_t.lookup_ms,
                    tree_t.lookup_ms / sqlite3_t.lookup_ms);
    }

    // Loose algorithm-preservation sanity check, not a strict performance
    // gate: per-row insert cost for the largest N should not have blown up
    // by more than an order of magnitude relative to the smallest N. A
    // balanced tree's per-row cost grows with log(n); true quadratic
    // behavior (a real regression, e.g. accidentally re-scanning from the
    // root's sibling on every split) would blow *far* past 10x well before
    // reaching 20x the row count used here.
    double smallest = tree_insert_per_row.front();
    double largest = tree_insert_per_row.back();
    double growth = largest / smallest;
    std::printf("\nper-row insert cost growth from n=%d to n=%d: %.2fx\n", sizes.front(), sizes.back(), growth);
    if (growth > 10.0) {
        std::fprintf(stderr, "FAIL: per-row insert cost grew %.2fx -- looks like a scaling regression, not O(log n)\n",
                     growth);
        return 1;
    }
    std::printf("OK: per-row insert cost growth is consistent with O(log n) scaling, not quadratic\n");
    return 0;
}
