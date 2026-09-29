// cpp_attribute_definitions.cpp
#include <array>
#include <stdexcept>
#include <utility>
#include <iostream>

#include "cpp_attribute_definitions.h"

extern "C" {
    #include "util.h"
}

namespace
{
    using AddAssignFn = void (*)(AttributeValueVariant&, const AttributeValueVariant&);

    template <typename Lhs, typename Rhs>
    void addAssign(AttributeValueVariant& lhs, const AttributeValueVariant& rhs) {
        std::get<Lhs>(lhs) += std::get<Rhs>(rhs);
    }

    constexpr std::size_t AttributeValueVariantSize = std::variant_size_v<AttributeValueVariant>;

    constexpr std::array<std::array<AddAssignFn, AttributeValueVariantSize>, AttributeValueVariantSize> addAssignRegistry = {{
        // LHS: int
        {{
            &addAssign<int, int>,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
        }},

        // LHS: Range
        {{
            nullptr,
            &addAssign<Range, Range>,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
        }},

        // LHS: RangeEntry
        {{
            nullptr,
            &addAssign<RangeEntry, Range>,
            &addAssign<RangeEntry, RangeEntry>,
            &addAssign<RangeEntry, std::string>,
            nullptr,
            nullptr,
        }},

        // LHS: std::string
        {{
            nullptr,
            nullptr,
            nullptr,
            &addAssign<std::string, std::string>,
            nullptr,
            nullptr,
        }},

        // LHS: TextSpan
        {{
            nullptr,
            nullptr,
            nullptr,
            &addAssign<TextSpan, std::string>,
            &addAssign<TextSpan, TextSpan>,
            &addAssign<TextSpan, ColumnSpan>,
        }},

        // LHS: ColumnSpan
        {{
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            &addAssign<ColumnSpan, ColumnSpan>,
        }},
    }};

    std::ostream& operator<<(std::ostream& os, const AttributeValueVariant& value) {
        std::visit(
            [&os](const auto& v) {
                os << v;
            },
            value);

        return os;
    }
}

NodeAttribute::NodeAttribute(AttributeId id, AttributeValueVariant value) : 
    id(id),
    _value(std::move(value))
{}

NodeAttribute& NodeAttribute::addAssign(const AttributeValueVariant& other) {
    const auto lhs_index = _value.index();
    const auto rhs_index = other.index();

    const auto add_assign = addAssignRegistry[lhs_index][rhs_index];

    if (add_assign == nullptr) {
        std::cerr << "Invalid types for operator+=\n";
    }

    add_assign(_value, other);

    return *this;
}
