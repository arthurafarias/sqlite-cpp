#pragma once

#include <charconv>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// Legacy source: vdbemem.c, the `Mem`/`sqlite3_value` struct in vdbeInt.h
// (SRS S3.4.1, sqlite::core::virtual_machine::aux). Every SQL value the VDBE
// manipulates -- register contents, bound parameters, column results -- is a
// `mem`.
//
// Deliberate modernization vs. the legacy struct: `sqlite3_value` caches
// *multiple simultaneous representations* of one logical value (e.g. both
// MEM_Int and MEM_Str set at once, with Mem.z/Mem.u.i both valid) as a
// performance optimization, and tracks string/blob ownership through a
// four-way MEM_Static/MEM_Dyn/MEM_Ephem/MEM_Term flag scheme with a manual
// destructor pointer. This port instead gives each `mem` exactly one
// canonical representation (a std::variant) and lets C++ value semantics
// (std::string/std::vector's own RAII) own storage -- there is no MEM_Dyn
// vs. MEM_Static distinction to get wrong. This changes an internal
// performance/ownership detail, not the observable type/value contract
// (SQL NULL/INTEGER/REAL/TEXT/BLOB, and SQLite's type-affinity coercion and
// ordering rules), which is what FR-7 API compatibility ultimately depends
// on and what this header's tests check.
namespace sqlite::core::virtual_machine::aux {

enum class value_type {
    null,
    integer,
    real,
    text,
    blob,
};

class mem {
public:
    mem() = default;

    static mem make_null() { return mem(); }
    static mem make_integer(std::int64_t v) { mem m; m.value_ = v; return m; }
    static mem make_real(double v) { mem m; m.value_ = v; return m; }
    static mem make_text(std::string v) { mem m; m.value_ = std::move(v); return m; }
    static mem make_blob(std::vector<std::byte> v) { mem m; m.value_ = std::move(v); return m; }

    [[nodiscard]] value_type type() const noexcept {
        switch (value_.index()) {
            case 0: return value_type::null;
            case 1: return value_type::integer;
            case 2: return value_type::real;
            case 3: return value_type::text;
            default: return value_type::blob;
        }
    }
    [[nodiscard]] bool is_null() const noexcept { return value_.index() == 0; }

    // numericType()/applyNumericAffinity(): a value is "numeric" only if
    // the *entire* text fully parses as an integer or real -- "12abc" is
    // not numeric for type-affinity purposes, even though as_integer()
    // below will still extract a leading 12 from it on request.
    [[nodiscard]] bool is_numeric() const noexcept {
        return type() == value_type::integer || type() == value_type::real ||
               (type() == value_type::text && full_text_numeric_type().has_value());
    }

    // sqlite3VdbeIntValue(): coerces to an integer. A real value truncates
    // toward zero; text takes a best-effort leading numeric prefix (matching
    // sqlite3AtoF's lenient "parse what you can" behavior for CAST-like
    // contexts), defaulting to 0 if nothing parses; blob and null are 0.
    [[nodiscard]] std::int64_t as_integer() const {
        if (auto* i = std::get_if<std::int64_t>(&value_)) return *i;
        if (auto* r = std::get_if<double>(&value_)) return static_cast<std::int64_t>(*r);
        if (auto* s = std::get_if<std::string>(&value_)) return parse_leading_integer(*s);
        return 0;
    }

    // sqlite3VdbeRealValue(): coerces to a real.
    [[nodiscard]] double as_real() const {
        if (auto* r = std::get_if<double>(&value_)) return *r;
        if (auto* i = std::get_if<std::int64_t>(&value_)) return static_cast<double>(*i);
        if (auto* s = std::get_if<std::string>(&value_)) return parse_leading_real(*s);
        return 0.0;
    }

    // sqlite3VdbeMemStringify(): stringifies for display/concatenation.
    // Integers print as plain decimal; reals print with up to 15
    // significant digits (SQLite's default %.15g-equivalent precision); a
    // blob's bytes are copied through as-is (matching legacy's raw
    // byte-for-byte blob-to-text reinterpretation, not a hex/base64
    // encoding); null stringifies to the empty string (never called for a
    // genuine SQL NULL in practice -- callers check is_null() first -- but
    // defined rather than undefined for a value that arrives null anyway).
    [[nodiscard]] std::string as_text() const {
        if (auto* s = std::get_if<std::string>(&value_)) return *s;
        if (auto* i = std::get_if<std::int64_t>(&value_)) return std::to_string(*i);
        if (auto* r = std::get_if<double>(&value_)) return format_real(*r);
        if (auto* b = std::get_if<std::vector<std::byte>>(&value_)) {
            return std::string(reinterpret_cast<const char*>(b->data()), b->size());
        }
        return {};
    }

