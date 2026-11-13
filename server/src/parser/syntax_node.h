#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "semantic_tokens.h"
#include "token_definition.h"
#include "parser_context.h"

class SyntaxNode
{
protected:
    SemanticTokenDefinition definition;
    std::vector<std::unique_ptr<SyntaxNode>> children;
    SyntaxNode *parent;
    OffsetRange range;

public:
    SyntaxNode();
    SyntaxNode(SemanticTokenType type, SemanticTokenModifierHandler modifiers, SyntaxNode *parent = nullptr);
    SyntaxNode(Token token, std::string_view type, std::initializer_list<std::string_view> modifiers = {}, SyntaxNode *parent = nullptr);
    SyntaxNode(Token token, SemanticTokenDefinition definition, SyntaxNode *parent = nullptr);

    virtual ~SyntaxNode();

    // Getters
    const file_offset_t &getLength() const { return range.length; }
    const SemanticTokenDefinition &getDefinition() const { return definition; }
    const std::vector<std::unique_ptr<SyntaxNode>> &getChildren() const { return children; }
    const OffsetRange &getRange() const { return range; }

    // Disable copying to prevent accidental ownership duplication.
    SyntaxNode(const SyntaxNode &) = delete;
    SyntaxNode &operator=(const SyntaxNode &) = delete;

    // Child management
    SyntaxNode *addChild(SyntaxNode *child);
    template <typename T, typename... Args>
    T *addChild(Args &&...args);

    std::unique_ptr<SyntaxNode> removeChild(SyntaxNode *child);
    std::unique_ptr<SyntaxNode> removeChild(std::size_t index);

    void deleteChild(SyntaxNode *child);
    void deleteChild(std::size_t index);

    template <typename T>
    T *getLastChildOfType() const;

    template <typename T>
    T *getLastChildOnlyIfType() const;

    template <typename T>
    bool isLastChildOfType() const;

    SyntaxNode *getLastChild() const
    {
        return children.empty() ? nullptr : children.back().get();
    }

    SyntaxNode *getParent() const
    {
        return parent;
    }

    // Node management
    SyntaxNode &operator+=(const Token &token);
    bool hasSemanticTokenDefinition() const;

    template <typename T>
    T *as();

    template <typename T>
    const T *as() const;
};

// make template implementations visible to compiler
#include "syntax_node.tpp"
