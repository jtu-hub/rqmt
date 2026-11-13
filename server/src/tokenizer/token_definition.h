#pragma once

#include "util.h"

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

struct OffsetRange
{
    file_offset_t offset;
    file_offset_t length;
};

struct Token : public OffsetRange
{
    TokenType type;
};

TokenType getTokenDefinitionFor(const char &c);
