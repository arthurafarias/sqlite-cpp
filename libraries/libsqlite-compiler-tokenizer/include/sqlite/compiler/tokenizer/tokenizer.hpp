#pragma once

#include "keyword_table.hpp"
#include "token.hpp"
#include "token_type.hpp"

#include <cctype>
#include <cstddef>
#include <string_view>
#include <vector>

// Legacy source: tokenize.c's sqlite3GetToken() (SRS S3.5.1,
// sqlite::compiler::tokenizer). The character-by-character SQL lexer: given
// remaining input, decide the type and length of the single token at its
// start. Ports the same per-character-class algorithm (see the case-by-case
// comments below, each naming the CC_* class it replaces) but dispatches by
// directly inspecting characters/ranges rather than porting tokenize.c's
// 256-entry `aiClass[]` lookup table verbatim -- same observable behavior
// (confirmed by testing/tokenizer_test.hpp against real SQL text), a more
// readable control-flow shape. The EBCDIC code path (`aiClass`'s
// `#ifdef SQLITE_EBCDIC` half) is not ported: EBCDIC is a legacy mainframe
// encoding no supported SQLite target platform (SRS S2.4's GCC/CMake
// toolchain) uses.
namespace sqlite::compiler::tokenizer {

namespace detail {

inline bool is_space(unsigned char c) noexcept {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}
inline bool is_digit(unsigned char c) noexcept { return c >= '0' && c <= '9'; }
inline bool is_xdigit(unsigned char c) noexcept {
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
// IdChar(): alphanumeric, underscore, '$' (SQLite's documented, deliberate
// non-standard allowance -- ticket #1066, tokenize.c's comment above the
// legacy IdChar macro), or any byte with the high bit set (a UTF-8
// continuation or multi-byte lead byte -- SQLite lets identifiers contain
// non-ASCII characters without validating their UTF-8 shape here).
inline bool is_id_char(unsigned char c) noexcept {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || is_digit(c) ||
           c == '_' || c == '$' || c >= 0x80;
}
inline bool is_id_start(unsigned char c) noexcept {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c >= 0x80;
}

constexpr char digit_separator = '_';

} // namespace detail

// sqlite3GetToken(): the type and length of the single token starting at
// `remaining`. Returns {end_of_input, {}} for an empty input, matching how
// callers (next_token loops / tokenize() below) know to stop.
inline token next_token(std::string_view remaining) {
    if (remaining.empty()) return {token_type::end_of_input, {}};
    unsigned char c0 = static_cast<unsigned char>(remaining[0]);
    auto at = [&](std::size_t i) -> unsigned char {
        return i < remaining.size() ? static_cast<unsigned char>(remaining[i]) : 0;
    };
    auto make = [&](token_type t, std::size_t len) {
        return token{t, remaining.substr(0, len)};
    };

    // CC_SPACE
    if (detail::is_space(c0)) {
        std::size_t i = 1;
        while (detail::is_space(at(i))) ++i;
        return make(token_type::space, i);
    }

    // CC_MINUS: '--' line comment, '->'/'->>' JSON path operator, or plain '-'.
    if (c0 == '-') {
        if (at(1) == '-') {
            std::size_t i = 2;
            while (at(i) != 0 && at(i) != '\n') ++i;
            return make(token_type::comment, i);
        }
        if (at(1) == '>') return make(token_type::ptr, at(2) == '>' ? 3 : 2);
        return make(token_type::minus, 1);
    }

    switch (c0) {
        case '(': return make(token_type::lp, 1);
        case ')': return make(token_type::rp, 1);
        case ';': return make(token_type::semi, 1);
        case '+': return make(token_type::plus, 1);
        case '*': return make(token_type::star, 1);
        case '%': return make(token_type::rem, 1);
        case ',': return make(token_type::comma, 1);
        case '&': return make(token_type::bitand_, 1);
        case '~': return make(token_type::bitnot, 1);
        default: break;
    }

    // CC_SLASH: '/*' block comment (unterminated comments still consume to
    // EOF and are still reported as TK_COMMENT, matching legacy) or '/'.
    if (c0 == '/') {
        if (at(1) != '*' || at(2) == 0) return make(token_type::slash, 1);
        std::size_t i = 3;
        unsigned char c = at(2);
        while ((c != '*' || at(i) != '/') && (c = at(i)) != 0) ++i;
        if (c != 0) ++i;
        return make(token_type::comment, i);
    }

    // CC_EQ / CC_LT / CC_GT / CC_BANG
    if (c0 == '=') return make(token_type::eq, at(1) == '=' ? 2 : 1);
    if (c0 == '<') {
        if (at(1) == '=') return make(token_type::le, 2);
        if (at(1) == '>') return make(token_type::ne, 2);
        if (at(1) == '<') return make(token_type::lshift, 2);
        return make(token_type::lt, 1);
    }
    if (c0 == '>') {
        if (at(1) == '=') return make(token_type::ge, 2);
        if (at(1) == '>') return make(token_type::rshift, 2);
        return make(token_type::gt, 1);
    }
    if (c0 == '!') return at(1) == '=' ? make(token_type::ne, 2) : make(token_type::illegal, 1);

    // CC_PIPE
    if (c0 == '|') return at(1) == '|' ? make(token_type::concat, 2) : make(token_type::bitor_, 1);

    // CC_QUOTE: '...' (string), "..."/`...` (quoted identifier). A doubled
    // delimiter inside the quotes is an escaped literal delimiter character
    // (SQL's '' -> a literal ' inside a '...'-quoted string), not the end of
    // the token.
    if (c0 == '\'' || c0 == '"' || c0 == '`') {
        unsigned char delim = c0;
        std::size_t i = 1;
        unsigned char c = 0;
        while ((c = at(i)) != 0) {
            if (c == delim) {
                if (at(i + 1) == delim) { i += 2; continue; }
                break;
            }
            ++i;
        }
        if (c == '\'') return make(token_type::string, i + 1);
        if (c != 0) return make(token_type::id, i + 1); // closed with " or `
        return make(token_type::illegal, i); // ran off the end, unterminated
    }

    // CC_QUOTE2: [...]-style quoted identifier (no escaping of ']' inside).
    if (c0 == '[') {
        std::size_t i = 1;
        while (at(i) != 0 && at(i) != ']') ++i;
        if (at(i) == ']') return make(token_type::id, i + 1);
        return make(token_type::illegal, i);
    }

    // CC_DOT: '.' alone, or the start of a floating-point literal like ".5"
    // (falls through into the digit-scanning logic below).
    bool dot_leads_number = c0 == '.' && detail::is_digit(at(1));
    if (c0 == '.' && !dot_leads_number) return make(token_type::dot, 1);

    // CC_DIGIT (and the ".5" case just above): integer, hex integer, or
    // floating-point literal, with SQLite's digit-separator ('_' between
    // digits, e.g. 1_000_000) producing TK_QNUMBER instead of
    // TK_INTEGER/TK_FLOAT when used. A digit run immediately followed by an
    // identifier character (e.g. "123abc") is reported as TK_ILLEGAL, same
    // as legacy.
    if (detail::is_digit(c0) || dot_leads_number) {
        token_type type = token_type::integer;
        std::size_t i;
        if (c0 == '0' && (at(1) == 'x' || at(1) == 'X') && detail::is_xdigit(at(2))) {
            i = 3;
            while (true) {
                if (!detail::is_xdigit(at(i))) {
                    if (at(i) == detail::digit_separator) { type = token_type::qnumber; }
                    else break;
                }
                ++i;
            }
        } else {
            i = 0;
            while (true) {
                if (!detail::is_digit(at(i))) {
                    if (at(i) == detail::digit_separator) { type = token_type::qnumber; }
                    else break;
                }
                ++i;
            }
            if (at(i) == '.') {
                if (type == token_type::integer) type = token_type::float_;
                for (++i;; ++i) {
                    if (!detail::is_digit(at(i))) {
                        if (at(i) == detail::digit_separator) { type = token_type::qnumber; }
                        else break;
                    }
                }
            }
            if ((at(i) == 'e' || at(i) == 'E') &&
                (detail::is_digit(at(i + 1)) ||
                 ((at(i + 1) == '+' || at(i + 1) == '-') && detail::is_digit(at(i + 2))))) {
                if (type == token_type::integer) type = token_type::float_;
                for (i += 2;; ++i) {
                    if (!detail::is_digit(at(i))) {
                        if (at(i) == detail::digit_separator) { type = token_type::qnumber; }
                        else break;
                    }
                }
            }
        }
        while (detail::is_id_char(at(i))) { type = token_type::illegal; ++i; }
        return make(type, i);
    }

    // CC_VARNUM: '?' or '?123'.
    if (c0 == '?') {
        std::size_t i = 1;
        while (detail::is_digit(at(i))) ++i;
        return make(token_type::variable, i);
    }

    // CC_DOLLAR / CC_VARALPHA: '$name', '@name', ':name', '#name', plus the
    // legacy TCL-variable extensions ('$name(...)' and '::' continuation).
    if (c0 == '$' || c0 == '@' || c0 == ':' || c0 == '#') {
        std::size_t i = 1;
        int n = 0;
        token_type type = token_type::variable;
        unsigned char c;
        while ((c = at(i)) != 0) {
            if (detail::is_id_char(c)) {
                ++n;
                ++i;
            } else if (c == '(' && n > 0) {
                do { ++i; c = at(i); } while (c != 0 && !detail::is_space(c) && c != ')');
                if (c == ')') ++i; else type = token_type::illegal;
                break;
            } else if (c == ':' && at(i + 1) == ':') {
                i += 2;
            } else {
                break;
            }
        }
        if (n == 0) type = token_type::illegal;
        return make(type, i);
    }

    // CC_X: a blob literal (x'...'/X'...') if followed by a quote, else an
    // ordinary identifier (falls through -- no SQL keyword starts with 'x').
    if (c0 == 'x' || c0 == 'X') {
        if (at(1) == '\'') {
            std::size_t i = 2;
            while (detail::is_xdigit(at(i))) ++i;
            token_type type = token_type::blob;
            if (at(i) != '\'' || (i % 2) != 0) {
                type = token_type::illegal;
                while (at(i) != 0 && at(i) != '\'') ++i;
            }
            if (at(i) != 0) ++i;
            return make(type, i);
        }
        // fall through to the identifier/keyword scan below
    }

    // CC_KYWD0/CC_KYWD/CC_ID: an identifier, or one of the 148 SQL keywords
    // if the scanned text matches one exactly (case-insensitively).
    if (detail::is_id_start(c0) || c0 == 'x' || c0 == 'X') {
        std::size_t i = 1;
        while (detail::is_id_char(at(i))) ++i;
        std::string_view text = remaining.substr(0, i);
        if (auto kw = lookup_keyword(text)) return make(*kw, i);
        return make(token_type::id, i);
    }

    return make(token_type::illegal, 1);
}

// Convenience over next_token(): the full token stream for `sql`. If
// `skip_trivia` is true (the default), TK_SPACE/TK_COMMENT tokens are
// dropped, matching how sqlite3RunParser()'s main loop
// (`while( (tokenType = getToken(...))!=0 ){ if(tokenType>=TK_SPACE) continue;`
// -- tokenize.c) filters them out before handing tokens to the parser.
inline std::vector<token> tokenize(std::string_view sql, bool skip_trivia = true) {
    std::vector<token> tokens;
    std::string_view remaining = sql;
    while (true) {
        token t = next_token(remaining);
        if (t.type == token_type::end_of_input) break;
        if (!skip_trivia || (t.type != token_type::space && t.type != token_type::comment)) {
            tokens.push_back(t);
        }
        remaining.remove_prefix(t.text.size());
    }
    return tokens;
}

} // namespace sqlite::compiler::tokenizer
