#include <string_view>

#include "token_definition.h"

namespace TokenDefinitions
{
    constexpr inline bool isNumber(const char &c)
    {
        if ('0' <= c && c <= '9')
        {
            return true;
        }
        return false;
    }
    constexpr inline bool isFence(const char &c)
    {
        return std::string_view("[]{}$`()\"'").find(c) != std::string_view::npos;
    }

    constexpr inline bool isPunctuation(char c)
    {
        return std::string_view(".,_:?!").find(c) != std::string_view::npos;
    }

    constexpr inline bool isModifier(char c)
    {
        return std::string_view("%\\@").find(c) != std::string_view::npos;
    }

    TokenType getTokenDefinitionFor(const char &c)
    {
        switch (c)
        {
        case '\0':
            return TokenType::Invalid;
        case '\r':
        case '\n':
            return TokenType::NewLine;
        case ' ':
            return TokenType::Spaces;
        default:
        {
            if (isNumber(c))
                return TokenType::Number;
            else if (isFence(c))
                return TokenType::Fence;
            else if (isPunctuation(c))
                return TokenType::Punctuation;
            else if (isModifier(c))
                return TokenType::Modifier;
            else
                return TokenType::Text;
        }
        }
    }
}