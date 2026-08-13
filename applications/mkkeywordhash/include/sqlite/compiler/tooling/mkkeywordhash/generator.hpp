#pragma once

#include "detail/keyword_table.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

// Legacy source: applications/mkkeywordhash-legacy/mkkeywordhash.c (SRS 002
// RR-5). A faithful, deterministic line-by-line port -- same Keyword-table
// preprocessing (substring embedding, longest-suffix computation, offset
// packing via chained tail-sharing), the same O(n^2) hash-table-size
// search, and the same emitted C source (zKWText/aKWHash/aKWNext/aKWLen/
// aKWOffset/aKWCode tables plus the keywordCode/sqlite3KeywordCode/
// sqlite3_keyword_name/sqlite3_keyword_count/sqlite3_keyword_check function
// bodies, emitted as literal text -- this tool generates C, not C++,
// exactly like the legacy tool it replaces). There is no CLI input: the
// legacy tool's keyword table is compiled in, and so is this one's (see
// detail/keyword_table.hpp).
//
// Every emission step below uses std::snprintf with the exact same format
// strings the legacy tool passed to printf, rather than hand-rolled
// formatting, specifically so column widths/padding can't drift from the
// legacy output by transcription error.
namespace sqlite::compiler::tooling::mkkeywordhash {

namespace detail {

struct working_keyword {
    std::string_view name;  // shrinks via remove_prefix (mirrors legacy zName+=k / len-=k)
    std::string_view token_type;
    int priority = 0;
    int id = 0;
    int hash = 0;
    int offset = 0;
    int prefix = 0;
    int longest_suffix = 0;
    int next = 0; // legacy iNext, 1-based (0 == end of chain)
    int substr_id = 0;
    int substr_offset = 0;
    std::string orig_name; // legacy zOrigName, captured before any truncation
};

// legacy charMap(X): maps ASCII alphabetics to lower-case for hashing.
inline constexpr unsigned char char_map(unsigned char c) { return static_cast<unsigned char>(0x20 | c); }

inline constexpr int hash_c0 = 4;
inline constexpr int hash_c1 = 3;
inline constexpr int hash_c2 = 1;

inline int compute_hash(std::string_view name) {
    return (char_map(static_cast<unsigned char>(name.front())) * hash_c0) ^
           (char_map(static_cast<unsigned char>(name.back())) * hash_c1) ^
           (static_cast<int>(name.size()) * hash_c2);
}

// legacy reorder(): bubbles a higher-priority keyword earlier in the hash
// collision chain represented by 1-based `next` links (0 == end of chain).
// `slot` is either a hash-bucket head (aKWHash[h]) or a chain link
// (table[i].next); an int& works for both uniformly.
inline void reorder(std::vector<working_keyword>& table, int& slot) {
    int i = slot - 1;
    if (i < 0) return;
    int j = table[static_cast<std::size_t>(i)].next;
    if (j == 0) return;
    --j;
    if (table[static_cast<std::size_t>(i)].priority >= table[static_cast<std::size_t>(j)].priority) return;
    table[static_cast<std::size_t>(i)].next = table[static_cast<std::size_t>(j)].next;
    table[static_cast<std::size_t>(j)].next = i + 1;
    slot = j + 1;
    reorder(table, table[static_cast<std::size_t>(i)].next);
}

template <class... Args>
inline void append_printf(std::string& out, const char* fmt, Args... args) {
    int needed = std::snprintf(nullptr, 0, fmt, args...);
    std::size_t old_size = out.size();
    out.resize(old_size + static_cast<std::size_t>(needed));
    std::snprintf(out.data() + old_size, static_cast<std::size_t>(needed) + 1, fmt, args...);
}

} // namespace detail

inline std::string generate() {
    using detail::append_printf;
    using detail::working_keyword;

    // Build the working table, dropping entries whose mask is 0 (only
    // WITHIN, in this workspace's fixed build configuration -- see
    // detail/keyword_table.hpp).
    std::vector<working_keyword> table;
    table.reserve(detail::keyword_table.size());
    for (const auto& source : detail::keyword_table) {
        if (!source.kept) continue;
        working_keyword kw;
        kw.name = source.name;
        kw.token_type = source.token_type;
        kw.priority = source.priority;
        table.push_back(kw);
    }

    // Fill in lengths/hashes/ids for all entries.
    int total_len = 0;
    for (int i = 0; i < static_cast<int>(table.size()); ++i) {
        working_keyword& kw = table[static_cast<std::size_t>(i)];
        kw.orig_name = std::string(kw.name);
        total_len += static_cast<int>(kw.name.size());
        kw.hash = detail::compute_hash(kw.name);
        kw.id = i + 1;
    }

    // Sort from shortest to longest keyword (ties broken by name).
    std::sort(table.begin(), table.end(), [](const working_keyword& a, const working_keyword& b) {
        if (a.name.size() != b.name.size()) return a.name.size() < b.name.size();
        return a.name < b.name;
    });

    const int n_keyword = static_cast<int>(table.size());

    // Look for short keywords embedded in longer keywords.
    for (int i = n_keyword - 2; i >= 0; --i) {
        working_keyword& p = table[static_cast<std::size_t>(i)];
        for (int j = n_keyword - 1; j > i && p.substr_id == 0; --j) {
            working_keyword& other = table[static_cast<std::size_t>(j)];
            if (other.substr_id != 0) continue;
            if (other.name.size() <= p.name.size()) continue;
            for (int k = 0; k <= static_cast<int>(other.name.size()) - static_cast<int>(p.name.size()); ++k) {
                if (other.name.substr(static_cast<std::size_t>(k), p.name.size()) == p.name) {
                    p.substr_id = other.id;
                    p.substr_offset = k;
                    break;
                }
            }
        }
    }

    // Compute the longest_suffix value for every word.
    for (int i = 0; i < n_keyword; ++i) {
        working_keyword& p = table[static_cast<std::size_t>(i)];
        if (p.substr_id != 0) continue;
        for (int j = 0; j < n_keyword; ++j) {
            if (j == i) continue;
            working_keyword& other = table[static_cast<std::size_t>(j)];
            if (other.substr_id != 0) continue;
            for (int k = p.longest_suffix + 1; k < static_cast<int>(p.name.size()) && k < static_cast<int>(other.name.size()); ++k) {
                if (p.name.substr(p.name.size() - static_cast<std::size_t>(k), static_cast<std::size_t>(k)) ==
                    other.name.substr(0, static_cast<std::size_t>(k))) {
                    p.longest_suffix = k;
                }
            }
        }
    }

    // Sort into reverse order by longest_suffix (ties broken by name).
    std::sort(table.begin(), table.end(), [](const working_keyword& a, const working_keyword& b) {
        if (a.longest_suffix != b.longest_suffix) return a.longest_suffix > b.longest_suffix;
        return a.name < b.name;
    });

    // Fill in the offset for all entries -- a chained tail-sharing packing
    // scheme. Faithfully mirrors the legacy loop's mid-loop `p` reassignment
    // and `j`/`k` resets; do not "clean up" this control flow, the ordering
    // is load-bearing for byte-for-byte parity (RR-7).
    int n_char = 0;
    for (int i = 0; i < n_keyword; ++i) {
        int p_idx = i;
        if (table[static_cast<std::size_t>(p_idx)].offset > 0 || table[static_cast<std::size_t>(p_idx)].substr_id != 0) continue;
        table[static_cast<std::size_t>(p_idx)].offset = n_char;
        n_char += static_cast<int>(table[static_cast<std::size_t>(p_idx)].name.size());
        for (int k = static_cast<int>(table[static_cast<std::size_t>(p_idx)].name.size()) - 1; k >= 1; --k) {
            for (int j = i + 1; j < n_keyword; ++j) {
                int other_idx = j;
                working_keyword& other = table[static_cast<std::size_t>(other_idx)];
                if (other.offset > 0 || other.substr_id != 0) continue;
                if (static_cast<int>(other.name.size()) <= k) continue;
                std::string_view p_name = table[static_cast<std::size_t>(p_idx)].name;
                if (p_name.substr(p_name.size() - static_cast<std::size_t>(k), static_cast<std::size_t>(k)) ==
                    other.name.substr(0, static_cast<std::size_t>(k))) {
                    p_idx = other_idx;
                    table[static_cast<std::size_t>(p_idx)].offset = n_char - k;
                    n_char = table[static_cast<std::size_t>(p_idx)].offset + static_cast<int>(table[static_cast<std::size_t>(p_idx)].name.size());
                    table[static_cast<std::size_t>(p_idx)].name.remove_prefix(static_cast<std::size_t>(k));
                    table[static_cast<std::size_t>(p_idx)].prefix = k;
                    j = i;
                    k = static_cast<int>(table[static_cast<std::size_t>(p_idx)].name.size());
                }
            }
        }
    }
    for (int i = 0; i < n_keyword; ++i) {
        working_keyword& p = table[static_cast<std::size_t>(i)];
        if (p.substr_id != 0) {
            auto found = std::find_if(table.begin(), table.end(),
                                       [&](const working_keyword& kw) { return kw.id == p.substr_id; });
            p.offset = found->offset + p.substr_offset;
        }
    }

    // Sort by offset (ties broken by descending id).
    std::sort(table.begin(), table.end(), [](const working_keyword& a, const working_keyword& b) {
        if (a.offset != b.offset) return a.offset < b.offset;
        return a.id > b.id;
    });

    // Figure out how big to make the hash table in order to minimize the
    // number of collisions.
    int best_size = n_keyword;
    int best_count = n_keyword * n_keyword;
    std::vector<int> kw_hash(static_cast<std::size_t>(2 * n_keyword + 1), 0);
    for (int i = n_keyword / 2; i <= 2 * n_keyword; ++i) {
        if (i <= 0) continue;
        for (int j = 0; j < i; ++j) kw_hash[static_cast<std::size_t>(j)] = 0;
        for (int j = 0; j < n_keyword; ++j) {
            int h = table[static_cast<std::size_t>(j)].hash % i;
            kw_hash[static_cast<std::size_t>(h)] *= 2;
            kw_hash[static_cast<std::size_t>(h)]++;
        }
        int count = 0;
        for (int j = 0; j < i; ++j) count += kw_hash[static_cast<std::size_t>(j)];
        if (count < best_count) {
            best_count = count;
            best_size = i;
        }
    }

    // Compute the hash chains.
    kw_hash.assign(static_cast<std::size_t>(best_size), 0);
    for (int i = 0; i < n_keyword; ++i) {
        int h = table[static_cast<std::size_t>(i)].hash % best_size;
        table[static_cast<std::size_t>(i)].next = kw_hash[static_cast<std::size_t>(h)];
        kw_hash[static_cast<std::size_t>(h)] = i + 1;
        detail::reorder(table, kw_hash[static_cast<std::size_t>(h)]);
    }

    // Begin generating code.
    std::string out;
    out +=
        "/***** This file contains automatically generated code ******\n"
        "**\n"
        "** The code in this file has been automatically generated by\n"
        "**\n"
        "**   applications/mkkeywordhash\n"
        "**\n"
        "** The code in this file implements a function that determines whether\n"
        "** or not a given identifier is really an SQL keyword.  The same thing\n"
        "** might be implemented more directly using a hand-written hash table.\n"
        "** But by using this automatically generated code, the size of the code\n"
        "** is substantially reduced.  This is important for embedded applications\n"
        "** on platforms with limited memory.\n"
        "*/\n";
    append_printf(out, "/* Hash score: %d */\n", best_count);
    append_printf(out, "/* zKWText[] encodes %d bytes of keyword text in %d bytes */\n", total_len + n_keyword, n_char + 1);

    std::string kw_text(static_cast<std::size_t>(n_char + 1), '\0');
    {
        int j = 0;
        int k = 0;
        for (int i = 0; i < n_keyword; ++i) {
            working_keyword& p = table[static_cast<std::size_t>(i)];
            if (p.substr_id != 0) continue;
            std::copy(p.name.begin(), p.name.end(), kw_text.begin() + k);
            k += static_cast<int>(p.name.size());
            if (j + static_cast<int>(p.name.size()) > 70) {
                append_printf(out, "%*s */\n", 74 - j, "");
                j = 0;
            }
            if (j == 0) {
                out += "/*   ";
                j = 8;
            }
            out += std::string(p.name);
            j += static_cast<int>(p.name.size());
        }
        if (j > 0) append_printf(out, "%*s */\n", 74 - j, "");
    }

    append_printf(out, "static const char zKWText[%d] = {\n", n_char);
    {
        int j = 0;
        for (int i = 0; i < n_char; ++i) {
            if (j == 0) out += "  ";
            if (kw_text[static_cast<std::size_t>(i)] == 0) {
                out += "0";
            } else {
                append_printf(out, "'%c',", kw_text[static_cast<std::size_t>(i)]);
            }
            j += 4;
            if (j > 68) {
                out += "\n";
                j = 0;
            }
        }
        if (j > 0) out += "\n";
    }
    out += "};\n";

    out += "/* aKWHash[i] is the hash value for the i-th keyword */\n";
    append_printf(out, "static const unsigned char aKWHash[%d] = {\n", best_size);
    {
        int j = 0;
        for (int i = 0; i < best_size; ++i) {
            if (j == 0) out += "  ";
            append_printf(out, " %3d,", kw_hash[static_cast<std::size_t>(i)]);
            ++j;
            if (j > 12) {
                out += "\n";
                j = 0;
            }
        }
        out += (j == 0 ? "" : "\n");
    }
    out += "};\n";

    out += "/* aKWNext[] forms the hash collision chain.  If aKWHash[i]==0\n";
    out += "** then the i-th keyword has no more hash collisions.  Otherwise,\n";
    out += "** the next keyword with the same hash is aKWHash[i]-1. */\n";
    append_printf(out, "static const unsigned char aKWNext[%d] = {0,\n", n_keyword + 1);
    {
        int j = 0;
        for (int i = 0; i < n_keyword; ++i) {
            if (j == 0) out += "  ";
            append_printf(out, " %3d,", table[static_cast<std::size_t>(i)].next);
            ++j;
            if (j > 12) {
                out += "\n";
                j = 0;
            }
        }
        out += (j == 0 ? "" : "\n");
    }
    out += "};\n";

    out += "/* aKWLen[i] is the length (in bytes) of the i-th keyword */\n";
    append_printf(out, "static const unsigned char aKWLen[%d] = {0,\n", n_keyword + 1);
    {
        int j = 0;
        for (int i = 0; i < n_keyword; ++i) {
            if (j == 0) out += "  ";
            append_printf(out, " %3d,", static_cast<int>(table[static_cast<std::size_t>(i)].name.size()) + table[static_cast<std::size_t>(i)].prefix);
            ++j;
            if (j > 12) {
                out += "\n";
                j = 0;
            }
        }
        out += (j == 0 ? "" : "\n");
    }
    out += "};\n";

    out += "/* aKWOffset[i] is the index into zKWText[] of the start of\n";
    out += "** the text for the i-th keyword. */\n";
    append_printf(out, "static const unsigned short int aKWOffset[%d] = {0,\n", n_keyword + 1);
    {
        int j = 0;
        for (int i = 0; i < n_keyword; ++i) {
            if (j == 0) out += "  ";
            append_printf(out, " %3d,", table[static_cast<std::size_t>(i)].offset);
            ++j;
            if (j > 12) {
                out += "\n";
                j = 0;
            }
        }
        out += (j == 0 ? "" : "\n");
    }
    out += "};\n";

    out += "/* aKWCode[i] is the parser symbol code for the i-th keyword */\n";
    append_printf(out, "static const unsigned char aKWCode[%d] = {0,\n", n_keyword + 1);
    {
        int j = 0;
        for (int i = 0; i < n_keyword; ++i) {
            std::string token = std::string(table[static_cast<std::size_t>(i)].token_type);
            if (j == 0) out += "  ";
            append_printf(out, "%s,%*s", token.c_str(), static_cast<int>(14 - token.size()), "");
            ++j;
            if (j >= 5) {
                out += "\n";
                j = 0;
            }
        }
        out += (j == 0 ? "" : "\n");
    }
    out += "};\n";

    out += "/* Hash table decoded:\n";
    for (int i = 0; i < best_size; ++i) {
        int j = kw_hash[static_cast<std::size_t>(i)];
        append_printf(out, "** %3d:", i);
        while (j) {
            append_printf(out, " %s", table[static_cast<std::size_t>(j - 1)].orig_name.c_str());
            j = table[static_cast<std::size_t>(j - 1)].next;
        }
        out += "\n";
    }
    out += "*/\n";

    out += "/* Check to see if z[0..n-1] is a keyword. If it is, write the\n";
    out += "** parser symbol code for that keyword into *pType.  Always\n";
    out += "** return the integer n (the length of the token). */\n";
    out += "static i64 keywordCode(const char *z, i64 n, int *pType){\n";
    out += "  i64 i, j;\n";
    out += "  const char *zKW;\n";
    out += "  assert( n>=2 );\n";
    append_printf(out, "  i = ((charMap(z[0])*%d) %c", detail::hash_c0, '^');
    append_printf(out, " (charMap(z[n-1])*%d) %c", detail::hash_c1, '^');
    append_printf(out, " n*%d) %% %d;\n", detail::hash_c2, best_size);
    out += "  for(i=(int)aKWHash[i]; i>0; i=aKWNext[i]){\n";
    out += "    if( aKWLen[i]!=n ) continue;\n";
    out += "    zKW = &zKWText[aKWOffset[i]];\n";
    out += "#ifdef SQLITE_ASCII\n";
    out += "    if( (z[0]&~0x20)!=zKW[0] ) continue;\n";
    out += "    if( (z[1]&~0x20)!=zKW[1] ) continue;\n";
    out += "    j = 2;\n";
    out += "    while( j<n && (z[j]&~0x20)==zKW[j] ){ j++; }\n";
    out += "#endif\n";
    out += "#ifdef SQLITE_EBCDIC\n";
    out += "    if( toupper(z[0])!=zKW[0] ) continue;\n";
    out += "    if( toupper(z[1])!=zKW[1] ) continue;\n";
    out += "    j = 2;\n";
    out += "    while( j<n && toupper(z[j])==zKW[j] ){ j++; }\n";
    out += "#endif\n";
    out += "    if( j<n ) continue;\n";
    for (int i = 0; i < n_keyword; ++i) {
        append_printf(out, "    testcase( i==%d ); /* %s */\n", i + 1, table[static_cast<std::size_t>(i)].orig_name.c_str());
    }
    out += "    *pType = aKWCode[i];\n";
    out += "    break;\n";
    out += "  }\n";
    out += "  return n;\n";
    out += "}\n";
    out += "int sqlite3KeywordCode(const unsigned char *z, int n){\n";
    out += "  int id = TK_ID;\n";
    out += "  if( n>=2 ) keywordCode((char*)z, n, &id);\n";
    out += "  return id;\n";
    out += "}\n";
    append_printf(out, "#define SQLITE_N_KEYWORD %d\n", n_keyword);
    out += "int sqlite3_keyword_name(int i,const char **pzName,int *pnName){\n";
    out += "  if( i<0 || i>=SQLITE_N_KEYWORD ) return SQLITE_ERROR;\n";
    out += "  i++;\n";
    out += "  *pzName = zKWText + aKWOffset[i];\n";
    out += "  *pnName = aKWLen[i];\n";
    out += "  return SQLITE_OK;\n";
    out += "}\n";
    out += "int sqlite3_keyword_count(void){ return SQLITE_N_KEYWORD; }\n";
    out += "int sqlite3_keyword_check(const char *zName, int nName){\n";
    out += "  return TK_ID!=sqlite3KeywordCode((const u8*)zName, nName);\n";
    out += "}\n";

    return out;
}

} // namespace sqlite::compiler::tooling::mkkeywordhash
