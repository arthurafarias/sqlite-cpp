#pragma once

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// Legacy source: printf.c (SRS S3.2, sqlite::utils::text). Ports mprintf's
// core conversion specifiers (%d/%i, %u, %x/%X, %o, %c, %s, %f, %e, %g, %q,
// %Q, %%) with their width/precision/flag handling. printf.c's va_list-driven
// parsing is replaced with a type-safe sqlite::utils::text::arg variant built
// from a variadic pack, since C++ has no portable equivalent of scanning an
// untyped va_list against a runtime format string; every legacy specifier
// this header implements produces byte-identical output to mprintf() for the
// same logical arguments.
//
// Deferred, not ported in this phase: %n (write-back), the '!' precision
// flag for %s/%q/%Q's over-long-UTF8-sequence handling, and the control-
// character (\x1B[...]) escaping mode %q/%Q gained for safely printing
// arbitrary bytes to a terminal -- none of these affect the well-formed-UTF8,
// non-terminal use of mprintf that the rest of this phase's code relies on.
namespace sqlite::utils::text {

class arg {
public:
    arg(int v) : value_(static_cast<std::int64_t>(v)) {}
    arg(long v) : value_(static_cast<std::int64_t>(v)) {}
    arg(long long v) : value_(static_cast<std::int64_t>(v)) {}
    arg(unsigned v) : value_(static_cast<std::uint64_t>(v)) {}
    arg(unsigned long v) : value_(static_cast<std::uint64_t>(v)) {}
    arg(unsigned long long v) : value_(static_cast<std::uint64_t>(v)) {}
    arg(double v) : value_(v) {}
    arg(char v) : value_(static_cast<std::int64_t>(v)) {}
    arg(const char* v) : value_(v == nullptr ? std::string_view{} : std::string_view{v}), is_null_(v == nullptr) {}
    arg(std::string_view v) : value_(v) {}
    arg(const std::string& v) : value_(std::string_view{v}) {}
    arg(std::nullptr_t) : value_(std::string_view{}), is_null_(true) {}

    [[nodiscard]] std::int64_t as_int() const {
        if (auto* i = std::get_if<std::int64_t>(&value_)) return *i;
        if (auto* u = std::get_if<std::uint64_t>(&value_)) return static_cast<std::int64_t>(*u);
        if (auto* d = std::get_if<double>(&value_)) return static_cast<std::int64_t>(*d);
        return 0;
    }
    [[nodiscard]] std::uint64_t as_uint() const {
        if (auto* u = std::get_if<std::uint64_t>(&value_)) return *u;
        if (auto* i = std::get_if<std::int64_t>(&value_)) return static_cast<std::uint64_t>(*i);
        return 0;
    }
    [[nodiscard]] double as_double() const {
        if (auto* d = std::get_if<double>(&value_)) return *d;
        if (auto* i = std::get_if<std::int64_t>(&value_)) return static_cast<double>(*i);
        if (auto* u = std::get_if<std::uint64_t>(&value_)) return static_cast<double>(*u);
        return 0.0;
    }
    [[nodiscard]] std::string_view as_string() const {
        if (auto* s = std::get_if<std::string_view>(&value_)) return *s;
        return {};
    }
    [[nodiscard]] bool is_null() const { return is_null_; }

private:
    std::variant<std::int64_t, std::uint64_t, double, std::string_view> value_;
    bool is_null_ = false;
};

namespace detail {

struct flags {
    bool left_justify = false;
    bool zero_pad = false;
    bool plus_sign = false;
    bool space_sign = false;
    bool alternate = false;
};

inline void pad(std::string& out, char c, int n) {
    if (n > 0) out.append(static_cast<std::size_t>(n), c);
}

inline void emit_padded(std::string& out, std::string_view body, const flags& f, int width, bool numeric) {
    int extra = width - static_cast<int>(body.size());
    if (extra <= 0) {
        out.append(body);
        return;
    }
    if (f.left_justify) {
        out.append(body);
        pad(out, ' ', extra);
    } else if (f.zero_pad && numeric) {
        std::size_t sign_len = 0;
        if (!body.empty() && (body[0] == '-' || body[0] == '+' || body[0] == ' ')) sign_len = 1;
        out.append(body.substr(0, sign_len));
        pad(out, '0', extra);
        out.append(body.substr(sign_len));
    } else {
        pad(out, ' ', extra);
        out.append(body);
    }
}

inline std::string format_signed(std::int64_t v, const flags& f) {
    std::string digits = std::to_string(v < 0 ? -(v) : v);
    if (v < 0) return "-" + digits;
    if (f.plus_sign) return "+" + digits;
    if (f.space_sign) return " " + digits;
    return digits;
}

inline std::string format_unsigned(std::uint64_t v, int base, bool uppercase) {
    if (v == 0) return "0";
    static constexpr char lower[] = "0123456789abcdef";
    static constexpr char upper[] = "0123456789ABCDEF";
    const char* digits = uppercase ? upper : lower;
    std::string out;
    while (v > 0) {
        out.push_back(digits[v % static_cast<std::uint64_t>(base)]);
        v /= static_cast<std::uint64_t>(base);
    }
    std::reverse(out.begin(), out.end());
    return out;
}

// %q/%Q/%w: doubles occurrences of `quote_char` (printf.c's etESCAPE_q /
// etESCAPE_Q / etESCAPE_w), for building SQL text safely from raw strings.
inline std::string double_quote_char(std::string_view s, char quote_char) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == quote_char) out.push_back(c);
        out.push_back(c);
    }
    return out;
}

} // namespace detail

