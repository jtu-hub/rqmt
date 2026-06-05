#include "cpp_util.h"

#include <string>
#include <filesystem>
#include <charconv>

std::string getCodeSource(const char* file, int line)
{
    const std::string filename =
        std::filesystem::path(file).filename().string();

    return "rqmtparser:" +
           filename +
           ":" +
           std::to_string(line);
}

bool parseInt(std::string_view s, int& value) {
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

    return ec == std::errc{} && ptr == s.data() + s.size();
};