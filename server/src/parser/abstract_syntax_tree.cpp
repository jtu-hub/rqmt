#include "abstract_syntax_tree.h"

#include <string>

void AbstractSyntaxTree::printTree(std::ostream &out) const
{
    if (!root)
    {
        out << "<empty AST>\n";
        return;
    }

    printNode(root.get(), out, "", true);
}

void AbstractSyntaxTree::printNode(const SyntaxNode *node, std::ostream &out, const std::string &prefix, bool isLast) const
{
    if (!node)
        return;

    const auto &range = node->getRange();
    const auto &definition = node->getDefinition();

    out << prefix
        << (isLast ? "O-- " : "|-- ") //"└── " : "├── ")
        << definition.getTypeName()
        << " ["
        << range.offset << ", "
        << range.offset + range.length << ")";

    const auto modifiers = definition.getModifierNames();

    if (!modifiers.empty())
    {
        out << " {";

        for (std::size_t i = 0; i < modifiers.size(); ++i)
        {
            if (i > 0)
                out << ", ";

            out << modifiers[i];
        }

        out << "}";
    }

    out << '\n';

    const auto &children = node->getChildren();

    const std::string childPrefix =
        prefix + (isLast ? "    " : "|   ");

    for (std::size_t i = 0; i < children.size(); ++i)
    {
        printNode(
            children[i].get(),
            out,
            childPrefix,
            i == children.size() - 1);
    }
}

SyntaxNode *ASTContext::changeLevel(int change_level)
{
    std::cout << "CHANGE LEVEL: " << change_level << std::endl;
    if (parent == nullptr)
        nullptr;

    while (change_level > 0)
    {
        SyntaxNode *child = parent->getLastChild();

        if (child == nullptr)
            parent;

        parent = child;
        --change_level;
    }

    while (change_level < 0)
    {
        SyntaxNode *ancestor = parent->getParent();

        if (ancestor == nullptr)
            parent;

        parent = ancestor;
        ++change_level;
    }

    return parent;
}