#include "semantic_tokens.h"

SemanticTokenModifierHandler::SemanticTokenModifierHandler(std::initializer_list<std::string_view> modifiers)
{
    for (const auto &modifier : modifiers)
        setModifier(modifier);
}

void SemanticTokenModifierHandler::setModifier(const std::string_view &modifier)
{
    const size_t idx = indexOf<semantic_token_modifiers>(modifier);

    if (idx < semantic_token_modifiers.size())
    {
        modifiers |= 1 << idx;
    }
}

void SemanticTokenModifierHandler::unsetModifier(const std::string_view &modifier)
{
    const size_t idx = indexOf<semantic_token_modifiers>(modifier);

    if (idx < semantic_token_modifiers.size())
    {
        modifiers &= !(1 << idx);
    }
}

bool SemanticTokenModifierHandler::hasModifier(const std::string_view &modifier)
{
    const size_t idx = indexOf<semantic_token_modifiers>(modifier);

    if (idx < semantic_token_modifiers.size())
    {
        return static_cast<bool>(modifiers & (1 << idx));
    }

    return false;
}