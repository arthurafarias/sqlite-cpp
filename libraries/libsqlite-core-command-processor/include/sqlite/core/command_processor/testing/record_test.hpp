#pragma once

#include "../record.hpp"

#include <sqlite/utils/testing/test_group.hpp>

#include <limits>
#include <vector>

namespace sqlite::core::command_processor::testing {

using sqlite::utils::testing::test_group;

namespace {

std::vector<vm::aux::mem> round_trip(const std::vector<vm::aux::mem>& values) {
    return record::decode_record(record::encode_record(values));
}

} // namespace

inline const test_group record_tests("record", {
    {"round-trips NULL, integers at each width boundary, and the 0/1 special cases", [](auto& ctx) {
        std::vector<vm::aux::mem> values = {
            vm::aux::mem::make_null(),
            vm::aux::mem::make_integer(0),
            vm::aux::mem::make_integer(1),
            vm::aux::mem::make_integer(-1),
            vm::aux::mem::make_integer(127),
            vm::aux::mem::make_integer(-128),
            vm::aux::mem::make_integer(128),
            vm::aux::mem::make_integer(32767),
            vm::aux::mem::make_integer(-32768),
            vm::aux::mem::make_integer(8388607),
            vm::aux::mem::make_integer(-8388608),
            vm::aux::mem::make_integer(2147483647),
            vm::aux::mem::make_integer(-2147483648LL),
            vm::aux::mem::make_integer(std::numeric_limits<std::int64_t>::max()),
            vm::aux::mem::make_integer(std::numeric_limits<std::int64_t>::min()),
        };
        auto out = round_trip(values);
        ctx.equal(out.size(), values.size());
        for (std::size_t i = 0; i < values.size(); ++i) {
            ctx.check(out[i].type() == values[i].type());
            if (!values[i].is_null()) ctx.check(out[i].as_integer() == values[i].as_integer());
        }
    }},
    {"round-trips real values, including negative and fractional", [](auto& ctx) {
        std::vector<vm::aux::mem> values = {
            vm::aux::mem::make_real(0.0),
            vm::aux::mem::make_real(-1.5),
            vm::aux::mem::make_real(3.14159265358979),
            vm::aux::mem::make_real(-1e300),
        };
        auto out = round_trip(values);
        for (std::size_t i = 0; i < values.size(); ++i) {
            ctx.check(out[i].type() == vm::aux::value_type::real);
            ctx.check(out[i].as_real() == values[i].as_real());
        }
    }},
    {"round-trips text, including empty text", [](auto& ctx) {
        std::vector<vm::aux::mem> values = {
            vm::aux::mem::make_text(""),
            vm::aux::mem::make_text("hello"),
            vm::aux::mem::make_text(std::string(500, 'x')), // forces a multi-byte serial-type varint
        };
        auto out = round_trip(values);
        for (std::size_t i = 0; i < values.size(); ++i) {
            ctx.check(out[i].type() == vm::aux::value_type::text);
            ctx.check(out[i].as_text() == values[i].as_text());
        }
    }},
    {"round-trips blob, including empty blob", [](auto& ctx) {
        std::vector<std::byte> empty_blob;
        std::vector<std::byte> small_blob{std::byte{0x00}, std::byte{0xff}, std::byte{0x7f}};
        std::vector<vm::aux::mem> values = {
            vm::aux::mem::make_blob(empty_blob),
            vm::aux::mem::make_blob(small_blob),
        };
        auto out = round_trip(values);
        for (std::size_t i = 0; i < values.size(); ++i) {
            ctx.check(out[i].type() == vm::aux::value_type::blob);
            ctx.check(*out[i].as_blob_if() == *values[i].as_blob_if());
        }
    }},
    {"round-trips a mixed row in column order", [](auto& ctx) {
        std::vector<vm::aux::mem> values = {
            vm::aux::mem::make_integer(42),
            vm::aux::mem::make_text("alice"),
            vm::aux::mem::make_null(),
            vm::aux::mem::make_real(9.5),
        };
        auto out = round_trip(values);
        ctx.equal(out.size(), std::size_t{4});
        ctx.check(out[0].as_integer() == 42);
        ctx.check(out[1].as_text() == "alice");
        ctx.check(out[2].is_null());
        ctx.check(out[3].as_real() == 9.5);
    }},
});

} // namespace sqlite::core::command_processor::testing
