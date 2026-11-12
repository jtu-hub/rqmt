#pragma once

#include <array>
#include <stdint.h>

#include "util.h"
#include "keywords.h"

constexpr auto semantic_token_types = std::to_array<std::string_view>({
    "keyword",
    "fence",
    "delimiter",
    "variable",
    "namespace",
    "operator",
    "escape",
    "entity",
    "number",
    "string",
    "comment",
});

constexpr auto additiona_modifers = std::to_array<std::string_view>({
    "attribute",
    "static",
    "entity",
    "declaration",
    "reference",
    "readonly",
    "alias",
    "open",
    "close",
    "list",
    "relative",
    "name",
    "label",
    "id",
    "value",
    "date",
    "filepath",
    "documentation",
});

constexpr auto semantic_token_modifiers = unique(limit<32>(concat(entity_keywords, additiona_modifers)));