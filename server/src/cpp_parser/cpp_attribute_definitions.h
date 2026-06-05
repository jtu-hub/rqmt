#pragma once

#include <string>
#include <array>
#include <functional>
#include <unordered_map>

enum class AttributeId {
    invalid = -1,
    ////////////////////////////////////////////////////////////////////////////
    /// START ATTRIBUTES ///////////////////////////////////////////////////////    
    req_id,
    req_id_range,
    alias,
    alias_range,
    title,
    text,
    description,
    author,
    date,
    link,
    link_range,
    ignore,
    /// END ATTRIBUTES /////////////////////////////////////////////////////////    
    ////////////////////////////////////////////////////////////////////////////
    count
};

class NodeAttribute {
    public:
        AttributeId id;
        std::string value;
        bool valid = false;
        
        NodeAttribute(AttributeId id, std::string value) : id(id), value(std::move(value)) {};
        NodeAttribute* asPointer() { return this; };
        NodeAttribute& asReference() { return *this; };

        NodeAttribute& operator+=(const std::string& add_val) {
            value += add_val;
            return *this;
        }
};

// using AttributeValidator = std::function<bool(const NodeAttribute&)>;
// using RequestMethod = std::string;

// const std::unordered_map<AttributeId, AttributeValidator>& getAttributeValidatorRegistry();