#pragma once

#include "parser_util.h"
#include "tokenizer.h"

void parse(const std::string &input, TokenizerResult res, AST &ast);