    [[nodiscard]] const std::vector<std::byte>* as_blob_if() const noexcept {
        return std::get_if<std::vector<std::byte>>(&value_);
    }

    bool operator==(const mem& other) const;

private:
    std::variant<std::monostate, std::int64_t, double, std::string, std::vector<std::byte>> value_;

    [[nodiscard]] std::optional<std::variant<std::int64_t, double>> full_text_numeric_type() const {
        auto* s = std::get_if<std::string>(&value_);
        if (s == nullptr) return std::nullopt;
        std::string_view sv = *s;
        // Trim ASCII whitespace, matching SQLite's leniency on surrounding
        // spaces in numeric string literals.
        std::size_t b = sv.find_first_not_of(" \t\n\r\f\v");
        if (b == std::string_view::npos) return std::nullopt;
        std::size_t e = sv.find_last_not_of(" \t\n\r\f\v");
        sv = sv.substr(b, e - b + 1);

        std::int64_t as_int = 0;
        auto int_result = std::from_chars(sv.data(), sv.data() + sv.size(), as_int);
        if (int_result.ec == std::errc{} && int_result.ptr == sv.data() + sv.size()) {
            return std::variant<std::int64_t, double>(std::in_place_index<0>, as_int);
        }
        double as_double = 0;
        auto real_result = std::from_chars(sv.data(), sv.data() + sv.size(), as_double);
        if (real_result.ec == std::errc{} && real_result.ptr == sv.data() + sv.size()) {
            return std::variant<std::int64_t, double>(std::in_place_index<1>, as_double);
        }
        return std::nullopt;
    }

    static std::int64_t parse_leading_integer(std::string_view sv) {
        std::size_t b = sv.find_first_not_of(" \t\n\r\f\v");
        if (b == std::string_view::npos) return 0;
        sv = sv.substr(b);
        std::int64_t out = 0;
        std::from_chars(sv.data(), sv.data() + sv.size(), out);
        return out;
    }

    static double parse_leading_real(std::string_view sv) {
        std::size_t b = sv.find_first_not_of(" \t\n\r\f\v");
        if (b == std::string_view::npos) return 0.0;
        sv = sv.substr(b);
        double out = 0.0;
        std::from_chars(sv.data(), sv.data() + sv.size(), out);
        return out;
    }

    static std::string format_real(double v) {
        char buf[32];
        auto result = std::to_chars(buf, buf + sizeof(buf), v, std::chars_format::general, 15);
        return std::string(buf, result.ptr);
    }
};

// sqlite3MemCompare(): SQLite's total value ordering -- NULL sorts before
// every NUMERIC value (INTEGER and REAL compared numerically against each
// other, regardless of which representation each side happens to hold),
// which sorts before TEXT, which sorts before BLOB. TEXT and BLOB compare
// byte-wise (memcmp-equivalent), the same as the legacy code's BINARY
// collation -- this port does not yet have a collation subsystem
// (sqlite-core-command-processor, not yet built), so BINARY is the only
// comparison this phase supports.
inline int compare(const mem& a, const mem& b) {
    auto rank = [](const mem& m) {
        switch (m.type()) {
            case value_type::null: return 0;
            case value_type::integer: return 1;
            case value_type::real: return 1;
            case value_type::text: return 2;
            default: return 3;
        }
    };
    int ra = rank(a);
    int rb = rank(b);
    if (ra != rb) return ra < rb ? -1 : 1;

    switch (ra) {
        case 0: return 0; // both null: SQLite's raw comparator treats NULL==NULL (IS-semantics live above this layer)
        case 1: {
            double da = a.as_real();
            double db = b.as_real();
            if (a.type() == value_type::integer && b.type() == value_type::integer) {
                std::int64_t ia = a.as_integer();
                std::int64_t ib = b.as_integer();
                if (ia != ib) return ia < ib ? -1 : 1;
                return 0;
            }
            if (da != db) return da < db ? -1 : 1;
            return 0;
        }
        case 2: {
            std::string ta = a.as_text();
            std::string tb = b.as_text();
            if (ta != tb) return ta < tb ? -1 : 1;
            return 0;
        }
        default: {
            const auto* ba = a.as_blob_if();
            const auto* bb = b.as_blob_if();
            if (*ba != *bb) return *ba < *bb ? -1 : 1;
            return 0;
        }
    }
}

inline bool mem::operator==(const mem& other) const { return compare(*this, other) == 0 && type() == other.type(); }

} // namespace sqlite::core::virtual_machine::aux
