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

template <std::size_t N>
constexpr const std::string_view &find_in_array(const std::array<std::string_view, N> array, std::string_view word)
{
    for (const auto &array_entry : array)
        if (array_entry == word)
            return array_entry;
    return std::string_view();
}
