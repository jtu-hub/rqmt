#pragma once

#include <stdbool.h>

#define k_max_value_str_size 4096
#define k_variable_token_length -1

// Enum to define the different types of tokens
typedef enum TokenType {
    k_token_type_invalid = -1,
    k_token_type_none,
// ADD THE DEFINITION OF THE TOKEN TYPES HERE ----------------------------------
    // New Line
    k_token_type_rreturn,
    k_token_type_nreturn,
    k_token_type_new_line,
    // Source
    k_token_type_source,
    k_token_type_space,
    k_token_type_text,
    k_token_type_pot_keyword,
    k_token_type_pot_ref_name,
    k_token_type_pot_ref_num,
    k_token_type_number,
    k_token_type_sindent,
    k_token_type_tindent,
    // Punctuation
    k_token_type_brace_o,
    k_token_type_brace_ref_o,
    k_token_type_brace_c,
    k_token_type_brace_ref_c,
    k_token_type_hyphen,
    k_token_type_dot,
    k_token_type_column,
    k_token_type_escape_char,
    k_token_type_backtick_delim,
    k_token_type_math_delim,
// -----------------------------------------------------------------------------
    k_token_type_EOF,
    k_token_type_count
} token_type_t;

// Structure containing a token
typedef struct Token {
    token_type_t type;
    char value[k_max_value_str_size];
    int line;
    int column;
    int char_count;
} token_t; 

// Structure to define a token
typedef struct TokenDefinition {
    bool (*isStartChar)(char c);    
    bool (*isEndChar)(char c);
    int length;
    bool include_end_char;
    token_type_t type;
    token_type_t depends_on;
    token_type_t depends_non;
} token_definition_t;

const token_definition_t* getTokenDefinitions(int* definition_count);
