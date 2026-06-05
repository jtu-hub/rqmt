#pragma once

extern "C" {
    #include "c_tokenizer.h"
}

#include "cpp_syntax_definitions.h"
#include "cpp_diagnostic_definitions.h"
#include "cpp_parser_util.h"

AbstractSyntaxTree parseLine(tokens_t tokens, int* indent_level, int* indent_width, token_type_t* indent_type);