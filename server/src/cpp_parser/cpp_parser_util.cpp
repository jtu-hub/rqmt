#include <algorithm>
#include <iostream>

extern "C" {
    #include "util.h"
    #include "list.h"
}

#include "cpp_parser_util.h"
#include "cpp_util.h"

void SyntaxNode::addChild(SyntaxNode& child) {
    children.push_back(child);
}

void SyntaxNode::addAttribute(const AttributeId& attr_id, const AttributeValueVariant& value) {
    //TBD change param to const NodeAttribute& as to addChild 
    auto& attrs = attributes[attr_id];
    
    attrs.emplace_back(attr_id, value);

    current_attribute = {attr_id, attrs.size() - 1};
}

void SyntaxNode::addDiagnostic(const DiagnosticSeverity& severity, const DiagnosticId& id, std::string source, int line_start, int line_end, int column_start, int column_end) {
    //TODO change param to const Diagnostic& as to addChild 
    diagnostics.emplace_back(Diagnostic{id, severity, source, line_start, line_end, column_start, column_end});
}

void SyntaxNode::removeDiagnosticOfType(const DiagnosticId& id) {
    diagnostics.erase(
        std::remove_if(
            diagnostics.begin(),
            diagnostics.end(),
            [&id](const Diagnostic& d) { return d.id == id; }
        ),
        diagnostics.end()
    );
}

void SyntaxNode::rremoveDiagnosticOfType(const DiagnosticId& id) {
    removeDiagnosticOfType(id);

    for(SyntaxNode c : children) {
        c.rremoveDiagnosticOfType(id);
    }
}

SyntaxNode& SyntaxNode::newChild() {
    SyntaxNode new_node;

    addChild(new_node);

    return children.back();
}

SyntaxNode& SyntaxNode::newChildFromToken(const token_t& token, const SemanticTokenDefinition& token_definition) {
    SyntaxNode& child = newChild();

    child = token;
    child = token_definition;

    return child;
}

void SyntaxNode::recomputeLength(int token_column, int token_lenght) {
    length = token_column - location.column + token_lenght;
}

void SyntaxNode::addToLastAttributeOfTypeOrCreate(const AttributeId& attr_id, const AttributeValueVariant& attr_value) {
    std::unordered_map<AttributeId, std::vector<NodeAttribute>>::iterator finder = attributes.find(attr_id);

    if(finder == attributes.end() || finder->second.size() == 0) {
        addAttribute(attr_id, attr_value);
    } else {
        finder->second.back() += attr_value;
    }
}

void SyntaxNode::renameCurrentAttribute(AttributeId old_id, AttributeId new_id) {
    AttributeId id = current_attribute.first;
    size_t     idx = current_attribute.second;

    if(id != old_id) m_error_unhandled();

    NodeAttribute attr = std::move(attributes[old_id][idx]);

    attr.id = new_id;

    attributes[old_id].erase(attributes[old_id].begin() + idx);

    attributes[new_id].push_back(std::move(attr));

    current_attribute = {new_id, attributes[new_id].size() - 1};
}

NodeAttribute& SyntaxNode::lastAttribute() {    
    if(current_attribute == std::pair<AttributeId, size_t>{AttributeId::invalid, -1}) m_error_unhandled();
    
    AttributeId id = current_attribute.first;
    size_t     idx = current_attribute.second;
    
    return attributes[id][idx];
}

NodeAttribute& SyntaxNode::lastAttributeOfType(const AttributeId& attr_id) {
    std::unordered_map<AttributeId, std::vector<NodeAttribute>>::iterator finder = attributes.find(attr_id);

    if(finder == attributes.end()) {
        m_error_unhandled();
    }

    return finder->second[finder->second.size() - 1];
}

NodeAttribute& SyntaxNode::firstAttributeOfType(AttributeId attr_id) {
    std::unordered_map<AttributeId, std::vector<NodeAttribute>>::iterator finder = attributes.find(attr_id);

    if(finder == attributes.end()) {
        m_error_unhandled();
    }
    
    return finder->second.begin()->asReference();
}

SyntaxNode& SyntaxNode::lastChild() {
    if(children.size() == 0) m_error_unhandled();

    return children.back();
}

SyntaxNode& SyntaxNode::lastChildOfType(SemanticTokenId st_id) {
    auto it = std::find_if(
        children.rbegin(),
        children.rend(),
        [&st_id](const SyntaxNode& sn) {
            return sn == st_id;
        }
    );

    if(it == children.rend()) m_error_unhandled();

    return *it;
}

