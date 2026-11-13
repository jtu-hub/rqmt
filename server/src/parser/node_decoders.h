#pragma once

#include "syntax_node.h"
#include "abstract_syntax_tree.h"
#include "parser_context.h"

class NodeDecoder
{
public:
    virtual ~NodeDecoder();
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) = 0;
};

/*
class Entity : public SyntaxNode
{
};

class Attribute : public SyntaxNode
{
};

class TagList : public SyntaxNode
{
};*/

class RichText : public SyntaxNode, public NodeDecoder
{
public:
    RichText(Token token, SyntaxNode *parent = nullptr) : SyntaxNode(token, "comment", {"documentation"}, parent) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;
};