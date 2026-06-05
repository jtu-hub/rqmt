// cpp_attribute_definitions.cpp

#include "cpp_attribute_definitions.h"

extern "C" {
    #include "util.h"
}

//TBD if necessary, might be useful, need to define hash for enum though
// bool alwaysValid(NodeAttribute& attr) { attr.valid = true; }


// const std::unordered_map<AttributeId, AttributeValidator>& getAttributeValidatorRegistry() {
//     static const std::unordered_map<AttributeId, AttributeValidator> registry = {
//         /// REGISTER CALLBACKS /////////////////////////////////////////////////
//         {AttributeId::req_id, alwaysValid},
//         {AttributeId::req_id_range, alwaysValid},
//         {AttributeId::alias, alwaysValid},
//         {AttributeId::alias_range, alwaysValid},
//         {AttributeId::title, alwaysValid},
//         {AttributeId::text, alwaysValid},
//         {AttributeId::description, alwaysValid},
//         {AttributeId::author, alwaysValid},
//         {AttributeId::date, alwaysValid},
//         {AttributeId::link, alwaysValid},
//         {AttributeId::link_range, alwaysValid},
//         {AttributeId::ignore, alwaysValid},  
//         //...
//         ////////////////////////////////////////////////////////////////////////
//     };

//     return registry;
// }