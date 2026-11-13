#pragma once

template <typename T>
T *SyntaxNode::getLastChildOfType() const
{
    for (auto it = children.rbegin(); it != children.rend(); ++it)
    {
        if (auto *child = dynamic_cast<T *>(it->get()))
            return child;
    }

    return nullptr;
}

template <typename T>
T *SyntaxNode::getLastChildOnlyIfType() const
{
    auto it = children.rbegin();

    if (it != children.rend())
    {
        if (auto *child = dynamic_cast<T *>(it->get()))
            return child;
    }

    return nullptr;
}

template <typename T, typename... Args>
T *SyntaxNode::addChild(Args &&...args)
{
    static_assert(
        std::is_base_of_v<SyntaxNode, T>,
        "T must derive from SyntaxNode");

    auto child = std::make_unique<T>(std::forward<Args>(args)...);

    T *result = child.get();
    result->parent = this;

    children.emplace_back(std::move(child));

    return result;
}

template <typename T>
bool SyntaxNode::isLastChildOfType() const
{
    auto it = children.rbegin();

    if (it != children.rend())
    {
        if (auto *child = dynamic_cast<T *>(it->get()))
            return true;
    }

    return false;
}

#include <stdexcept>
#include <typeinfo>

template <typename T>
T *SyntaxNode::as()
{
    auto *result = dynamic_cast<T *>(this);

    if (!result)
        throw std::runtime_error("SyntaxNode cannot be cast to requested type");

    return result;
}

template <typename T>
const T *SyntaxNode::as() const
{
    auto *result = dynamic_cast<const T *>(this);

    if (!result)
        throw std::runtime_error("SyntaxNode cannot be cast to requested type");

    return result;
}