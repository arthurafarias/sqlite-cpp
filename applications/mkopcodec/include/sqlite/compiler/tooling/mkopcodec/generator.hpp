#pragma once

#include <cstdio>
#include <istream>
#include <map>
#include <sstream>
#include <string>

// Legacy source: applications/mkopcodec-legacy/mkopcodec.tcl (SRS 002
// RR-5). A faithful, deterministic line-by-line port: scans a stream of
// `#define OP_<name> <number> [... synopsis: <text> */]` lines (the
// opcodes.h format mkopcodeh emits) and prints the fixed
// sqlite3OpcodeName() preamble/table/postamble.
namespace sqlite::compiler::tooling::mkopcodec {

namespace detail {

inline std::string trim(std::string s) {
    std::size_t begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

template <class... Args>
inline void append_printf(std::string& out, const char* fmt, Args... args) {
    int needed = std::snprintf(nullptr, 0, fmt, args...);
    std::size_t old_size = out.size();
    out.resize(old_size + static_cast<std::size_t>(needed));
    std::snprintf(out.data() + old_size, static_cast<std::size_t>(needed) + 1, fmt, args...);
}

} // namespace detail

inline std::string generate(std::istream& opcodes_h) {
    std::string out;
    out += "/* Automatically generated.  Do not edit */\n";
    out += "/* See the applications/mkopcodec script for details. */\n";
    out += "#if !defined(SQLITE_OMIT_EXPLAIN) \\\n";
    out += " || defined(VDBE_PROFILE) \\\n";
    out += " || defined(SQLITE_DEBUG)\n";
    out += "#if defined(SQLITE_ENABLE_EXPLAIN_COMMENTS) || defined(SQLITE_DEBUG)\n";
    out += "# define OpHelp(X) \"\\0\" X\n";
    out += "#else\n";
    out += "# define OpHelp(X)\n";
    out += "#endif\n";
    out += "const char *sqlite3OpcodeName(int i){\n";
    out += " static const char *const azName[] = {\n";

    std::map<int, std::string> label;
    std::map<int, std::string> synopsis;
    int mx = 0;

    std::string line;
    while (std::getline(opcodes_h, line)) {
        if (line.rfind("#define OP_", 0) != 0) continue;

        std::istringstream tokens(line);
        std::string keyword, name, number_text;
        tokens >> keyword >> name >> number_text;
        if (name.rfind("OP_", 0) == 0) name = name.substr(3);
        int i = std::stoi(number_text);

        label[i] = name;
        if (mx < i) mx = i;

        std::string syn;
        auto start = line.find("synopsis: ");
        if (start != std::string::npos) {
            auto end = line.rfind(" */"); // greedy match, mirrors TCL's `.*`
            if (end != std::string::npos && end > start) {
                syn = detail::trim(line.substr(start + 10, end - (start + 10)));
            }
        }
        synopsis[i] = syn;
    }

    for (int i = 0; i <= mx; ++i) {
        std::string lbl = label.count(i) ? label[i] : "";
        std::string syn = synopsis.count(i) ? synopsis[i] : "";
        std::string quoted_lbl = "\"" + lbl + "\"";
        detail::append_printf(out, "    /* %3d */ %-18s OpHelp(\"%s\"),\n", i, quoted_lbl.c_str(), syn.c_str());
    }

    out += "  };\n";
    out += "  return azName[i];\n";
    out += "}\n";
    out += "#endif\n";
    return out;
}

} // namespace sqlite::compiler::tooling::mkopcodec
