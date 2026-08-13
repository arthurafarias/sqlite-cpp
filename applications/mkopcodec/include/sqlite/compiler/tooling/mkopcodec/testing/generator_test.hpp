#pragma once

#include "../generator.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <sstream>
#include <string>

namespace sqlite::compiler::tooling::mkopcodec::testing {

using sqlite::utils::testing::test_group;

inline const test_group generator_tests("generator", {
    {"emits azName entries in numeric order with synopsis text", [](auto& ctx) {
        std::istringstream in(
            "#define OP_Savepoint        0\n"
            "#define OP_Add               1 /* same as TK_PLUS, synopsis: r[P3]=r[P1]+r[P2] */\n");
        std::string out = generate(in);
        ctx.check(out.find("/*   0 */ \"Savepoint\"        OpHelp(\"\"),") != std::string::npos);
        ctx.check(out.find("/*   1 */ \"Add\"              OpHelp(\"r[P3]=r[P1]+r[P2]\"),") != std::string::npos);
    }},
    {"emits the fixed preamble and OpHelp macro guard", [](auto& ctx) {
        std::istringstream in("#define OP_Noop 0\n");
        std::string out = generate(in);
        ctx.check(out.find("const char *sqlite3OpcodeName(int i){") != std::string::npos);
        ctx.check(out.find("# define OpHelp(X) \"\\0\" X") != std::string::npos);
    }},
});

} // namespace sqlite::compiler::tooling::mkopcodec::testing
