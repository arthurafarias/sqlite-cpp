#pragma once

#include "../../testing/test_group.hpp"
#include "../hash_table.hpp"

#include <string>

namespace sqlite::utils::hash::testing {

using sqlite::utils::testing::test_group;

inline const test_group hash_table_tests("hash_table", {
    {"find on an empty table returns nullptr", [](auto& ctx) {
        hash_table<int> table;
        ctx.check(table.find("missing") == nullptr);
        ctx.equal(table.count(), 0u);
    }},
    {"insert then find round-trips", [](auto& ctx) {
        hash_table<int> table;
        int value = 42;
        ctx.check(table.insert("answer", &value) == nullptr);
        ctx.equal(table.find("answer"), &value);
        ctx.equal(table.count(), 1u);
    }},
    {"insert with an existing key replaces and returns the old data", [](auto& ctx) {
        hash_table<int> table;
        int a = 1, b = 2;
        table.insert("k", &a);
        int* old = table.insert("k", &b);
        ctx.equal(old, &a);
        ctx.equal(table.find("k"), &b);
        ctx.equal(table.count(), 1u);
    }},
    {"insert with nullptr data removes the entry", [](auto& ctx) {
        hash_table<int> table;
        int a = 1;
        table.insert("k", &a);
        int* old = table.insert("k", nullptr);
        ctx.equal(old, &a);
        ctx.check(table.find("k") == nullptr);
        ctx.equal(table.count(), 0u);
    }},
    {"lookup is ASCII case-insensitive, matching legacy strHash masking", [](auto& ctx) {
        hash_table<int> table;
        int value = 7;
        table.insert("MixedCase", &value);
        ctx.equal(table.find("mixedcase"), &value);
        ctx.equal(table.find("MIXEDCASE"), &value);
    }},
    {"grows past the linear-search threshold and keeps all entries findable", [](auto& ctx) {
        hash_table<int> table;
        std::vector<std::string> keys;
        std::vector<int> values;
        keys.reserve(50);
        values.reserve(50);
        for (int i = 0; i < 50; ++i) {
            keys.push_back("key" + std::to_string(i));
            values.push_back(i);
        }
        for (int i = 0; i < 50; ++i) table.insert(keys[static_cast<std::size_t>(i)], &values[static_cast<std::size_t>(i)]);
        ctx.equal(table.count(), 50u);
        for (int i = 0; i < 50; ++i) {
            ctx.equal(table.find(keys[static_cast<std::size_t>(i)]), &values[static_cast<std::size_t>(i)]);
        }
    }},
    {"iteration visits every inserted element exactly once", [](auto& ctx) {
        hash_table<int> table;
        int a = 1, b = 2, c = 3;
        table.insert("a", &a);
        table.insert("b", &b);
        table.insert("c", &c);
        std::size_t seen = 0;
        for ([[maybe_unused]] const auto& elem : table) ++seen;
        ctx.equal(seen, std::size_t{3});
    }},
});

} // namespace sqlite::utils::hash::testing
