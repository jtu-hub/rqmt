#include "tokenizer.h"

class TokenizerContext
{
    const std::string &input;
    file_offset_t offset = 0;

    line_number_t line_number = 0;

    TokenizerResult res{};

public:
    TokenType state = TokenType::Invalid;
    static constexpr const char sentinel_char = '\0';

    TokenizerContext(const std::string &input) : input(input) {};

    const char &lookAhead(file_offset_t look_ahead = 1)
    {
        if (offset + look_ahead < input.size())
            return input[offset + look_ahead];
        else
            return sentinel_char;
    }

    bool next()
    {
        if (offset + 1 < input.size())
        {
            offset++;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool advance(const int advance_offset)
    {
        if (offset + advance_offset < input.size())
        {
            offset += advance_offset;
            return true;
        }
        else
        {
            return false;
        }
    }

    const char &currentChar()
    {
        if (offset < input.size())
            return input[offset];
        else
            return sentinel_char;
    }

    const file_offset_t &currentOffset()
    {
        return offset;
    }

    void newLine(file_offset_t token_length = 1)
    {
        line_number++;

        if (line_number < res.line_offsets.size())
        {
            res.line_offsets[line_number] = offset + token_length;
        }
        else if (line_number == res.line_offsets.size())
        {
            res.line_offsets.emplace_back(offset + token_length);
        }
        else
        {
            // TODO raise error
        }
    }

    void newToken(const TokenType &type, const file_offset_t &token_start, const file_offset_t &token_length)
    {
        res.tokens.emplace_back(Token{
            {token_start, token_length},
            type});
    }

    void
    resetState()
    {
        state = TokenType::Invalid;
    }

    const TokenizerResult &getResult() const { return res; };
};

TokenizerResult tokenize(const std::string &input)
{
    TokenizerContext ctx(input);

    do
    {
        const char &c = ctx.currentChar();

        TokenType type = getTokenDefinitionFor(c);

        if (ctx.state == TokenType::Invalid)
        {
            ctx.state = type;
        }

        file_offset_t token_start = ctx.currentOffset();
        file_offset_t token_length = 1;

        switch (ctx.state)
        {
        case TokenType::NewLine:
            if (c == '\r' && ctx.lookAhead() == '\n')
            {
                token_length = 2;
            }

            ctx.newLine(token_length);
            break;

        case TokenType::Spaces:
        case TokenType::Number:
        case TokenType::Text:
            while (ctx.state == getTokenDefinitionFor(ctx.lookAhead(token_length)))
            {
                token_length++;
            }
            break;

        case TokenType::Punctuation:
        case TokenType::Fence:
        case TokenType::Modifier:
        default:
            break;
        }

        ctx.newToken(ctx.state, token_start, token_length);
        ctx.resetState();
        ctx.advance(token_length - 1);

    } while (ctx.next());

    return ctx.getResult();
}