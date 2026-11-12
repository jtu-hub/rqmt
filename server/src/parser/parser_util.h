#pragma once

#include <vector>
#include <string_view>
#include <stdint.h>

using SemanticTokenModifiers = uint32_t;
using SemanticTokenType = int;

class SemanticTokenModifierHandler
{
    SemanticTokenModifiers modifiers;

public:
    const SemanticTokenModifiers &getModifiers() const { return modifiers; };
    void set(const std::string_view &modifier);
    void unset(const std::string_view &modifier);
    bool has(const std::string_view &modifier);
};

class SemanticTokenDefinition
{

private:
    SemanticTokenType type;
    SemanticTokenModifierHandler modifiers;

public:
    SemanticTokenDefinition(SemanticTokenType type, SemanticTokenModifierHandler modifiers) : type(type),
                                                                                              modifiers(modifiers) {}

    const SemanticTokenType &getType() { return type; }
};

class SyntaxNode
{
private:
    SemanticTokenDefinition definition;
    std::vector<SyntaxNode *> children;
    SyntaxNode *parent;

public:
    SyntaxNode(SemanticTokenType type, SemanticTokenModifierHandler modifiers, SyntaxNode *parent) : definition(type, modifiers),
                                                                                                     children(),
                                                                                                     parent(parent) {}
    template <TokenDefinitions::TokenType T>
    const bool isOfType() const
    {
        return definition.getType() == T;
    }
};

class AbstractSyntaxTree
{
private:
    SyntaxNode root;

public:
    AbstractSyntaxTree() : root(-1, SemanticTokenModifierHandler(), nullptr) {};
    SyntaxNode *getRoot() { return &root; };
};

using AST = AbstractSyntaxTree;
