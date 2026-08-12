#pragma once

#include "token_type.hpp"

#include <string_view>

// Legacy source: the (tokenType, token-length) pair sqlite3GetToken() and
// sqlite3RunParser() (tokenize.c) pass around, packaged as one value type
// (SRS S3.5.1). `text` is a view into the original SQL source the caller
// supplied -- this library never copies SQL text, matching how the legacy
// parser also reads tokens directly out of the caller-owned `zSql` buffer.
namespace sqlite::compiler::tokenizer {

struct token {
    token_type type = token_type::end_of_input;
    std::string_view text;
};

} // namespace sqlite::compiler::tokenizer
