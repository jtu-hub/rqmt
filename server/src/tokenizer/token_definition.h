#pragma once

#include "util.h"

namespace TokenDefinitions
{

    enum class TokenType
    {
        Invalid = -1,
        NewLine,
        Spaces,
        Punctuation,
        Text,
        Number,
        Fence,
        Modifier
    };

    struct Token
    {
        TokenType type;
        file_offset_t offset;
        file_offset_t length;
    };

    TokenType getTokenDefinitionFor(const char &c);

} // namespace TokenDefinitions
