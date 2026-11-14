#pragma once

#include "tokenizer.h"
#include "token_definition.h"

enum class ParserMainState
{
    Invalid = -1,
    NewLine,
    Indent,
    Content
};

struct Indent
{
    int level = 0;
    int depth = 0;

    int level_change = 0;
    bool irregular_depth = false;
};

class ParserContext
{
    const LineOffsets &offs;
    const Tokens &tks;
    const std::string &input;
    int current_token_idx = -1;

public:
    Indent indent{0};
    ParserMainState main_state = ParserMainState::NewLine;
    static constexpr const Token sentinel_token = Token{{}, TokenType::Invalid};
    ParserContext(const std::string &input, const LineOffsets &offs, const Tokens &tks);
    bool next();
    const Token &peek(int peek_idx = 1) const;
    bool advance(const int advance_offset);
    std::string_view tokenView() const;
    std::string_view peekView(int peek_idx) const;
    const Token &currentToken() const;
    bool isCurrentToken(TokenType type, std::string_view value) const;
};
