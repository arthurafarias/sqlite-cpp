#pragma once

#include <algorithm>
#include <cstdio>
#include <istream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Legacy source: applications/mkopcodeh-legacy/mkopcodeh.tcl (SRS 002
// RR-5). A faithful, deterministic line-by-line port: scans a stream that
// is the concatenation of parse.h (for `#define TK_x y`) and vdbe.c (for
// `/* Opcode: ... */`/`** Synopsis: ...` comments and `case OP_x:` lines,
// with their trailing attribute-keyword comments), then assigns opcode
// numbers in the legacy tool's exact four-pass priority order and emits
// opcodes.h. See this application's docs/index.md for why that ordering
// isn't independently re-derivable from a "spec" and must be preserved
// exactly, not just approximated.
namespace sqlite::compiler::tooling::mkopcodeh {

namespace detail {

struct opcode_info {
    int op = -1;
    bool group = false;
    bool jump = false;
    bool jump0 = false;
    bool in1 = false;
    bool in2 = false;
    bool in3 = false;
    bool out2 = false;
    bool out3 = false;
    bool ncycle = false;
};

inline std::string trim_chars(std::string s, std::string_view chars) {
    std::size_t begin = s.find_first_not_of(chars);
    if (begin == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(chars);
    return s.substr(begin, end - begin + 1);
}

inline std::string trim(std::string s) { return trim_chars(std::move(s), " \t\r\n"); }

inline std::vector<std::string> split_ws(const std::string& line) {
    std::vector<std::string> out;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok) out.push_back(tok);
    return out;
}

template <class... Args>
inline void append_printf(std::string& out, const char* fmt, Args... args) {
    int needed = std::snprintf(nullptr, 0, fmt, args...);
    std::size_t old_size = out.size();
    out.resize(old_size + static_cast<std::size_t>(needed));
    std::snprintf(out.data() + old_size, static_cast<std::size_t>(needed) + 1, fmt, args...);
}

// legacy $rp2v_ops: opcodes given special processing in resolveP2Values(),
// which get the smallest numbers so that function's switch() is smaller.
inline const std::vector<std::string>& rp2v_ops() {
    static const std::vector<std::string> ops = {
        "OP_Transaction", "OP_AutoCommit", "OP_Savepoint", "OP_Checkpoint", "OP_Vacuum",
        "OP_JournalMode", "OP_VUpdate", "OP_VFilter", "OP_Init",
    };
    return ops;
}

} // namespace detail

inline std::string generate(std::istream& parse_h_and_vdbe_c) {
    using detail::opcode_info;

    std::map<std::string, int> tk;
    std::map<std::string, opcode_info> info;
    std::map<std::string, std::string> synopsis;
    std::vector<std::string> order;
    std::map<int, std::vector<std::string>> groups_map;
    std::string current_op;
    std::string prev_name;
    int n_group = 0;

    std::string line;
    while (std::getline(parse_h_and_vdbe_c, line)) {
        if (line.rfind("#define TK_", 0) == 0) {
            std::vector<std::string> tokens = detail::split_ws(line);
            if (tokens.size() >= 3) tk[tokens[1]] = std::stoi(tokens[2]);
            continue;
        }

        // legacy regex ^.. Opcode: / ^.. Synopsis: -- exactly 2 arbitrary
        // leading chars ("/*" or "**"), then a literal " Opcode: "/" Synopsis: ".
        if (line.size() >= 2 && line.compare(2, 9, " Opcode: ") == 0) {
            std::vector<std::string> tokens = detail::split_ws(line);
            if (tokens.size() >= 3) current_op = "OP_" + tokens[2];
            continue;
        }

        if (line.size() >= 2 && line.compare(2, 11, " Synopsis: ") == 0 && !current_op.empty()) {
            synopsis[current_op] = detail::trim(line.substr(13));
            continue;
        }

        if (line.rfind("case OP_", 0) == 0) {
            std::vector<std::string> tokens = detail::split_ws(line);
            std::string name = detail::trim_chars(tokens.size() > 1 ? tokens[1] : "", ":");
            if (name == "OP_Abortable") continue; // put OP_Abortable last

            opcode_info entry;
            for (std::size_t i = 3; i + 1 < tokens.size(); ++i) {
                std::string word = detail::trim_chars(tokens[i], ",");
                if (word == "same") {
                    ++i;
                    if (i < tokens.size() && tokens[i] == "as") {
                        ++i;
                        if (i < tokens.size()) {
                            std::string sym = detail::trim_chars(tokens[i], ",");
                            auto found = tk.find(sym);
                            if (found != tk.end()) {
                                // used/def/sameas are recorded in the
                                // "Recover same-as assignments" pass below,
                                // once `info[name]` has been stored.
                                entry.op = found->second;
                            }
                        }
                    }
                } else if (word == "group") {
                    entry.group = true;
                } else if (word == "jump") {
                    entry.jump = true;
                } else if (word == "in1") {
                    entry.in1 = true;
                } else if (word == "in2") {
                    entry.in2 = true;
                } else if (word == "in3") {
                    entry.in3 = true;
                } else if (word == "out2") {
                    entry.out2 = true;
                } else if (word == "out3") {
                    entry.out3 = true;
                } else if (word == "ncycle") {
                    entry.ncycle = true;
                } else if (word == "jump0") {
                    entry.jump = true;
                    entry.jump0 = true;
                }
            }
            info[name] = entry;

            if (entry.group) {
                bool new_group = false;
                if (groups_map.count(n_group)) {
                    if (prev_name.empty() || !info[prev_name].group) new_group = true;
                }
                groups_map[n_group].push_back(name);
                if (new_group) ++n_group;
            } else {
                if (!prev_name.empty() && info[prev_name].group) ++n_group;
            }

            order.push_back(name);
            prev_name = name;
            continue;
        }
    }

    // Recover "same as TK_x" assignments (op value only known once tk[] is
    // populated, which -- for a genuine same-as opcode -- always happens
    // before its case line, since parse.h precedes vdbe.c in the input).
    std::set<int> used;
    std::map<int, std::string> def;
    std::map<int, std::string> sameas_by_value;
    for (const auto& name : order) {
        int val = info.at(name).op;
        if (val >= 0) {
            used.insert(val);
            def[val] = name;
            // Recover the TK_ symbol name for the comment.
            for (const auto& [sym, tkval] : tk) {
                if (tkval == val) {
                    sameas_by_value[val] = sym;
                    break;
                }
            }
        }
    }

    for (const std::string& name : {std::string("OP_Noop"), std::string("OP_Explain"), std::string("OP_Abortable")}) {
        info[name] = opcode_info{};
        order.push_back(name);
    }

    std::vector<std::vector<std::string>> groups;
    groups.reserve(static_cast<std::size_t>(n_group));
    for (int g = 0; g < n_group; ++g) groups.push_back(groups_map.at(g));

    // Pass 1: rp2v_ops get the smallest numbers.
    int cnt = -1;
    for (const auto& name : order) {
        if (std::find(detail::rp2v_ops().begin(), detail::rp2v_ops().end(), name) != detail::rp2v_ops().end()) {
            ++cnt;
            while (used.count(cnt)) ++cnt;
            info[name].op = cnt;
            used.insert(cnt);
            def[cnt] = name;
        }
    }

    // Pass 2: jump opcodes next.
    for (const auto& name : order) {
        if (info[name].op >= 0) continue;
        if (!info[name].jump) continue;
        ++cnt;
        while (used.count(cnt)) ++cnt;
        info[name].op = cnt;
        used.insert(cnt);
        def[cnt] = name;
    }

    int mx_jump = -1;
    for (const auto& name : order) {
        if (info[name].jump && info[name].op > mx_jump) mx_jump = info[name].op;
    }

    // Pass 3: grouped opcodes, contiguous runs found via linear probing.
    // `cnt` is deliberately NOT advanced by this pass -- every group's
    // search restarts from the same base (see docs/index.md).
    for (int g = 0; g < n_group; ++g) {
        const auto& grp = groups[static_cast<std::size_t>(g)];
        int g_len = static_cast<int>(grp.size());
        bool ok = false;
        int start = -1;
        int seek = cnt;
        while (!ok) {
            ++seek;
            while (used.count(seek)) ++seek;
            ok = true;
            start = seek;
            for (int j = 0; j < g_len; ++j) {
                ++seek;
                if (used.count(seek)) {
                    ok = false;
                    break;
                }
            }
        }
        int next = start;
        for (const auto& name : grp) {
            if (info[name].op >= 0) continue;
            info[name].op = next;
            used.insert(next);
            def[next] = name;
            ++next;
        }
    }

    // Pass 4: everything else fills remaining gaps.
    for (const auto& name : order) {
        if (info[name].op < 0) {
            ++cnt;
            while (used.count(cnt)) ++cnt;
            info[name].op = cnt;
            used.insert(cnt);
            def[cnt] = name;
        }
    }

    int max_val = used.empty() ? -1 : *used.rbegin();
    if (max_val > 255) throw std::runtime_error("More than 255 opcodes - VdbeOp.opcode is of type u8!");

    std::string out;
    out += "/* Automatically generated.  Do not edit */\n";
    out += "/* See the applications/mkopcodeh script for details */\n";

    for (int i = 0; i <= max_val; ++i) {
        std::string name;
        if (!def.count(i)) {
            name = "OP_NotUsed_" + std::to_string(i);
            def[i] = name;
        } else {
            name = def[i];
        }
        detail::append_printf(out, "#define %-16s %3d", name.c_str(), i);

        std::vector<std::string> com;
        bool has_info = info.count(name) != 0;
        if (has_info && info[name].jump0) {
            com.push_back("jump0");
        } else if (has_info && info[name].jump) {
            com.push_back("jump");
        }
        if (sameas_by_value.count(i)) com.push_back("same as " + sameas_by_value[i]);
        if (synopsis.count(name)) com.push_back("synopsis: " + synopsis[name]);

        if (!com.empty()) {
            std::string joined;
            for (std::size_t k = 0; k < com.size(); ++k) {
                if (k) joined += ", ";
                joined += com[k];
            }
            detail::append_printf(out, " /* %-42s */", joined.c_str());
        }
        out += "\n";
    }

    if (max_val > 255) throw std::runtime_error("More than 255 opcodes - VdbeOp.opcode is of type u8!");

    // Generate the bitvectors.
    std::vector<int> bv(static_cast<std::size_t>(max_val + 1), 0);
    for (int i = 0; i <= max_val; ++i) {
        int x = 0;
        const std::string& name = def[i];
        if (name.rfind("OP_NotUsed", 0) != 0) {
            const opcode_info& e = info[name];
            if (e.jump) x += 1;
            if (e.in1) x += 2;
            if (e.in2) x += 4;
            if (e.in3) x += 8;
            if (e.out2) x += 16;
            if (e.out3) x += 32;
            if (e.ncycle) x += 64;
            if (e.jump0) x += 128;
        }
        bv[static_cast<std::size_t>(i)] = x;
    }

    out += "\n";
    out += "/* Properties such as \"out2\" or \"jump\" that are specified in\n";
    out += "** comments following the \"case\" for each opcode in the vdbe.c\n";
    out += "** are encoded into bitvectors as follows:\n";
    out += "*/\n";
    out += "#define OPFLG_JUMP        0x01  /* jump:  P2 holds jmp target */\n";
    out += "#define OPFLG_IN1         0x02  /* in1:   P1 is an input */\n";
    out += "#define OPFLG_IN2         0x04  /* in2:   P2 is an input */\n";
    out += "#define OPFLG_IN3         0x08  /* in3:   P3 is an input */\n";
    out += "#define OPFLG_OUT2        0x10  /* out2:  P2 is an output */\n";
    out += "#define OPFLG_OUT3        0x20  /* out3:  P3 is an output */\n";
    out += "#define OPFLG_NCYCLE      0x40  /* ncycle:Cycles count against P1 */\n";
    out += "#define OPFLG_JUMP0       0x80  /* jump0:  P2 might be zero */\n";
    out += "#define OPFLG_INITIALIZER {\\\n";
    for (int i = 0; i <= max_val; ++i) {
        if (i % 8 == 0) detail::append_printf(out, "/* %3d */", i);
        detail::append_printf(out, " 0x%02x,", bv[static_cast<std::size_t>(i)]);
        if (i % 8 == 7) out += "\\\n";
    }
    out += "}\n";
    out += "\n";
    out += "/* The resolve3P2Values() routine is able to run faster if it knows\n";
    out += "** the value of the largest JUMP opcode.  The smaller the maximum\n";
    out += "** JUMP opcode the better, so the mkopcodeh.tcl script that\n";
    out += "** generated this include file strives to group all JUMP opcodes\n";
    out += "** together near the beginning of the list.\n";
    out += "*/\n";
    detail::append_printf(out, "#define SQLITE_MX_JUMP_OPCODE  %d  /* Maximum JUMP opcode */\n", mx_jump);

    return out;
}

} // namespace sqlite::compiler::tooling::mkopcodeh