SyntaxNode& AbstractSyntaxTree::lastChildAtLevel(int level, int* found_at_level) {
    if(found_at_level != NULL) 
        *found_at_level = level; 

    if(level <= 0) {
        return root;
    } else {
        SyntaxNode* child = root.asPointer(); 

        while (level > 0 && child->childCount() > 0)
        {
            child = child->lastChild().asPointer();
            level--;
        }

        if(found_at_level != NULL) 
            (*found_at_level) -= level;

        if(level > 0) return root;

        return child->asReference();
    }
}

void SyntaxNode::recursiveSetLine(int line_number) { 
    location.line = line_number; 

    for(SyntaxNode& child : children) {
        child.recursiveSetLine(line_number);
    }
};

void SyntaxNode::copyTokenDefinition(const SyntaxNode& from) {
    definition.id        = from.getDefinition().id;
    definition.name      = from.getDefinition().name;
    definition.modifiers = from.getDefinition().modifiers;
    definition.token     = from.getDefinition().token;
}

void SyntaxNode::copyChildrenTo(SyntaxNode& target) const {
    target.children.insert(target.children.end(), children.begin(), children.end());
}

void SyntaxNode::copyDiagnosticsTo(SyntaxNode& target) const {
    target.diagnostics.insert(target.diagnostics.end(), diagnostics.begin(), diagnostics.end());
}

const std::vector<NodeAttribute>& SyntaxNode::getAttributesById(AttributeId id) const { 
    static const std::vector<NodeAttribute> empty;

    auto finder = attributes.find(id);
    
    return (finder != attributes.end()) ? finder->second : empty;
}

void SyntaxNode::debugPrintSyntaxNode(int level) {
    for(int i = 0; i < level; i++) std::cerr << "    ";

    std::cerr << "+ " << definition.name << ": " << static_cast<int>(definition.id) << "\n";
    for(int i = 0; i < level; i++) std::cerr << "    ";

    std::cerr << "    L: " << location.line << ":" << location.column << "\n";  

    for(auto& [a_id, vec_a] : attributes) {
        for(auto& a : vec_a){
            for(int i = 0; i < level; i++) std::cerr << "    ";

            std::cerr << "    > " << static_cast<int>(a.id) << ": " << a.value() << "\n";
        }
    }

    for(auto& d : diagnostics)
    {       
        for(int i = 0; i < level; i++) std::cerr << "    ";

        std::cerr << "    !> " << static_cast<int>(d.id) << " L: " << d.line_start << ":" << d.column_start << ":" << d.line_end << ":" << d.column_end <<"\n";
    }
}

void SyntaxNode::rdebugPrintSyntaxNode(int level) {
    debugPrintSyntaxNode(level);

    for(auto& child : children) {
        child.rdebugPrintSyntaxNode(level + 1);
    }
}

void SyntaxNode::overrideDefinition(const SemanticTokenDefinition& new_definition) {
    *this = new_definition;
}

void SyntaxNode::roverrideDefinition(const SemanticTokenDefinition& new_definition) {
    overrideDefinition(new_definition);

    for(auto& child : children) {
        child.roverrideDefinition(new_definition);
    }

}

bool mergeAttributeValues(AttributeId id) {
    //helper function to decide whether attributes shall be merged or 
    //overwritten in SyntaxNode::copyAttributesTo TODO: decide where to place this function

    switch (id)
    {
    case AttributeId::description_body:
    case AttributeId::author:
        return true;
    default:
        return false;
    }
}

void SyntaxNode::copyAttributesTo(SyntaxNode& target, const int& line_number, const AttributeId& text_override_id) const {
    for (auto& [id, source_attributes] : attributes)
    {
        AttributeId target_id = id == AttributeId::text ? text_override_id : id;

        auto& target_attributes = target.attributes[target_id];

        for (auto& source_attr : source_attributes)
        {
            if (target_attributes.empty())
                target_attributes.emplace_back(target_id, source_attr.value());
            else if (mergeAttributeValues(target_id))
                target_attributes.front() += source_attr.value();
            else {
                int column_start = this->location.column;
                int column_end   = column_start + this->length;

                if(column_start < 0 && this->childCount() > 0) {
                    column_start = this->getChildren().begin()->getColumn();
                    column_end   = column_start + this->getChildren().begin()->getLength();
                }

                if(id == AttributeId::text) {
                    target.addDiagnostic(DiagnosticSeverity::error, 
                        DiagnosticId::multiline_not_allowed, 
                        m_file_src, line_number, line_number, 
                        column_start, column_end);

                } else {
                    target.addDiagnostic(DiagnosticSeverity::error, 
                        DiagnosticId::multiple_values_for_attr, 
                        m_file_src, line_number, line_number, 
                        column_start, column_end);
                }
            }
        }
    }
}

