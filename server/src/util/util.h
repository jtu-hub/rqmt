#pragma once

#include <string>
#include <variant>
#include <iostream>
#include <array>
#include <cstddef>

#include "errors.h"

#define m_file_src getCodeSource(__FILE__, __LINE__)

#define _m_str_helper(x) #x
#define m_str(x) _m_str_helper(x)

using file_offset_t = size_t;
using line_number_t = size_t;

/**
 * \brief Generates a formatted diagnostic source string for error reporting.
 * \param file  The source file path from which the diagnostic originates.
 * \param line  The line number in the source file where the diagnostic originates.
 * \return A statically allocated string containing the formatted diagnostic source in the format
 *         "rqmtparser:<filename>:<line>".
 */
std::string getCodeSource(const char *file, int line);

bool parseInt(std::string_view s, int &value);

template <auto &Legend>
consteval size_t constIndexOf(std::string_view word)
{
    for (size_t i = 0; i < Legend.size(); ++i)
    {
        if (Legend[i] == word)
        {
            return i;
        }
    }

    throw std::runtime_error("unknown token in legend"); // reached -> not a constant expression -> build error
}

template <auto &Legend>
constexpr size_t indexOf(std::string_view word)
{
    for (size_t i = 0; i < Legend.size(); ++i)
    {
        if (Legend[i] == word)
            return i;
    }

    return Legend.size();
}

template <typename T, std::size_t N1, std::size_t N2>
constexpr std::array<T, N1 + N2> concat(const std::array<T, N1> &a, const std::array<T, N2> &b)
{
    std::array<T, N1 + N2> out{};

    for (std::size_t i = 0; i < N1; ++i)
        out[i] = a[i];

    for (std::size_t i = 0; i < N2; ++i)
        out[N1 + i] = b[i];

    return out;
}

template <std::size_t N, typename T, std::size_t M>
constexpr std::array<T, M> limit(const std::array<T, M> &arr)
{
    static_assert(M <= N, "Limit reached");
    return arr;
}

template <typename T, std::size_t N>
constexpr std::array<T, N> unique(const std::array<T, N> &arr)
{
    for (std::size_t i = 0; i < N; ++i)
    {
        for (std::size_t j = i + 1; j < N; ++j)
        {
            if (arr[i] == arr[j])
                throw std::runtime_error("Duplicate array entry");
        }
    }

    return arr;
}
