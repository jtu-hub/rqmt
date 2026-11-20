#pragma once

#include <array>
#include <cstddef>
#include <string_view>

constexpr auto entity_keywords = std::to_array<std::string_view>({
    "req",
    "tag",
    "def",
});

constexpr auto attribute_keywords = std::to_array<std::string_view>({
    "author",
    "date",
    "test",
});

constexpr auto decorator_keywords = std::to_array<std::string_view>({
    "draft",
    "deprecated",
    "notimplemented",
});
