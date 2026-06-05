#pragma once

#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <optional>

extern "C" {
    #include "c_token_definitions.h"
    #include "list.h"
}

#include "cpp_attribute_definitions.h"
#include "cpp_diagnostic_definitions.h"
#include "cpp_semantic_token_definitions.h"
#include "lsp_util.h"

#define m_is_syntax_node_keyword(syntax_node)   m_st_is_keyword((syntax_node)->id)
#define m_is_syntax_node_attribute(syntax_node) m_st_is_attribute((syntax_node)->id)
#define m_is_syntax_node_other(syntax_node)     m_st_is_other((syntax_node)->id)

using json = nlohmann::json;

class SyntaxNode {
    private:
        SemanticTokenDefinition definition = {};
        TextLocation location = {};

        int length = 0;

        std::vector<SyntaxNode> children = {};
        std::unordered_map<AttributeId, std::vector<NodeAttribute>> attributes = {};
        std::vector<Diagnostic> diagnostics = {};

        std::pair<AttributeId, size_t> current_attribute = {AttributeId::invalid, -1}; //attribute that has been decoded last TODO handle properly, or rather: implement it!! 

    public:
        SyntaxNode& operator=(const TextLocation& new_location) {
            location = new_location;
            return *this;
        }

        SyntaxNode& operator=(const SemanticTokenDefinition& new_definition) {
            definition = new_definition;
            return *this;
        }

        SyntaxNode& operator=(const token_t& t) {
            location.line   = t.line;
            location.column = t.column;
            length          = t.char_count;
            return *this;
        }

        bool operator==(const SemanticTokenId& id) const {
            return definition.id == id;
        }

        bool operator==(const SyntaxNode& n) const {
            return definition.id == n.getDefinition().id;
        }
        
        void addChild(SyntaxNode& child);
        void addAttribute(const AttributeId& attr_id, const std::string& value);
        void addDiagnostic(const DiagnosticSeverity& severity, const DiagnosticId& id, std::string source, int line_start, int line_end, int column_start, int column_end);
        void removeDiagnosticOfType(const DiagnosticId& id);
        /**
         * \brief recursive removal of diagnostics   
         **/
        void rremoveDiagnosticOfType(const DiagnosticId& id);
        SyntaxNode& newChild();
        SyntaxNode& newChildFromToken(const token_t& token, const SemanticTokenDefinition& token_definition);
        void recomputeLength(int token_column, int token_lenght);
        void addToLastAttributeOfTypeOrCreate(const AttributeId& attr_id, const std::string& attr_value);
        void renameCurrentAttribute(AttributeId old_id, AttributeId new_id);
        NodeAttribute& lastAttribute();
        NodeAttribute& lastAttributeOfType(const AttributeId& attr_id);
        NodeAttribute& firstAttributeOfType(AttributeId attr_id);
        SyntaxNode& lastChild();
        SyntaxNode& lastChildOfType(SemanticTokenId st_id);
        size_t childCount() const { return children.size(); };
        const SemanticTokenDefinition& getDefinition() const { return definition; };
        int getLine() const { return location.line; };
        void recursiveSetLine(int line_number);
        int getColumn() const { return location.column; };
        int getLength() const { return length; };
        void copyTokenDefinition(const SyntaxNode& from);
        SyntaxNode* asPointer() { return this; };
        SyntaxNode& asReference() { return *this; };
        void copyChildrenTo(SyntaxNode& target) const;
        void copyDiagnosticsTo(SyntaxNode& target) const;        
        const std::vector<SyntaxNode>& getChildren() const { return children; };
        const std::vector<NodeAttribute>& getAttributesById(AttributeId id) const;
        void debugPrintSyntaxNode(int level);
        void rdebugPrintSyntaxNode(int level);
        void overrideDefinition(const SemanticTokenDefinition& new_definition);
        void roverrideDefinition(const SemanticTokenDefinition& new_definition);
        json getDiagnosticLSPJSON() const;
        void rgetDiagnosticLSPJSON(json& array) const;
};

class AbstractSyntaxTree {
    private:
        std::unordered_map<int, std::vector<RangeEntry>> references;
        std::unordered_map<std::string, Range> aliases;

        SyntaxNode root = {};
        int previous_line = 0;
        int previous_column = 0;
        void syntaxNodeToLSPData(const SyntaxNode& node, std::vector<int>& data);
        void addSyntaxNodeLSPData(const SyntaxNode& n, std::vector<int>& data);

    public:
        AbstractSyntaxTree() { root = SemanticTokenDefinition{SemanticTokenId::root, "root", SemanticTokens::root, {SemanticTokenModifier::none}}; };
        
        SyntaxNode& lastChildAtLevel(int level, int* found_at_level);
        const SyntaxNode& getRoot() const { return root; };
        SyntaxNode* getRootPointer() { return &root; };
        json getSemanticTokenData();
        void debugPrintTree();
        void addReference(int line, const std::string& text_with_col_range);
        void addAlias(int line_number, const std::string& text_with_col_range);
        std::optional<const std::vector<RangeEntry>> getReferences(int line_number) const;
        std::optional<const Range> getAliasRange(std::string alias) const;
};
