#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <nlohmann/json.hpp>

#include "lsp.h"
#include "cpp_syntax_definitions.h"
#include "cpp_diagnostic_definitions.h"
#include "cpp_parser_util.h"
#include "cpp_parser.h"
#include "cpp_ast_merger.h"

extern "C" {
    #include "c_tokenizer.h"
    #include "util.h"
}

using json = nlohmann::json;

class Link : public Range {
    protected:
        std::string destination;
    public:
        Link(Range r, std::string destination);
        Link(int start, int end, std::string destination);
        Link(const char* JSON_range, std::string destination);
        std::string linkDestination() const;
};

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
        int line_indent;
        tokens_t tokens;
        AbstractSyntaxTree line_ast;
        std::list<Link*> links;
    public:
        Line(std::string line_str, int line_indent, tokens_t tks, AbstractSyntaxTree line_ast);
        void addLink(Link* l);
        // std::string hasLinkAt(int column);
        const AbstractSyntaxTree& getAST() const { return line_ast; };
        int getLineIndent() const { return line_indent; };

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
        std::vector<Line> lines;
        AbstractSyntaxTree ast;
        int indent_width = 0;
        int max_id = 0;
        token_type_t indent_type = k_token_type_none;
        SemanticTokenModifier current_block = SemanticTokenModifier::none;
        ASTMerger merger;
        
    public:
        RqmtFile(const std::string& uri, const std::string& input);
        const std::string& uri() const { return _uri; };
        AbstractSyntaxTree& getAST() { return ast; };
        std::vector<std::string> getLineRange(const Range& range) const;
        void replaceLineRangeWith(const Range& range_to_replace, const std::vector<Line>& replacing_lines);
        void applyChange(const ContentChange& change);
        void applyChanges(const std::vector<ContentChange>& changes);
        json getDiagnostics(DiagnosticReportKind kind = DocumentDiagnosticReportKind::Full) const;
        json getGoToDefinition(TextLocation location) const;
};