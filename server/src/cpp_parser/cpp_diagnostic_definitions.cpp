#include "cpp_diagnostic_definitions.h"

extern "C" {
#include "util.h"
}

constexpr std::size_t diagnostic_count =
    static_cast<std::size_t>(DiagnosticId::count);

const DiagnosticMessageArray diagnostic_msgs = [] {
    DiagnosticMessageArray arr{};

    arr[static_cast<std::size_t>(DiagnosticId::unexpected_indent)] =
        "Unexpected indentation: this line is indented without a valid parent block or indentation context.";

    arr[static_cast<std::size_t>(DiagnosticId::mixed_indent)] =
        "Mixed indentation detected: use either spaces or tabs consistently throughout the document.";

    arr[static_cast<std::size_t>(DiagnosticId::indent_not_multiple)] =
        "Inconsistent indentation width: indentation does not follow the established indentation step.";

    arr[static_cast<std::size_t>(DiagnosticId::unexpected_indent_depth)] =
        "Unexpected indentation depth: indentation level is deeper than expected.";

    arr[static_cast<std::size_t>(DiagnosticId::unexpected_token)] =
        "Unexpected token: the parser encountered a token that is not valid in this context.";

    arr[static_cast<std::size_t>(DiagnosticId::expecting_new_line)] =
        "Expected a new line before continuing.";

    arr[static_cast<std::size_t>(DiagnosticId::shallow_indent)] =
        "Indentation width is very small: using only one space per indentation level may reduce readability.";

    arr[static_cast<std::size_t>(DiagnosticId::shallow_indent_hint)] =
        "Consider using 2 or 4 spaces per indentation level for improved readability and consistency.";

    arr[static_cast<std::size_t>(DiagnosticId::missing_block_delimiter)] =
        "Missing closing delimiter for inline formatting block (e.g. '`' for inline code or '$' for inline math).";

    arr[static_cast<std::size_t>(DiagnosticId::wrong_ref_pattern)] =
        "Invalid reference format: references must start with '{', end with '}', and contain only alphanumeric characters or '.'.";
    
    return arr;
}();

const std::string mapSeverity(DiagnosticSeverity severity) {

    switch (severity)
    {
    case DiagnosticSeverity::error:
        return "error";

    case DiagnosticSeverity::warning:
        return "warning";

    case DiagnosticSeverity::info:
        return "info";

    default:
    case DiagnosticSeverity::hint:
        return "hint";
    }
}

const std::string& getDiagnosticMessage(DiagnosticId diagnostic_id) {

    if (diagnostic_id == DiagnosticId::count)
        m_index_out_of_bound(
            static_cast<int>(diagnostic_id),
            static_cast<int>(DiagnosticId::count)
        );

    return diagnostic_msgs[static_cast<std::size_t>(diagnostic_id)];
}

const DiagnosticMessageArray& getDiagnosticMessages() {
    return diagnostic_msgs;
}

json Diagnostic::toLSPJson() const {
    json diagnostic_json = {
        {"range", {
            {"start", {
                {"line", line_start},
                {"character", column_start}
            }},
            {"end", {
                {"line", line_end},
                {"character", column_end}
            }}
        }},
        {"message", getDiagnosticMessage(id)},
        {"severity", static_cast<int>(severity)},
        {"source", source}
    };

    return diagnostic_json;
}