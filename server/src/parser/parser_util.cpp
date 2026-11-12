#include "parser_util.h"
#include "semantic_tokens.h"

void SemanticTokenModifierHandler::set(const std::string_view &modifier)
{
    const size_t idx = indexOf<semantic_token_modifiers>(modifier);

    if (idx < semantic_token_modifiers.size())
    {
        modifiers |= 1 << idx;
    }
}
void SemanticTokenModifierHandler::unset(const std::string_view &modifier)
{
    const size_t idx = indexOf<semantic_token_modifiers>(modifier);

    if (idx < semantic_token_modifiers.size())
    {
        modifiers &= !(1 << idx);
    }
}
bool SemanticTokenModifierHandler::has(const std::string_view &modifier)
{
    const size_t idx = indexOf<semantic_token_modifiers>(modifier);

    if (idx < semantic_token_modifiers.size())
    {
        return static_cast<bool>(modifiers & (1 << idx));
    }

    return false;
}