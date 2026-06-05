#include <stdbool.h>

#include "c_token_definitions.h"
#include "util.h"

//Note: tokens which's isStartChar/isEndChar are set ton NULL are virtual ones, 
//      that are used by the tokenizer itself, e.g. the new_line is set after 
//      recognizing the intermediate tokens n/rreturn, to handle cases like '\n' 
//      is new line, as is '\r' and '\r\n'!

// UNKNOWN ---------------------------------------------------------------------
#define k_unknown {                     \
    .isStartChar = NULL,                \
    .isEndChar   = NULL,                \
    .length = -1,                       \
    .include_end_char = false,          \
    .type = k_token_type_none,          \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

// NEW LINE --------------------------------------------------------------------
// CARRIAGE RETURN - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define k_rreturn {                     \
    .isStartChar = isCReturnToken,      \
    .isEndChar   = isCReturnToken,      \
    .length = 1,                        \
    .include_end_char = true,           \
    .type = k_token_type_rreturn,       \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isCReturnToken(char c) {
    return c == '\r';
}

// RETURN - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#define k_nreturn {                     \
    .isStartChar = isNReturnToken,      \
    .isEndChar   = isNReturnToken,      \
    .length = 1,                        \
    .include_end_char = true,           \
    .type = k_token_type_nreturn,       \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isNReturnToken(char c) {
    return c == '\n';
}

// NEW LINE TOKEN - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define k_new_line {                    \
    .isStartChar = NULL,                \
    .isEndChar   = NULL,                \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_new_line,      \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isNewLine(char c) {
    return isCReturnToken(c) || isNReturnToken(c);
}

// NUMBERS ---------------------------------------------------------------------
#define k_number {                      \
    .isStartChar = isNumberTokenStart,  \
    .isEndChar   = isNumberTokenEnd,    \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_number,        \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isNumberTokenStart(char c) {
    return c >= '0' && c <= '9';
}

bool isNumberTokenEnd(char c) {
    return !isNumberTokenStart(c);
}

// INDENTATION -----------------------------------------------------------------
// SPACE INDENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#define k_sindent_after_nl {            \
    .isStartChar = isSIndentTokenStart, \
    .isEndChar   = isSIndentTokenEnd,   \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_sindent,       \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_new_line \
}

#define k_sindent_after_ti {            \
    .isStartChar = isSIndentTokenStart, \
    .isEndChar   = isSIndentTokenEnd,   \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_sindent,       \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_tindent  \
}

bool isSIndentTokenStart(char c) {
    return c == ' ';
}

bool isSIndentTokenEnd(char c) {
    return !isSIndentTokenStart(c);
}

// TAB INDENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#define k_tindent_after_nl {            \
    .isStartChar = isTIndentTokenStart, \
    .isEndChar   = isTIndentTokenEnd,   \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_tindent,       \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_new_line \
}

#define k_tindent_after_si {            \
    .isStartChar = isTIndentTokenStart, \
    .isEndChar   = isTIndentTokenEnd,   \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_tindent,       \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_sindent  \
}

bool isTIndentTokenStart(char c) {
    return c == '\t';
}

bool isTIndentTokenEnd(char c) {
    return !isTIndentTokenStart(c);
}

// PUNCTUATION -----------------------------------------------------------------
// BRACES - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#define k_brace_o {                     \
    .isStartChar = isBraceOToken,       \
    .isEndChar   = isBraceOToken,       \
    .length = 1,                        \
    .include_end_char = true,           \
    .type = k_token_type_brace_o,       \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isBraceOToken(char c) {
    return c == '{';
}

#define k_brace_c {                     \
    .isStartChar = isBraceCToken,       \
    .isEndChar   = isBraceCToken,       \
    .length = 1,                        \
    .include_end_char = true,           \
    .type = k_token_type_brace_c,       \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isBraceCToken(char c) {
    return c == '}';
}

// HYPEHN - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#define k_hyphen {                      \
    .isStartChar = isHyphenToken,       \
    .isEndChar   = isHyphenToken,       \
    .length = 1,                        \
    .include_end_char = true,           \
    .type = k_token_type_hyphen,        \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isHyphenToken(char c) {
    return c == '-';
}

