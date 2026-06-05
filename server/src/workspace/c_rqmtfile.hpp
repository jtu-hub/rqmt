#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <nlohmann/json.hpp>

#include "lsp.h"

extern "C" {
    #include "c_parser.h"
    #include "c_tokenizer.h"
    #include "syntax_definitions.h"
    #include "diagnostic_definitions.h"
    #include "parser_util.h"
    #include "util.h"
}

using json = nlohmann::json;

typedef class Link : public Range {
    protected:
        std::string destination;
    public:
        Link(Range r, std::string destination);
        Link(int start, int end, std::string destination);
        Link(const char* JSON_range, std::string destination);
        std::string linkDestination() const;
} link_t;

class Alias : public Range {
    protected:
        std::string name;
    public:
        Alias(const char* JSON_range, std::string name);
        inline bool operator==(std::string& rhs) { return name == rhs; };
        inline std::string getName() { return name; }
};
    
inline bool operator==(Alias* lhs, std::string& rhs) { return lhs != NULL && *lhs == rhs; }

inline bool operator==(std::string& lhs, Alias* rhs) { return rhs != NULL && *rhs == lhs; }

class Line : public std::string {
    protected:
        int line_number;
        int line_indent;
        bool is_markdown;
        tokens_t tokens;
        ast_t line_ast;
        std::list<link_t*> links;
    public:
        Line(std::string line_str, int line_num, int line_indent, tokens_t tks, ast_t line_ast);
        void addLink(link_t* l);
        std::string hasLinkAt(int column);
        void setIsMarkdown(bool is_md) { is_markdown = is_md; };
        bool isMarkdown() { return is_markdown; };
        ast_t* getAST() { return &line_ast; };
        int getLineNumber() { return line_number; };
        int getLineIndent() { return line_indent; };

        void setLineIndent(int l_i) { line_indent = l_i; };

        std::string handleDelete(ContentChange change);
        std::string handleInsert(ContentChange change);
        std::string handleReplace(ContentChange change);
};

class RqmtFile {
    private:
        //file properties
        std::string _uri;
        
        //parsed properties
        std::vector<Line*> lines;
        std::vector<Alias*> aliases;
        ast_t ast{NULL};
        int indent_width = 0;
        token_type_t indent_type = k_token_type_none;
        semantic_token_modifier_t current_block = k_token_modifier_none;
        
    public:
        RqmtFile(const std::string& uri, const std::string& input);
        void print();
        void printMarkdownLines();
        void addLine(Line* l);
        void applySingleChange(const ContentChange& change);
        json applyChanges(std::vector<ContentChange> content_changes);
        void recomputeAst();
        int getLineCount();
        const char* goToDefinition(const char* click_location);
        syntax_node_t* getASTRoot() {return ast.root;};
        syntax_node_t* merger_getValidParent(syntax_node_t* into_root, syntax_node_t* from_root, int indent_level, int* found_at_level);
        void merger_handleChildRichText(syntax_node_t* parent, syntax_node_t* child);
        void merger_handleOverIndentOnRichText(syntax_node_t** parent, syntax_node_t* child, Line* line, int found_at_level);
        void merger_handleUnexpectedIndent(syntax_node_t* parent, syntax_node_t* child);
        void merger_extractAliasesAndLinks(syntax_node_t* child, Line* line);
        void merger_addChildren(Line* line, syntax_node_t** parent, int found_at_level);
        void merger_addAttributes(syntax_node_t* root, syntax_node_t** parent);
        void merger_addDiagnostics(syntax_node_t* root, syntax_node_t** parent);
        void merger_handleCurrentBlockLogic(semantic_token_modifier_t target_block, semantic_token_modifier_t alternative_block, syntax_node_t* child, semantic_token_definition_t* alternative_block_def);
        void merger_handleMultilineStrings(syntax_node_t* parent, syntax_node_t* child);
        bool merger_isLineMarkdown(syntax_node_t* child);
        void merger_handleInvalidDiagnostics(int child_idx, syntax_node_t* child, Line* line);
        void mergeAsts(ast_t into, Line* from);
        const std::string& uri() const { return _uri; };

        //TODO: this should be class LspAst(ast_t*)
    public:
        json getSemanticTokenData();
    private:
        int previous_line = 0;
        int previous_column = 0;
        void syntaxNodeToLSPData(syntax_node_t* node, std::vector<int>& data);
        void addSyntaxNodeLSPData(syntax_node_t* n, std::vector<int>& data);
};