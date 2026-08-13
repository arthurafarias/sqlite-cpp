#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <stdexcept>

// Legacy source: pager.c -- the legacy rollback-journal on-disk format
// (SRS S3.3.2, "PENDING_BYTE"/journal header comment block, pager_cksum(),
// pagerAddPageToRollbackJournal()). Ported bit-for-bit: the 8-byte magic,
// the journal header's five fixed fields in their documented order, the
// checksum algorithm (which deliberately samples only every 200th byte of
// a page, not the whole page -- see page_checksum() below), and the
// per-page record layout (4-byte pgno, page_size bytes of pre-image
// content, 4-byte checksum).
//
// Not ported: WAL (a distinct, newer durability mechanism -- out of scope
// per this library's agreed scope, "page cache + real rollback journal").
namespace sqlite::backend::pager {

// pager.c's aJournalMagic[], byte for byte.
inline constexpr std::array<unsigned char, 8> journal_magic = {
    0xd9, 0xd5, 0x05, 0xf9, 0x20, 0xa1, 0x63, 0xd7,
};

// Legacy's JOURNAL_HDR_SZ rounds up to the underlying disk's sector size,
// so a header write lands in a single atomic sector (pager.c's own
// comment: "put the header in the additional space at the beginning").
// sqlite::backend::os doesn't expose real device sector sizes yet (its
// unix_file::sector_size() is a fixed 4096 stand-in -- see that library's
// docs), so this port always uses one fixed, generously-sized header
// record instead of sector-rounding. This changes only how much padding
// follows the real header fields below, not their layout or meaning.
inline constexpr int journal_header_size = 512;
static_assert(journal_header_size >= 8 + 4 + 4 + 4 + 4 + 4);

// Byte size of one page record: 4-byte pgno + page content + 4-byte
// checksum (pagerAddPageToRollbackJournal()).
inline constexpr int page_record_overhead = 8;

inline std::uint32_t jf_read_u32(const unsigned char* p) noexcept {
    return (static_cast<std::uint32_t>(p[0]) << 24) | (static_cast<std::uint32_t>(p[1]) << 16) |
           (static_cast<std::uint32_t>(p[2]) << 8) | p[3];
}

inline void jf_write_u32(unsigned char* p, std::uint32_t v) noexcept {
    p[0] = static_cast<unsigned char>(v >> 24);
    p[1] = static_cast<unsigned char>(v >> 16);
    p[2] = static_cast<unsigned char>(v >> 8);
    p[3] = static_cast<unsigned char>(v);
}

// pager.c's pager_cksum(): a deliberately cheap checksum, not a
// cryptographic or full-coverage one -- it samples only every 200th byte
// of the page, starting from (page_size - 200) and walking backwards,
// seeded with a per-journal random value (cksum_init) so a stale leftover
// page from a previous journal can't be replayed and mistaken for a valid
// record of the current one. Ported as-is: this port's job is behavioral
// fidelity to legacy, not a security upgrade legacy itself doesn't make.
inline std::uint32_t page_checksum(std::uint32_t cksum_init, const unsigned char* page_data, int page_size) noexcept {
    std::uint32_t cksum = cksum_init;
    int i = page_size - 200;
    while (i > 0) {
        cksum += page_data[i];
        i -= 200;
    }
    return cksum;
}

// The journal header's five fixed fields, in pager.c's documented order:
// "8 bytes: Magic. 4 bytes: Number of records in journal. 4 bytes: Random
// number used for page hash. 4 bytes: Initial database page count. 4
// bytes: Sector size used by the process that wrote this journal. 4
// bytes: Database page size." Legacy's nRec == -1 ("no-sync mode is on,
// keep reading records until EOF") sentinel is not modeled -- this port
// always writes a real, final record count at commit/rollback time since
// it does not implement legacy's no-sync journal mode.
struct journal_header {
    std::uint32_t nrec = 0;
    std::uint32_t cksum_init = 0;
    std::uint32_t orig_page_count = 0;
    std::uint32_t sector_size = 512;
    std::uint32_t page_size = 4096;

    [[nodiscard]] static bool has_valid_magic(const unsigned char* buf) noexcept {
        return std::memcmp(buf, journal_magic.data(), journal_magic.size()) == 0;
    }

    static journal_header parse(const unsigned char* buf) {
        if (!has_valid_magic(buf)) throw std::runtime_error("journal_header: bad magic");
        journal_header h;
        h.nrec = jf_read_u32(buf + 8);
        h.cksum_init = jf_read_u32(buf + 12);
        h.orig_page_count = jf_read_u32(buf + 16);
        h.sector_size = jf_read_u32(buf + 20);
        h.page_size = jf_read_u32(buf + 24);
        return h;
    }

    void write(unsigned char* buf) const {
        std::memset(buf, 0, journal_header_size);
        std::memcpy(buf, journal_magic.data(), journal_magic.size());
        jf_write_u32(buf + 8, nrec);
        jf_write_u32(buf + 12, cksum_init);
        jf_write_u32(buf + 16, orig_page_count);
        jf_write_u32(buf + 20, sector_size);
        jf_write_u32(buf + 24, page_size);
    }
};

} // namespace sqlite::backend::pager
