#pragma once

#include "tokenizer.h"
#include "abstract_syntax_tree.h"

void parse(const std::string &input, TokenizerResult res, AST &ast);