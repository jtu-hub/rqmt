#pragma once

#include <vector>
#include <variant>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class TextLocation {
    public:
        int line;
        int column;
        TextLocation();
        TextLocation(int line, int column);
        TextLocation(json lsp_location);

        json toLSPJSON() const;
};


class ColumnSpan {
    public:

        friend std::ostream& operator<<(std::ostream& os, const ColumnSpan& span) {
            os << "ColumnSpan("
            << "start: " << span.start
            << ", end: " << span.end
            << ")";

            return os;
        }

        int start;
        int end;

        ColumnSpan();
        ColumnSpan(int column_start, int column_end);

        ColumnSpan& operator+=(const ColumnSpan& other);
};

class TextSpan {
    public:
        friend std::ostream& operator<<(std::ostream& os, const TextSpan& span) {
            os << "TextSpan("
            << "value: " << span.value
            << ", span: " << span.span
            << ")";

            return os;
        }

        std::string value;
        ColumnSpan span;

        TextSpan();
        TextSpan(std::string value);
        TextSpan(std::string value, int column_start, int column_end);
        TextSpan(std::string value, ColumnSpan span);

        static TextSpan fromTextWithColumnRange(const std::string& text_with_range);

        TextSpan& operator+=(const std::string& other);
        TextSpan& operator+=(const ColumnSpan& other);
        TextSpan& operator+=(const TextSpan& other);
};

class Range {
    friend std::ostream& operator<<(std::ostream& os, const Range& range) {
        os << "Range("
        << "start: {" << range.start.line << ", " << range.start.column << "}, "
        << "end: {" << range.end.line << ", " << range.end.column << "})";
        return os;
    }

    void unionStartIfInit(const TextLocation& other_start);
    void unionEndIfInit(const TextLocation& other_end);

    public:
        TextLocation start;
        TextLocation end;
        Range();
        Range(int start, int end);
        Range(int line_start, int column_start, int line_end, int column_end);
        Range(json range_json);
        Range(TextLocation start, TextLocation end);
        Range(const char* JSON_range);

        bool isInRange(int line, int column) const;
        bool isInRange(const TextLocation& location) const;
        json getLSPRangeAsJSON() const;
        int getLineStart() const { return start.line; };
        bool jsonRangeIsValid(json range_json) const;

        Range& operator+=(const Range& other);
};

class RangeEntry {
    public: 
        Range range;
        std::string value;
        
        friend std::ostream& operator<<(std::ostream& os, const RangeEntry& entry) {
            os << "RangeEntry("
            << "value: " << entry.value << ", "
            << "range: " << entry.range << ")";

            return os;
        }

        /**
         * \brief Creates a RangeEntry from a string containing text and a column range.
         * \param line__number The nuber of the line where this text range is located
         * \param text_with_range A string in the format "<text>:<column_start>:<column_end>".
         * \return A RangeEntry whose range spans from column_start to column_end and whose
         *         value contains the text portion preceding the first ':' character.
         *
         * \note The input format is assumed to be valid. If parsing fails or the expected
         *       delimiters are not found, m_error_unhandled() is invoked.
         */
        static RangeEntry fromTextWithColumnRange(int line_number, const std::string& text_with_range);

        RangeEntry(std::string value, int line_start, int column_start, int line_end, int column_end);
        RangeEntry(std::string value, Range range);
        RangeEntry(std::string value);
        RangeEntry(int line_number, TextSpan text_span);

        RangeEntry& operator+=(const std::string& other);
        RangeEntry& operator+=(const Range& other);
        RangeEntry& operator+=(const RangeEntry& other);
};