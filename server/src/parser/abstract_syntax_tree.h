#pragma once
#include <iostream>
#include <memory>

#include "syntax_node.h"

class AbstractSyntaxTree
{
private:
    std::unique_ptr<SyntaxNode> root;

    void printNode(const SyntaxNode *node, std::ostream &out, const std::string &prefix, bool isLast) const;

public:
    AbstractSyntaxTree() : root(new SyntaxNode(-1, SemanticTokenModifierHandler(), nullptr)) {}

    template <typename T>
    T *getRoot() { return dynamic_cast<T *>(root.get()); }

    void printTree(std::ostream &out = std::cout) const;
};

using AST = AbstractSyntaxTree;

class ASTContext
{
    AST &ast;
    SyntaxNode *parent;

public:
    ASTContext(AST &ast) : ast(ast), parent(ast.getRoot<SyntaxNode>()) {};

    template <typename T>
    T *getRoot() { return ast.getRoot<T>(); }
    SyntaxNode *currentParent() { return parent; }
    SyntaxNode *changeLevel(int change_level);
    SyntaxNode *levelUp() { return changeLevel(-1); }
    SyntaxNode *levelDown() { return changeLevel(1); }
};
