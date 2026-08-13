# Test-only support (not part of the SRS FR-11/FR-12 unit-test surface):
# a comparative benchmark against legacy libsqlite3, not registered as a
# pass/fail ctest case (throughput comparisons are inherently noisy across
# machines/CI runners) except for one loose scaling-shape sanity check --
# see main.cpp and docs/index.md's "Benchmark: algorithm-preservation check
# against libsqlite3" section.
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/SqliteDependency.cmake)
sqlite_require_core()

add_executable(sqlite_backend_tree_benchmark main.cpp)
target_link_libraries(sqlite_backend_tree_benchmark PRIVATE sqlite::backend::tree sqlite3::sqlite3)
target_compile_features(sqlite_backend_tree_benchmark PRIVATE cxx_std_20)

# Registered with ctest (unlike the raw comparative numbers, which are just
# printed) so `ctest -R sqlite-backend-tree-benchmark` gives a fast,
# CI-friendly regression signal: does per-row insert cost still scale like
# O(log n) as row count grows, or has something turned quadratic.
add_test(NAME sqlite-backend-tree-benchmark COMMAND sqlite_backend_tree_benchmark)
set_tests_properties(sqlite-backend-tree-benchmark PROPERTIES LABELS "benchmark")
