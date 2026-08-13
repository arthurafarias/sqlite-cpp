#pragma once

#include "../btree.hpp"
#include "../cursor.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>
#include <unistd.h>
#include <vector>

namespace sqlite::backend::tree::testing {

using sqlite::utils::testing::test_group;

namespace {
std::string make_temp_path(const char* suffix) {
    std::string tmpl = "/tmp/sqlite_cpp_backend_tree_test_XXXXXX";
    std::vector<char> buf(tmpl.begin(), tmpl.end());
    buf.push_back('\0');
    int fd = ::mkstemp(buf.data());
    if (fd >= 0) ::close(fd);
    std::string path(buf.data());
    ::unlink(path.c_str());
    return path + suffix;
}

std::vector<unsigned char> bytes_from(std::string_view s) {
    return std::vector<unsigned char>(s.begin(), s.end());
}
std::string string_from(const std::vector<unsigned char>& b) {
    return std::string(b.begin(), b.end());
}
} // namespace

inline const test_group btree_tests("btree", {
    {"insert then find round-trips a small payload on a single page", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        page_store store(path, 4096);
        btree tree(store);

        auto payload = bytes_from("hello, b-tree");
        tree.insert(1, payload.data(), payload.size());

        auto found = tree.find(1);
        ctx.check(found.has_value());
        ctx.equal(string_from(*found), std::string{"hello, b-tree"});
        ctx.check(!tree.find(2).has_value());

        ::unlink(path.c_str());
    }},
    {"insert with an existing rowid replaces (upsert)", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        page_store store(path, 4096);
        btree tree(store);

        auto a = bytes_from("version1");
        auto b = bytes_from("version2-longer");
        tree.insert(5, a.data(), a.size());
        tree.insert(5, b.data(), b.size());

        auto found = tree.find(5);
        ctx.check(found.has_value());
        ctx.equal(string_from(*found), std::string{"version2-longer"});

        ::unlink(path.c_str());
    }},
    {"erase removes a row and leaves others intact", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        page_store store(path, 4096);
        btree tree(store);

        for (std::int64_t i = 1; i <= 5; ++i) {
            auto p = bytes_from("row" + std::to_string(i));
            tree.insert(i, p.data(), p.size());
        }
        ctx.check(tree.erase(3));
        ctx.check(!tree.erase(3));  // already gone
        ctx.check(!tree.find(3).has_value());
        ctx.check(tree.find(1).has_value());
        ctx.check(tree.find(5).has_value());

        ::unlink(path.c_str());
    }},
    {"a payload too large for one page splits across overflow pages and reads back exactly", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        page_store store(path, 512);  // small page size to force overflow easily
        btree tree(store);

        std::vector<unsigned char> big(5000);
        for (std::size_t i = 0; i < big.size(); ++i) big[i] = static_cast<unsigned char>(i % 256);
        tree.insert(42, big.data(), big.size());

        auto found = tree.find(42);
        ctx.check(found.has_value());
        ctx.check(*found == big);

        ::unlink(path.c_str());
    }},
    {"many inserts force multi-page splits and the tree grows past one level, but every row is still findable", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        page_store store(path, 512);  // small pages so a few hundred rows force real splitting
        btree tree(store);

        constexpr int n = 500;
        for (std::int64_t i = 0; i < n; ++i) {
            auto p = bytes_from("payload-for-row-" + std::to_string(i));
            tree.insert(i, p.data(), p.size());
        }
        ctx.check(store.page_count() > 3);  // real evidence multiple leaves (and an interior level) exist

        bool all_found = true;
        for (std::int64_t i = 0; i < n; ++i) {
            auto found = tree.find(i);
            if (!found || string_from(*found) != "payload-for-row-" + std::to_string(i)) { all_found = false; break; }
        }
        ctx.check(all_found);

        ::unlink(path.c_str());
    }},
    {"inserting out of order still leaves every row correctly findable after splitting", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        page_store store(path, 512);
        btree tree(store);

        std::vector<std::int64_t> rowids(300);
        std::iota(rowids.begin(), rowids.end(), 0);
        std::mt19937 rng(12345);
        std::shuffle(rowids.begin(), rowids.end(), rng);

        for (std::int64_t rowid : rowids) {
            auto p = bytes_from("v" + std::to_string(rowid));
            tree.insert(rowid, p.data(), p.size());
        }
        bool all_found = true;
        for (std::int64_t rowid : rowids) {
            auto found = tree.find(rowid);
            if (!found || string_from(*found) != "v" + std::to_string(rowid)) { all_found = false; break; }
        }
        ctx.check(all_found);

        ::unlink(path.c_str());
    }},
    {"a cursor iterates every row in ascending rowid order", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        page_store store(path, 512);
        btree tree(store);

        std::vector<std::int64_t> rowids{50, 10, 30, 20, 40, 1, 99};
        for (std::int64_t rowid : rowids) {
            auto p = bytes_from(std::to_string(rowid));
            tree.insert(rowid, p.data(), p.size());
        }

        cursor c(tree);
        std::vector<std::int64_t> seen;
        for (; !c.eof(); c.next()) seen.push_back(c.rowid());

        std::vector<std::int64_t> expected = rowids;
        std::sort(expected.begin(), expected.end());
        ctx.check(seen == expected);

        ::unlink(path.c_str());
    }},
    {"a cursor over many rows spanning multiple pages still iterates in order with no gaps or duplicates", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        page_store store(path, 512);
        btree tree(store);

        constexpr int n = 400;
        std::vector<std::int64_t> rowids(n);
        std::iota(rowids.begin(), rowids.end(), 0);
        std::mt19937 rng(777);
        std::shuffle(rowids.begin(), rowids.end(), rng);
        for (std::int64_t rowid : rowids) {
            auto p = bytes_from("x");
            tree.insert(rowid, p.data(), p.size());
        }

        cursor c(tree);
        std::vector<std::int64_t> seen;
        for (; !c.eof(); c.next()) seen.push_back(c.rowid());
        ctx.equal(seen.size(), static_cast<std::size_t>(n));
        ctx.check(std::is_sorted(seen.begin(), seen.end()));
        ctx.check(std::adjacent_find(seen.begin(), seen.end()) == seen.end());  // no duplicates

        ::unlink(path.c_str());
    }},
    {"data survives closing and reopening the file (real on-disk persistence)", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        {
            page_store store(path, 4096);
            btree tree(store);
            for (std::int64_t i = 0; i < 20; ++i) {
                auto p = bytes_from("persisted-" + std::to_string(i));
                tree.insert(i, p.data(), p.size());
            }
        }  // page_store/btree destroyed, file closed

        {
            page_store store(path, 4096);
            btree tree(store, /*root_page=*/1);
            for (std::int64_t i = 0; i < 20; ++i) {
                auto found = tree.find(i);
                ctx.check(found.has_value());
                ctx.equal(string_from(*found), "persisted-" + std::to_string(i));
            }
        }

        ::unlink(path.c_str());
    }},
    {"reopening an existing file recovers the same page size it was created with", [](auto& ctx) {
        std::string path = make_temp_path(".db");
        { page_store store(path, 1024); }
        page_store reopened(path, 4096 /* ignored: file already exists */);
        ctx.equal(reopened.page_size(), 1024);
        ::unlink(path.c_str());
    }},
});

} // namespace sqlite::backend::tree::testing