// DOT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#define k_dot {                         \
    .isStartChar = isDotToken,          \
    .isEndChar   = isDotToken,          \
    .length = 1,                        \
    .include_end_char = true,           \
    .type = k_token_type_dot,           \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isDotToken(char c) {
    return c == '.';
}

// COLUMN - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#define k_column {                      \
    .isStartChar = isColumnToken,       \
    .isEndChar   = isColumnToken,       \
    .length = 1,                        \
    .include_end_char = true,           \
    .type = k_token_type_column,        \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isColumnToken(char c) {
    return c == ':';
}

// ESCAPE CHAR - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define k_escape_char {                 \
    .isStartChar = isEscCharToken,      \
    .isEndChar   = isEscCharToken,      \
    .length = 1,                        \
    .include_end_char = true,           \
    .type = k_token_type_escape_char,   \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isEscCharToken(char c) {
    return c == '\\';
}

bool isPunctuation(char c) {
    return isColumnToken(c) || isHyphenToken(c) || isEscCharToken(c) || 
           isBraceOToken(c) || isBraceCToken(c) || isDotToken(c);
}

// DELIMITERS ------------------------------------------------------------------
// MARKDOWN ELEMENT DELIMITERS - - - - - - - - - - - - - - - - - - - - - - - - -
#define k_fstring {                     \
    .isStartChar = isFstringTokenStart, \
    .isEndChar   = isFstringTokenEnd,   \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_backtick_delim,\
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

#define k_math_expr {                   \
    .isStartChar = isMathExprTokenStart,\
    .isEndChar   = isMathExprTokenEnd,  \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_math_delim,    \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

bool isFstringTokenStart(char c) {
    return c == '`';
}

bool isMathExprTokenStart(char c) {
    return c == '$';
}

bool isFstringTokenEnd(char c) {
    return c != '`';
}

bool isMathExprTokenEnd(char c) {
    return c != '$';
}

// REFERENCE DELIMITERS - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#define k_brace_ref_o {                        \
    .isStartChar = isBraceOToken,              \
    .isEndChar   = isBraceOToken,              \
    .length = 1,                               \
    .include_end_char = true,                  \
    .type = k_token_type_brace_ref_o,          \
    .depends_non = k_token_type_escape_char,   \
    .depends_on = k_token_type_none            \
}

#define k_brace_ref_c {                        \
    .isStartChar = isBraceCToken,              \
    .isEndChar   = isBraceCToken,              \
    .length = 1,                               \
    .include_end_char = true,                  \
    .type = k_token_type_brace_ref_c,          \
    .depends_non = k_token_type_escape_char,   \
    .depends_on = k_token_type_none            \
}

bool isDelimiter(char c) {
    return isFstringTokenStart(c) || isMathExprTokenStart(c);
}

// POTENTIAL KEYWORD -----------------------------------------------------------
// Note: a potential keyword is only found at the beginning of a new line, i.e.
//       after a new line or a indent

#define k_keyword_after_nl {            \
    .isStartChar = isKeywordTokenStart, \
    .isEndChar   = isKeywordTokenEnd,   \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_pot_keyword,   \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_new_line \
}

#define k_keyword_after_si {            \
    .isStartChar = isKeywordTokenStart, \
    .isEndChar   = isKeywordTokenEnd,   \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_pot_keyword,   \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_sindent  \
}

#define k_keyword_after_ti {            \
    .isStartChar = isKeywordTokenStart, \
    .isEndChar   = isKeywordTokenEnd,   \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_pot_keyword,   \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_tindent  \
}

bool isKeywordTokenStart(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool isKeywordTokenEnd(char c) {
    return !isKeywordTokenStart(c);
}

// REFERENCE NAME --------------------------------------------------------------
#define k_ref_name_after_dot {          \
    .isStartChar = isNameTokenStart,    \
    .isEndChar   = isNameTokenEnd,      \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_pot_ref_name,  \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_dot      \
}

#define k_ref_name_after_brace_ref_o {     \
    .isStartChar = isNameTokenStart,       \
    .isEndChar   = isNameTokenEnd,         \
    .length = k_variable_token_length,     \
    .include_end_char = false,             \
    .type = k_token_type_pot_ref_name,     \
    .depends_non = k_token_type_none,      \
    .depends_on = k_token_type_brace_ref_o \
}

