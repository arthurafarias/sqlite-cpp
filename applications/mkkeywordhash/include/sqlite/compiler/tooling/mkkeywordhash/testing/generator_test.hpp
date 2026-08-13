#pragma once

#include "../generator.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <string>

namespace sqlite::compiler::tooling::mkkeywordhash::testing {

using sqlite::utils::testing::test_group;

inline const test_group generator_tests("generator", {
    {"emits sqlite3KeywordCode and the fixed public API surface", [](auto& ctx) {
        std::string out = generate();
        ctx.check(out.find("int sqlite3KeywordCode(const unsigned char *z, int n){") != std::string::npos);
        ctx.check(out.find("int sqlite3_keyword_name(int i,const char **pzName,int *pnName){") != std::string::npos);
        ctx.check(out.find("int sqlite3_keyword_count(void){ return SQLITE_N_KEYWORD; }") != std::string::npos);
        ctx.check(out.find("int sqlite3_keyword_check(const char *zName, int nName){") != std::string::npos);
    }},
    {"drops WITHIN (mask 0 in this workspace's fixed build config) but keeps WITHOUT", [](auto& ctx) {
        std::string out = generate();
        ctx.check(out.find("#define SQLITE_N_KEYWORD 147") != std::string::npos);
    }},
    {"maps a representative keyword to its parser token constant", [](auto& ctx) {
        std::string out = generate();
        ctx.check(out.find("TK_SELECT") != std::string::npos);
        ctx.check(out.find("TK_WHERE") != std::string::npos);
    }},
});

} // namespace sqlite::compiler::tooling::mkkeywordhash::testing
