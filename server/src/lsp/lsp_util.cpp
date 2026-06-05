#include <iostream>

#include "lsp_util.h"
#include "cpp_util.h"

extern "C" {
    #include "util.h"
};

TextLocation::TextLocation() : 
line(-1),
column(-1)
{}

TextLocation::TextLocation(int line, int column) : 
line(line),
column(column)
{}


TextLocation::TextLocation(json lsp_location) {
    line   = lsp_location.contains("line")      ? lsp_location["line"].get<int>()      : -1;
    column = lsp_location.contains("character") ? lsp_location["character"].get<int>() : -1;
}

json TextLocation::toLSPJSON() const {
    json ret = {
        {"line", line},
        {"character", column}
    };

    return ret;
}

Range::Range() :
start(),
end() 
{}

bool Range::jsonRangeIsValid(json range_json) const {
    bool start_valid = range_json.contains("start") && range_json["start"].contains("line") && range_json["start"].contains("character");
    bool end_valid = range_json.contains("end") && range_json["end"].contains("line") && range_json["end"].contains("character");

    bool values_are_integers = start_valid && range_json["start"]["line"].is_number_integer() && range_json["start"]["character"].is_number_integer() &&
                               end_valid   && range_json["end"]["line"].is_number_integer()   && range_json["end"]["character"].is_number_integer();

    return start_valid && end_valid && values_are_integers;
}

Range::Range(json range_json) : 
start(), 
end() 
{
    if(jsonRangeIsValid(range_json)) {
        start = TextLocation(range_json["start"]["line"].get<int>(), range_json["start"]["character"].get<int>());
        end   = TextLocation(range_json["end"]["line"].get<int>(),   range_json["end"]["character"].get<int>());
    }
}

Range::Range(int column_start, int column_end) : Range(-1, column_start, -1, column_end)
{}

Range::Range(int line_start, int column_start, int line_end, int column_end) :
start(line_start, column_start),
end(line_end, column_end)
{}

Range::Range(TextLocation start, TextLocation end) :
start(start),
end(end)
{}

/**
 * \brief Used to initialize from the range attribute provided by c_parser.c 
 * \warning This function is highly specific and can only be used for JSON 
 *          strings with the minimum set of chars,and no additional whitespaces 
 *          or formatting... The string must match the pattern: 
 *          "`line_start`:`column_start`:`line_end`:`column_end`"
 */
Range::Range(const char* col_sep_range) {
    int elements_to_decode = 4;
    int vals[elements_to_decode] = {0};
    
    parseColumnSeparatedInts(col_sep_range, vals, elements_to_decode);

    start = TextLocation(vals[0], vals[1]);
    end   = TextLocation(vals[2], vals[3]);
}

RangeEntry RangeEntry::fromTextWithColumnRange(int line_number, const std::string& text_with_range)
{
    const auto p1 = text_with_range.rfind(':');
    if (p1 == std::string_view::npos) m_error_unhandled();

    const auto p2 = text_with_range.rfind(':', p1 - 1);
    if (p2 == std::string_view::npos) m_error_unhandled();

    int column_start;
    int column_end;
    
    if (!parseInt(text_with_range.substr(p2 + 1, p1 - p2 - 1), column_start)) m_error_unhandled();
    if (!parseInt(text_with_range.substr(p1 + 1), column_end))                m_error_unhandled();

    return RangeEntry{Range(line_number, column_start, line_number, column_end), text_with_range.substr(0, p2)};
}

bool Range::isInRange(int line, int column) const {
    return start.line == end.line ?
        start.column <= column && column <= end.column :

        start.line < line  && line < end.line        ||
        line == start.line && column >= start.column ||
        line == end.line   && column <= end.column;
}


bool Range::isInRange(const TextLocation& location) const {
    return isInRange(location.line, location.column);
}

json Range::getLSPRangeAsJSON() const {
    json out = {
        "range", {
            {"start", start.toLSPJSON()},
            {"end",   end.toLSPJSON()}
        }
    };

    return out;
}
