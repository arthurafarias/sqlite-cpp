#pragma once

#include "../journal_format.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <vector>

namespace sqlite::backend::pager::testing {

using sqlite::utils::testing::test_group;

inline const test_group journal_format_tests("journal_format", {
    {"journal_header round-trips through write/parse", [](auto& ctx) {
        journal_header h;
        h.nrec = 7;
        h.cksum_init = 0xdeadbeef;
        h.orig_page_count = 42;
        h.sector_size = 512;
        h.page_size = 4096;

        unsigned char buf[journal_header_size];
        h.write(buf);
        journal_header parsed = journal_header::parse(buf);

        ctx.equal(parsed.nrec, h.nrec);
        ctx.equal(parsed.cksum_init, h.cksum_init);
        ctx.equal(parsed.orig_page_count, h.orig_page_count);
        ctx.equal(parsed.sector_size, h.sector_size);
        ctx.equal(parsed.page_size, h.page_size);
    }},
    {"journal_header::parse rejects a buffer with the wrong magic", [](auto& ctx) {
        unsigned char buf[journal_header_size] = {0};
        ctx.check(!journal_header::has_valid_magic(buf));
        ctx.template throws<std::runtime_error>([&]() { journal_header::parse(buf); });
    }},
    {"page_checksum is deterministic and sensitive to content changes", [](auto& ctx) {
        std::vector<unsigned char> page(4096, 0x11);
        std::uint32_t cksum_a = page_checksum(12345, page.data(), static_cast<int>(page.size()));
        std::uint32_t cksum_b = page_checksum(12345, page.data(), static_cast<int>(page.size()));
        ctx.equal(cksum_a, cksum_b);

        page[page.size() - 200] ^= 0xff;  // page_checksum's first sampled byte (page_size - 200)
        std::uint32_t cksum_c = page_checksum(12345, page.data(), static_cast<int>(page.size()));
        ctx.check(cksum_a != cksum_c);
    }},
    {"page_checksum depends on cksum_init (the per-journal random seed)", [](auto& ctx) {
        std::vector<unsigned char> page(4096, 0x22);
        std::uint32_t cksum_a = page_checksum(1, page.data(), static_cast<int>(page.size()));
        std::uint32_t cksum_b = page_checksum(2, page.data(), static_cast<int>(page.size()));
        ctx.check(cksum_a != cksum_b);
    }},
});

} // namespace sqlite::backend::pager::testing
