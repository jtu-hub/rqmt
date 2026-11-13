#pragma once

#include <array>
#include <cstddef>
#include <string_view>

constexpr auto entity_keywords = std::to_array<std::string_view>({
    "req",
    "fr",
    "br",
    "nfr",
    "ur",
    "sr",
    "tag",
    "def",
});

constexpr auto attribute_keywords = std::to_array<std::string_view>({
    "author",
    "date",
    "test",
});
