#pragma once

#include <vector>
#include <map>

#include "util.h"
#include "token_definition.h"

using Tokens = std::vector<TokenDefinitions::Token>;
using LineOffsets = std::vector<file_offset_t>;

struct TokenizerResult
{
    Tokens tokens = {};
    LineOffsets line_offsets = {
        0,
    };
};

TokenizerResult tokenize(const std::string &input);
