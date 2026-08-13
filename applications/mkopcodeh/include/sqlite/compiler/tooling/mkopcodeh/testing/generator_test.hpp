#pragma once

#include "../generator.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <sstream>
#include <string>

namespace sqlite::compiler::tooling::mkopcodeh::testing {

using sqlite::utils::testing::test_group;

inline const test_group generator_tests("generator", {
    {"assigns same-as opcodes the TK_ value and records the comment", [](auto& ctx) {
        std::istringstream in(
            "#define TK_PLUS 107\n"
            "case OP_Add: {             /* same as TK_PLUS, in1, in2, out2 */\n");
        std::string out = generate(in);
        ctx.check(out.find("#define OP_Add") != std::string::npos);
        ctx.check(out.find("107") != std::string::npos);
        ctx.check(out.find("same as TK_PLUS") != std::string::npos);
    }},
    {"assigns jump opcodes the smallest available numbers and tracks the max", [](auto& ctx) {
        std::istringstream in("case OP_Goto: {            /* jump */\ncase OP_Halt: {\n");
        std::string out = generate(in);
        ctx.check(out.find("#define OP_Goto            0 /* jump") != std::string::npos);
        ctx.check(out.find("#define OP_Halt            1") != std::string::npos);
        ctx.check(out.find("SQLITE_MX_JUMP_OPCODE  0") != std::string::npos);
    }},
    {"attaches an Opcode:/Synopsis: comment pair to the following case line", [](auto& ctx) {
        std::istringstream in(
            "/* Opcode: Add P1 P2 P3 * *\n"
            "** Synopsis: r[P3]=r[P1]+r[P2]\n"
            "*/\n"
            "case OP_Add: {             /* in1, in2, out2 */\n");
        std::string out = generate(in);
        ctx.check(out.find("synopsis: r[P3]=r[P1]+r[P2]") != std::string::npos);
    }},
    {"encodes the OPFLG bitvector for in1|in2|out2 as 0x16", [](auto& ctx) {
        std::istringstream in("case OP_Add: {             /* in1, in2, out2 */\n");
        std::string out = generate(in);
        ctx.check(out.find("0x16") != std::string::npos);
    }},
});

} // namespace sqlite::compiler::tooling::mkopcodeh::testing