#define k_ref_name_after_number {           \
    .isStartChar = isNameTokenStart,        \
    .isEndChar   = isNameTokenEnd,          \
    .length = k_variable_token_length,      \
    .include_end_char = false,              \
    .type = k_token_type_pot_ref_name,      \
    .depends_non = k_token_type_none,       \
    .depends_on = k_token_type_pot_ref_num  \
}

#define k_number_after_ref {                \
    .isStartChar = isNumberTokenStart,      \
    .isEndChar   = isNumberTokenEnd,        \
    .length = k_variable_token_length,      \
    .include_end_char = false,              \
    .type = k_token_type_pot_ref_num,       \
    .depends_non = k_token_type_none,       \
    .depends_on = k_token_type_pot_ref_name \
}

#define k_number_after_ref_brace {          \
    .isStartChar = isNumberTokenStart,      \
    .isEndChar   = isNumberTokenEnd,        \
    .length = k_variable_token_length,      \
    .include_end_char = false,              \
    .type = k_token_type_pot_ref_num,       \
    .depends_non = k_token_type_none,       \
    .depends_on = k_token_type_brace_ref_o  \
}

#define k_number_after_ref_dot {            \
    .isStartChar = isNumberTokenStart,      \
    .isEndChar   = isNumberTokenEnd,        \
    .length = k_variable_token_length,      \
    .include_end_char = false,              \
    .type = k_token_type_pot_ref_num,       \
    .depends_non = k_token_type_none,       \
    .depends_on = k_token_type_dot          \
}

bool isNameTokenStart(char c) {
    return c == '_' || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

bool isNameTokenEnd(char c) {
    return !isNameTokenStart(c);
}

// TEXT ------------------------------------------------------------------------
// Note: all other character will be set to token type text or unknown
bool isOtherTokenStart(char c) {
    return isPunctuation(c)     ||
           isNewLine(c)         ||
           isDelimiter(c)       ||
           isNumberTokenStart(c);
}

#define k_text {                        \
    .isStartChar = isTextTokenStart,    \
    .isEndChar   = isTextTokenEnd,      \
    .length = k_variable_token_length,  \
    .include_end_char = false,          \
    .type = k_token_type_text,          \
    .depends_non = k_token_type_none,   \
    .depends_on = k_token_type_none     \
}

#define k_space_nafter_nl {               \
    .isStartChar = isSpaceTokenStart,     \
    .isEndChar   = isSpaceTokenEnd,       \
    .length = k_variable_token_length,    \
    .include_end_char = false,            \
    .type = k_token_type_space,           \
    .depends_non = k_token_type_new_line, \
    .depends_on = k_token_type_none       \
}

bool isSpaceTokenStart(char c) {
    return c == ' ';
}

bool isSpaceTokenEnd(char c) {
    return c != ' ';
}

bool isTextTokenStart(char c) {
    return ' ' < c && c <= '~' && !isOtherTokenStart(c);
}

bool isTextTokenEnd(char c) {
    return (c <= ' ' || '~' < c) || isOtherTokenStart(c);
}

// Note: for concurring potential candidates, the one with higher index prevales
const token_definition_t token_definitions[] = {
    k_rreturn,
    k_nreturn,
    k_space_nafter_nl,
    k_text,
    k_ref_name_after_dot,
    k_ref_name_after_brace_ref_o,
    k_ref_name_after_number,
    k_keyword_after_nl,
    k_keyword_after_si,
    k_keyword_after_ti,
    k_number,
    k_number_after_ref,
    k_number_after_ref_dot,
    k_number_after_ref_brace,
    k_sindent_after_nl,
    k_sindent_after_ti,
    k_tindent_after_nl,
    k_tindent_after_si,
    k_brace_o,
    k_brace_ref_o,
    k_brace_c,
    k_brace_ref_c,
    k_hyphen,
    k_dot,
    k_column,
    k_escape_char,
    k_fstring,
    k_math_expr,
};

const token_definition_t* getTokenDefinitions(int* definition_count) {
    m_check_pointer(definition_count);

    *definition_count = sizeof(token_definitions) / sizeof(token_definitions[0]);

    return token_definitions;
}