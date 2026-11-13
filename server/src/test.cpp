#include <iostream>
#include "tokenizer.h"
#include "parser.h"

int main(int argc, char **argw)
{
    std::string input = std::string("");
    if (argc > 1)
        input = std::string(argw[1]);

    std::cout << "INPUT: >>" << input << "<<\n";

    TokenizerResult tks = tokenize(input);

    AST ast;

    parse(input, tks, ast);

    ast.printTree();
}