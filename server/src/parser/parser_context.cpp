#include "parser_context.h"

ParserContext::ParserContext(const std::string &input, const LineOffsets &offs, const Tokens &tks) : offs(offs),
                                                                                                     tks(tks),
                                                                                                     input(input) {}
bool ParserContext::next()
{
    if (current_token_idx + 1 < tks.size())
    {
        current_token_idx++;
        return true;
    }

    return false;
}

const Token &ParserContext::peek(int peek_idx) const
{
    int look_at = current_token_idx + peek_idx;

    if (0 <= look_at && look_at < tks.size())
        return tks[look_at];
    else
        return sentinel_token;
}

bool ParserContext::advance(const int advance_offset)
{
    if (current_token_idx + advance_offset < tks.size())
    {
        current_token_idx += advance_offset;
        return true;
    }
    else
    {
        return false;
    }
}

const Token &ParserContext::currentToken() const { return tks[current_token_idx]; }

std::string_view ParserContext::tokenView() const
{
    const Token &token = currentToken();

    return std::string_view(input).substr(
        token.offset,
        token.length);
}

std::string_view ParserContext::peekView(int peek_idx) const
{
    const Token &token = peek(peek_idx);

    if (&token == &sentinel_token)
        return {};

    return std::string_view(input).substr(
        token.offset,
        token.length);
}

bool ParserContext::isCurrentToken(TokenType type, std::string_view value) const
{
    return currentToken().type == type && tokenView() == value;
}
