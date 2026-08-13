#pragma once

#include "token_type.hpp"

#include <array>
#include <cctype>
#include <optional>
#include <string_view>

// Legacy source: applications/mkkeywordhash-legacy/mkkeywordhash.c's generated output (keywordhash.h,
// sqlite3KeywordCode() -- an ASCII-case-insensitive perfect hash over the
// 148 SQL keywords; SRS S3.5.1). This port doesn't hand-replicate the
// generated perfect-hash function itself (that's tooling output, not
// meaningful to transcribe by hand), but does replicate the same
// observable contract: given an identifier-shaped token's text, decide
// whether it is exactly one of the 148 reserved keywords (case-insensitively)
// and, if so, which one -- otherwise it's a plain identifier (TK_ID).
// A linear scan over a 148-entry array is asymptotically slower than a
// perfect hash, but the difference is not performance-relevant at this
// size, and it keeps this header simple, obviously correct, and easy to
// keep in sync with token_type.hpp by inspection.
namespace sqlite::compiler::tokenizer {

namespace detail {

struct keyword_entry {
    std::string_view text;   // always upper-case; lookup compares case-insensitively
    token_type type;
};

inline constexpr std::array<keyword_entry, 148> keyword_table{{
    {"ABORT", token_type::abort},
    {"ACTION", token_type::action},
    {"ADD", token_type::add},
    {"AFTER", token_type::after},
    {"ALL", token_type::all},
    {"ALTER", token_type::alter},
    {"ALWAYS", token_type::always},
    {"ANALYZE", token_type::analyze},
    {"AND", token_type::and_},
    {"AS", token_type::as},
    {"ASC", token_type::asc},
    {"ATTACH", token_type::attach},
    {"AUTOINCREMENT", token_type::autoincrement},
    {"BEFORE", token_type::before},
    {"BEGIN", token_type::begin},
    {"BETWEEN", token_type::between},
    {"BY", token_type::by},
    {"CASCADE", token_type::cascade},
    {"CASE", token_type::case_},
    {"CAST", token_type::cast},
    {"CHECK", token_type::check},
    {"COLLATE", token_type::collate},
    {"COLUMN", token_type::column},
    {"COMMIT", token_type::commit},
    {"CONFLICT", token_type::conflict},
    {"CONSTRAINT", token_type::constraint},
    {"CREATE", token_type::create},
    {"CROSS", token_type::cross},
    {"CURRENT", token_type::current},
    {"CURRENT_DATE", token_type::current_date},
    {"CURRENT_TIME", token_type::current_time},
    {"CURRENT_TIMESTAMP", token_type::current_timestamp},
    {"DATABASE", token_type::database},
    {"DEFAULT", token_type::default_},
    {"DEFERRED", token_type::deferred},
    {"DEFERRABLE", token_type::deferrable},
    {"DELETE", token_type::delete_},
    {"DESC", token_type::desc},
    {"DETACH", token_type::detach},
    {"DISTINCT", token_type::distinct},
    {"DO", token_type::do_},
    {"DROP", token_type::drop},
    {"END", token_type::end},
    {"EACH", token_type::each},
    {"ELSE", token_type::else_},
    {"ESCAPE", token_type::escape},
    {"EXCEPT", token_type::except},
    {"EXCLUSIVE", token_type::exclusive},
    {"EXCLUDE", token_type::exclude},
    {"EXISTS", token_type::exists},
    {"EXPLAIN", token_type::explain},
    {"FAIL", token_type::fail},
    {"FILTER", token_type::filter},
    {"FIRST", token_type::first},
    {"FOLLOWING", token_type::following},
    {"FOR", token_type::for_},
    {"FOREIGN", token_type::foreign},
    {"FROM", token_type::from},
    {"FULL", token_type::full},
    {"GENERATED", token_type::generated},
    {"GLOB", token_type::glob},
    {"GROUP", token_type::group},
    {"GROUPS", token_type::groups},
    {"HAVING", token_type::having},
    {"IF", token_type::if_},
    {"IGNORE", token_type::ignore},
    {"IMMEDIATE", token_type::immediate},
    {"IN", token_type::in},
    {"INDEX", token_type::index},
    {"INDEXED", token_type::indexed},
    {"INITIALLY", token_type::initially},
    {"INNER", token_type::inner},
    {"INSERT", token_type::insert},
    {"INSTEAD", token_type::instead},
    {"INTERSECT", token_type::intersect},
    {"INTO", token_type::into},
    {"IS", token_type::is},
    {"ISNULL", token_type::isnull},
    {"JOIN", token_type::join},
    {"KEY", token_type::key},
    {"LAST", token_type::last},
    {"LEFT", token_type::left},
    {"LIKE", token_type::like},
    {"LIMIT", token_type::limit},
    {"MATCH", token_type::match},
    {"MATERIALIZED", token_type::materialized},
    {"NATURAL", token_type::natural},
    {"NO", token_type::no},
    {"NOT", token_type::not_},
    {"NOTHING", token_type::nothing},
    {"NOTNULL", token_type::notnull},
    {"NULL", token_type::null},
    {"NULLS", token_type::nulls},
    {"OF", token_type::of},
    {"OFFSET", token_type::offset},
    {"ON", token_type::on},
    {"OR", token_type::or_},
    {"ORDER", token_type::order},
    {"OTHERS", token_type::others},
    {"OUTER", token_type::outer},
    {"OVER", token_type::over},
    {"PARTITION", token_type::partition},
    {"PLAN", token_type::plan},
    {"PRAGMA", token_type::pragma},
    {"PRECEDING", token_type::preceding},
    {"PRIMARY", token_type::primary},
    {"QUERY", token_type::query},
    {"RAISE", token_type::raise},
    {"RANGE", token_type::range},
    {"RECURSIVE", token_type::recursive},
    {"REFERENCES", token_type::references},
    {"REGEXP", token_type::regexp},
    {"REINDEX", token_type::reindex},
    {"RELEASE", token_type::release},
    {"RENAME", token_type::rename},
    {"REPLACE", token_type::replace},
    {"RESTRICT", token_type::restrict},
    {"RETURNING", token_type::returning},
    {"RIGHT", token_type::right},
    {"ROLLBACK", token_type::rollback},
    {"ROW", token_type::row},
    {"ROWS", token_type::rows},
    {"SAVEPOINT", token_type::savepoint},
    {"SELECT", token_type::select},
    {"SET", token_type::set},
    {"TABLE", token_type::table},
    {"TEMP", token_type::temp},
    {"TEMPORARY", token_type::temporary},
    {"THEN", token_type::then},
    {"TIES", token_type::ties},
    {"TO", token_type::to},
    {"TRANSACTION", token_type::transaction},
    {"TRIGGER", token_type::trigger},
    {"UNBOUNDED", token_type::unbounded},
    {"UNION", token_type::union_},
    {"UNIQUE", token_type::unique},
    {"UPDATE", token_type::update},
    {"USING", token_type::using_},
    {"VACUUM", token_type::vacuum},
    {"VALUES", token_type::values},
    {"VIEW", token_type::view},
    {"VIRTUAL", token_type::virtual_},
    {"WHEN", token_type::when},
    {"WHERE", token_type::where},
    {"WINDOW", token_type::window},
    {"WITH", token_type::with},
    {"WITHIN", token_type::within},
    {"WITHOUT", token_type::without},
}};

inline bool iequals(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::toupper(static_cast<unsigned char>(a[i])) != std::toupper(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

} // namespace detail

// sqlite3KeywordCode(): returns the keyword token type for `text` if it
// exactly (case-insensitively) matches one of the 148 SQL keywords,
// otherwise nullopt (the caller then knows to treat the token as TK_ID).
inline std::optional<token_type> lookup_keyword(std::string_view text) noexcept {
    for (const auto& entry : detail::keyword_table) {
        if (detail::iequals(entry.text, text)) return entry.type;
    }
    return std::nullopt;
}

} // namespace sqlite::compiler::tokenizer
