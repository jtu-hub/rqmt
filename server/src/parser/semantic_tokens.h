#pragma once

#include <array>
#include <stdint.h>
#include <vector>

#include "util.h"
#include "keywords.h"

using SemanticTokenModifiers = uint32_t;
using SemanticTokenType = int;
using ModifierList = std::vector<std::string_view>;

constexpr auto semantic_token_types = std::to_array<std::string_view>({
    "text",
    "keyword",
    "prefix",
    "directory",
    "delimiter",
    "resource",
    "identifier",
    "fence",
});

constexpr auto additiona_modifers = std::to_array<std::string_view>({
    "filepath",
    "relative",
    "absolute",
    "entity",
    "attribute",
    "documentation",
    "alias",
    "declaration",
    "open",
    "close",
    "id",
    "title",
    "tag-list",
});

constexpr auto semantic_token_modifiers = unique(limit<32>(concat(additiona_modifers, entity_keywords)));

inline std::string_view semanticTokenTypeName(SemanticTokenType type)
{
    if (type < 0 || static_cast<std::size_t>(type) >= semantic_token_types.size())
        return "<unknown>";

    return semantic_token_types[static_cast<std::size_t>(type)];
}

inline std::string_view semanticTokenModifierName(std::size_t index)
{
    if (index >= semantic_token_modifiers.size())
        return "<unknown>";

    return semantic_token_modifiers[index];
}

class SemanticTokenModifierHandler
{
protected:
    SemanticTokenModifiers modifiers = 0;

public:
    SemanticTokenModifierHandler(ModifierList modifiers = {});
    const SemanticTokenModifiers &getModifiers() const { return modifiers; };
    void setModifier(const std::string_view &modifier);
    void unsetModifier(const std::string_view &modifier);
    bool hasModifier(const std::string_view &modifier);

    std::vector<std::string_view> getModifierNames() const
    {
        std::vector<std::string_view> result;

        for (std::size_t i = 0; i < semantic_token_modifiers.size(); ++i)
        {
            if (modifiers & (SemanticTokenModifiers{1} << i))
                result.push_back(semantic_token_modifiers[i]);
        }

        return result;
    }
};

class SemanticTokenDefinition : public SemanticTokenModifierHandler
{

protected:
    SemanticTokenType type;

public:
    SemanticTokenDefinition(SemanticTokenType type, SemanticTokenModifierHandler modifiers) : SemanticTokenModifierHandler(modifiers),
                                                                                              type(type) {}

    SemanticTokenDefinition(std::string_view type, ModifierList modifiers = {}) : SemanticTokenModifierHandler(modifiers),
                                                                                  type(static_cast<SemanticTokenType>(indexOf<semantic_token_types>(type))) {}

    SemanticTokenDefinition(std::string_view type, SemanticTokenModifierHandler modifiers = {}) : SemanticTokenModifierHandler(modifiers),
                                                                                                  type(static_cast<SemanticTokenType>(indexOf<semantic_token_types>(type))) {}

    SemanticTokenDefinition() : SemanticTokenDefinition(-1, SemanticTokenModifierHandler()) {}

    const SemanticTokenType &getType() const { return type; }
    inline std::string_view getTypeName() const { return semanticTokenTypeName(type); }
};