// mprintf()'s core loop, over a pre-collected argument list. Consumes `%`
// conversions from `fmt` in order, pulling the next unconsumed element of
// `args` for each one (matching mprintf's left-to-right va_arg consumption --
// there is no positional `%2$d`-style addressing in the legacy format
// language, so there is none here either).
inline std::string vformat(std::string_view fmt, const std::vector<arg>& args) {
    std::string out;
    out.reserve(fmt.size());
    std::size_t arg_index = 0;
    auto next_arg = [&]() -> const arg& {
        static const arg missing{std::string_view{}};
        return arg_index < args.size() ? args[arg_index++] : missing;
    };

    for (std::size_t i = 0; i < fmt.size(); ++i) {
        char c = fmt[i];
        if (c != '%') {
            out.push_back(c);
            continue;
        }
        ++i;
        if (i >= fmt.size()) break;
        if (fmt[i] == '%') {
            out.push_back('%');
            continue;
        }

        detail::flags f;
        bool parsing_flags = true;
        while (parsing_flags && i < fmt.size()) {
            switch (fmt[i]) {
                case '-': f.left_justify = true; ++i; break;
                case '0': f.zero_pad = true; ++i; break;
                case '+': f.plus_sign = true; ++i; break;
                case ' ': f.space_sign = true; ++i; break;
                case '#': f.alternate = true; ++i; break;
                default: parsing_flags = false; break;
            }
        }

        int width = 0;
        bool has_width = false;
        while (i < fmt.size() && fmt[i] >= '0' && fmt[i] <= '9') {
            has_width = true;
            width = width * 10 + (fmt[i] - '0');
            ++i;
        }
        (void)has_width;

        int precision = -1;
        if (i < fmt.size() && fmt[i] == '.') {
            ++i;
            precision = 0;
            while (i < fmt.size() && fmt[i] >= '0' && fmt[i] <= '9') {
                precision = precision * 10 + (fmt[i] - '0');
                ++i;
            }
        }

        if (i >= fmt.size()) break;
        char conv = fmt[i];

        switch (conv) {
            case 'd':
            case 'i': {
                std::string body = detail::format_signed(next_arg().as_int(), f);
                detail::emit_padded(out, body, f, width, true);
                break;
            }
            case 'u': {
                std::string body = detail::format_unsigned(next_arg().as_uint(), 10, false);
                detail::emit_padded(out, body, f, width, true);
                break;
            }
            case 'x': case 'X': {
                std::string body = detail::format_unsigned(next_arg().as_uint(), 16, conv == 'X');
                detail::emit_padded(out, body, f, width, true);
                break;
            }
            case 'o': {
                std::string body = detail::format_unsigned(next_arg().as_uint(), 8, false);
                detail::emit_padded(out, body, f, width, true);
                break;
            }
            case 'c': {
                std::string body(1, static_cast<char>(next_arg().as_int()));
                detail::emit_padded(out, body, f, width, false);
                break;
            }
            case 's': {
                const arg& a = next_arg();
                std::string_view s = a.is_null() ? std::string_view{"(null)"} : a.as_string();
                if (precision >= 0 && static_cast<std::size_t>(precision) < s.size()) {
                    s = s.substr(0, static_cast<std::size_t>(precision));
                }
                detail::emit_padded(out, s, f, width, false);
                break;
            }
            case 'z': {
                // %z: like %s (mprintf's variant that additionally frees its
                // string argument after use -- moot in C++, where args are
                // never owned by vformat()).
                const arg& a = next_arg();
                std::string_view s = a.is_null() ? std::string_view{"(null)"} : a.as_string();
                detail::emit_padded(out, s, f, width, false);
                break;
            }
            case 'q': case 'w': {
                const arg& a = next_arg();
                std::string body = detail::double_quote_char(
                    a.as_string(), conv == 'w' ? '"' : '\'');
                detail::emit_padded(out, body, f, width, false);
                break;
            }
            case 'Q': {
                const arg& a = next_arg();
                if (a.is_null()) {
                    detail::emit_padded(out, "NULL", f, width, false);
                } else {
                    std::string body = "'" + detail::double_quote_char(a.as_string(), '\'') + "'";
                    detail::emit_padded(out, body, f, width, false);
                }
                break;
            }
            case 'f': case 'e': case 'g': case 'E': case 'G': {
                double v = next_arg().as_double();
                char buf[64];
                std::chars_format cf = conv == 'f' ? std::chars_format::fixed
                    : (conv == 'e' || conv == 'E') ? std::chars_format::scientific
                    : std::chars_format::general;
                auto result = precision >= 0
                    ? std::to_chars(buf, buf + sizeof(buf), v, cf, precision)
                    : std::to_chars(buf, buf + sizeof(buf), v, cf, 6);
                std::string body(buf, result.ptr);
                if (conv == 'E' || conv == 'G') {
                    for (char& ch : body) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                }
                if (v >= 0 && f.plus_sign) body = "+" + body;
                detail::emit_padded(out, body, f, width, true);
                break;
            }
            case 'p': {
                std::string body = "0x" + detail::format_unsigned(next_arg().as_uint(), 16, false);
                detail::emit_padded(out, body, f, width, false);
                break;
            }
            default:
                out.push_back('%');
                out.push_back(conv);
                break;
        }
    }
    return out;
}

template<class... Args>
std::string format(std::string_view fmt, Args&&... args) {
    std::vector<arg> collected{arg(std::forward<Args>(args))...};
    return vformat(fmt, collected);
}

} // namespace sqlite::utils::text
