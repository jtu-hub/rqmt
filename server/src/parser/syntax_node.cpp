#include "syntax_node.h"

#include <algorithm>
#include <utility>

SyntaxNode::SyntaxNode(SemanticTokenType type, SemanticTokenModifierHandler modifiers, SyntaxNode *parent) : definition(type, modifiers),
                                                                                                             children(),
                                                                                                             parent(parent),
                                                                                                             range{} {}

SyntaxNode::SyntaxNode() : definition(-1, SemanticTokenModifierHandler()),
                           children(),
                           parent(nullptr),
                           range{} {}

SyntaxNode::SyntaxNode(Token token, std::string_view type, std::initializer_list<std::string_view> modifiers, SyntaxNode *parent) : definition(type, modifiers),
                                                                                                                                    children(),
                                                                                                                                    parent(parent),
                                                                                                                                    range{token.offset, token.length} {}

SyntaxNode::SyntaxNode(Token token, SemanticTokenDefinition definition, SyntaxNode *parent) : definition(definition),
                                                                                              children(),
                                                                                              parent(parent),
                                                                                              range{token.offset, token.length} {}

SyntaxNode::~SyntaxNode() = default;

// Adds an existing node and transfers ownership to this node.
SyntaxNode *SyntaxNode::addChild(SyntaxNode *child)
{
    if (child == nullptr || child == this || child->parent != nullptr)
        return nullptr;

    // Prevent cycles by checking whether child is an ancestor of this node.
    for (SyntaxNode *ancestor = parent;
         ancestor != nullptr;
         ancestor = ancestor->parent)
    {
        if (ancestor == child)
            return nullptr;
    }

    child->parent = this;

    children.emplace_back(child);

    return child;
}

// Removes a child by pointer and transfers ownership to the caller.
std::unique_ptr<SyntaxNode> SyntaxNode::removeChild(SyntaxNode *child)
{
    auto it = std::find_if(
        children.begin(),
        children.end(),
        [child](const std::unique_ptr<SyntaxNode> &entry)
        {
            return entry.get() == child;
        });

    if (it == children.end())
        return nullptr;

    std::unique_ptr<SyntaxNode> removed = std::move(*it);

    children.erase(it);

    removed->parent = nullptr;

    return removed;
}

// Removes a child by index and transfers ownership to the caller.
std::unique_ptr<SyntaxNode> SyntaxNode::removeChild(std::size_t index)
{
    if (index >= children.size())
        return nullptr;

    std::unique_ptr<SyntaxNode> removed = std::move(children[index]);

    children.erase(children.begin() + index);

    removed->parent = nullptr;

    return removed;
}

// Removes and destroys a child.
void SyntaxNode::deleteChild(SyntaxNode *child)
{
    removeChild(child);
}

// Removes and destroys a child by index.
void SyntaxNode::deleteChild(std::size_t index)
{
    removeChild(index);
}

SyntaxNode &SyntaxNode::operator+=(const Token &token)
{
    range.length = token.offset + token.length - range.offset;
    return *this;
}

bool SyntaxNode::hasSemanticTokenDefinition() const
{
    const SemanticTokenType &t = definition.getType();
    return 0 <= t && t < semantic_token_types.size();
}