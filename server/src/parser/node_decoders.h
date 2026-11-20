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

class Entity : public SyntaxNode, public NodeDecoder
{
public:
    Entity(Token token, SyntaxNode *parent = nullptr) : SyntaxNode(token, "keyword", {"entity"}, parent) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;

    /*template <typename T, typename... Args>
    T *addChild(Args &&...args)
    {
        T *child = SyntaxNode::addChild<T>(
            std::forward<Args>(args)...);

        if constexpr (std::is_same_v<T, RichText>)
        {
            // Entity-specific RichText handling.
            // For example: register the description node.
        }

        return child;
    }*/
};

class Alias : public SyntaxNode, public NodeDecoder
{
    std::string_view parent_keyword;

public:
    Alias(std::string_view parent_keyword) : SyntaxNode(-1, SemanticTokenModifierHandler(), parent),
                                             parent_keyword(parent_keyword) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;
};


class Title : public SyntaxNode, public NodeDecoder
{
    int number_of_title_tokens;
public:
    Title(int number_of_title_tokens) : SyntaxNode (-1, SemanticTokenModifierHandler(), parent),
                                                     number_of_title_tokens(number_of_title_tokens) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;
};

class EntityId : public SyntaxNode, public NodeDecoder
{
    std::string_view parent_keyword;

public:
    EntityId(std::string_view parent_keyword) : SyntaxNode(-1, SemanticTokenModifierHandler(), parent),
                                                parent_keyword(parent_keyword) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;
};

class PathDecoder : public SyntaxNode, public NodeDecoder
{
    SemanticTokenModifierHandler modifiers;

public:
    PathDecoder(Token token, SyntaxNode *parent = nullptr, SemanticTokenModifierHandler modifiers = {{"filepath"}}) : SyntaxNode(token, "reference", ModifierList{}, parent),
                                                                                                                      modifiers(modifiers) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;
};

class Attribute : public SyntaxNode, public NodeDecoder
{
public:
    Attribute(Token token, SyntaxNode *parent = nullptr) : SyntaxNode(token, "keyword", {"attribute"}, parent) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;
};

class TagList : public SyntaxNode, public NodeDecoder
{
public:
    TagList() : SyntaxNode (-1, SemanticTokenModifierHandler(), parent) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;
};

class RichText : public SyntaxNode, public NodeDecoder
{
    SemanticTokenModifierHandler modifiers;
public:
    RichText(Token token, SyntaxNode *parent = nullptr, SemanticTokenModifierHandler modifiers = {{"documentation"}}) : SyntaxNode(token, "text", modifiers, parent),
                                                                                                                        modifiers(modifiers) {}
    virtual void decode(ParserContext &ctx, ASTContext &ast_ctx) override;
};

