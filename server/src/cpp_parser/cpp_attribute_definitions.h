#pragma once

#include <string>
#include <array>
#include <functional>
#include <variant>
#include <unordered_map>
#include <ostream>
#include <utility>

#include "text_util.h"

enum class AttributeId {
    invalid = -1,

    ////////////////////////////////////////////////////////////////////////////
    /// START ATTRIBUTES ///////////////////////////////////////////////////////

    req_id,
    alias,
    title,
    text,
    description,
    description_body,
    author,
    date,
    type,
    tags,
    link,
    ignore,

    /// END ATTRIBUTES /////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    count
};

using AttributeValueVariant = std::variant<
    int,
    Range,
    RangeEntry,
    std::string,
    TextSpan,
    ColumnSpan
>;

std::ostream& operator<<(std::ostream& os, const AttributeValueVariant& value);

class NodeAttribute {
public:
    AttributeId id;
    bool valid = false;

    NodeAttribute(AttributeId id, AttributeValueVariant value);

    NodeAttribute* asPointer() { return this; }
    NodeAttribute& asReference() { return *this; }

    NodeAttribute& operator+=(NodeAttribute& other) {
        if(this->id == other.id)
            *this += other.value();
        else
            std::cerr << "Error when adding attributes of id " << (int)(this->id) << " with " << (int)(other.id) << std::endl;
        return *this;
    }

    NodeAttribute& operator+=(const NodeAttribute& other) {
        if(this->id == other.id)
            *this += other.value();
        else
            std::cerr << "Error when adding attributes of id " << (int)(this->id) << " with " << (int)(other.id) << std::endl;
        return *this;
    }

    // Used when adding another AttributeValueVariant.
    NodeAttribute& operator+=(const AttributeValueVariant& other) {
        return addAssign(other);
    }

    // Used when adding a concrete value, e.g. std::string or RangeEntry.
    template <typename T>
    NodeAttribute& operator+=(T&& other) {
        return addAssign(AttributeValueVariant{std::forward<T>(other)});
    }

    template <typename T>
    const T& value() const { return std::get<T>(_value); }
    const AttributeValueVariant& value() const { return _value; }

private:
    AttributeValueVariant _value;

    NodeAttribute& addAssign(const AttributeValueVariant& other);
};