void SyntaxNode::moveAttributesTo(SyntaxNode& target, const int& line_number) {
    for (auto& [id, source_attributes] : attributes)
    {
        auto& target_attributes = target.attributes[id];

        for (auto& source_attr : source_attributes)
        {
            if (target_attributes.empty())
                target_attributes.push_back(std::move(source_attr));
            else if (mergeAttributeValues(id))
                target_attributes.front() += source_attr;
            else {
                int column_start = this->location.column;
                int column_end   = column_start + this->length;

                if(column_start < 0 && this->childCount() > 0) {
                    column_start = this->getChildren().begin()->getColumn();
                    column_end   = column_start + this->getChildren().begin()->getLength();
                }

                target.addDiagnostic(DiagnosticSeverity::error, 
                                     DiagnosticId::multiple_values_for_attr, 
                                     m_file_src, line_number, line_number, 
                                     column_start, column_end);
            }
        }
    }

    attributes.clear();
}


void SyntaxNode::rgetDiagnosticLSPJSON(json& diagnostics_array) const {
    for(const auto& diagnostic : diagnostics) {
        diagnostics_array.push_back(diagnostic.toLSPJson());
    }

    for(const auto& child : children) {
        child.rgetDiagnosticLSPJSON(diagnostics_array);
    }
}

json SyntaxNode::getDiagnosticLSPJSON() const {
    json diagnostics_array = json::array();

    rgetDiagnosticLSPJSON(diagnostics_array);

    return diagnostics_array;
}

void AbstractSyntaxTree::debugPrintTree() {
    std::cerr << "\n////////////////////////////////////////////////////////////////////////////////\nAST:\nReferences:\n";
    for(auto& refs : references) {
        for(auto ref : refs.second) {
            std::cerr << "o-o  " << ref.range.getLineStart() << ": " << ref.value << std::endl;
        }
    }
    std::cerr << "Aliases:\n";
    for(auto& als : aliases) {
        std::cerr << "<-o  " << als.first << ": " << als.second.getLSPRangeAsJSON() << std::endl;
    }
    root.rdebugPrintSyntaxNode(0);
}

void AbstractSyntaxTree::addSyntaxNodeLSPData(const SyntaxNode& n, std::vector<int>& data) {
    int delta_line = n.getLine() - previous_line;
    int delta_column = (bool)delta_line ? 
        n.getColumn() : n.getColumn() - previous_column; 

    int token_modifiers = 0, i=0;
    STDModifierArray modifiers = n.getDefinition().modifiers;

    while(modifiers[i] != SemanticTokenModifier::none && i < k_max_number_modifiers) {
        token_modifiers |= 1 << static_cast<int>(modifiers[i]);
        i++;
    }
    
    data.push_back(delta_line);
    data.push_back(delta_column);
    data.push_back(n.getLength());
    data.push_back(static_cast<int>(n.getDefinition().token));
    data.push_back(token_modifiers);
    
    previous_line   = n.getLine();
    previous_column = n.getColumn();  
}

void AbstractSyntaxTree::syntaxNodeToLSPData(const SyntaxNode& node, std::vector<int>& data) {
    if (node.getDefinition().id != SemanticTokenId::root) {
        addSyntaxNodeLSPData(node, data);
    }

    for (const SyntaxNode& child : node.getChildren()) {        
        syntaxNodeToLSPData(child, data);
    }
}

json AbstractSyntaxTree::getSemanticTokenData() {
    std::vector<int> data;
    
    syntaxNodeToLSPData(root, data);

    json res = {{"data", data}};

    previous_line = 0; 
    previous_column = 0;

    return res;
}

void AbstractSyntaxTree::addReference(int line_number, const TextSpan& text_with_col_range) {
    references[line_number].push_back(RangeEntry(line_number, text_with_col_range));
}

void AbstractSyntaxTree::addAlias(int line_number, const TextSpan& text_with_col_range) {
    RangeEntry parsed_alias = RangeEntry(line_number, text_with_col_range);

    aliases[parsed_alias.value] = parsed_alias.range;
}

std::optional<const std::vector<RangeEntry>> AbstractSyntaxTree::getReferences(int line_number) const {
    auto it = references.find(line_number);
    if (it != references.end()) {
        return it->second; 
    }
    return std::nullopt;
}

std::optional<const Range> AbstractSyntaxTree::getAliasRange(std::string alias) const {
    auto it = aliases.find(alias);
    if (it != aliases.end()) {
        return it->second; 
    }
    return std::nullopt;
}
