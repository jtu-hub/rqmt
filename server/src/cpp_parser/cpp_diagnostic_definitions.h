#pragma once

#include <string>
#include <array>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class DiagnosticSeverity {
    none = 0,
    error,
    warning,
    info,
    hint,
};

enum class DiagnosticId {
    unexpected_indent = 0,
    mixed_indent,
    indent_not_multiple,
    unexpected_indent_depth,
    unexpected_token,
    expecting_new_line,
    shallow_indent,
    shallow_indent_hint,
    missing_block_delimiter,
    wrong_ref_pattern,
    
    count
};

class Diagnostic {
    public:
        DiagnosticId id;
        DiagnosticSeverity severity;
        std::string source;
        int line_start;
        int line_end;
        int column_start;
        int column_end;

        json toLSPJson() const;
};

using DiagnosticMessageArray = std::array<std::string, static_cast<std::size_t>(DiagnosticId::count)>;

const std::string mapSeverity(DiagnosticSeverity severity);

const std::string& getDiagnosticMessage(DiagnosticId diagnostic_id);

const DiagnosticMessageArray& getDiagnosticMessages();
