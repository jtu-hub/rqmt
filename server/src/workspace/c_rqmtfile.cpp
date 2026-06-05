#include <string>
#include <format>
#include <sstream>

#include "rqmtfile.hpp"
#include "lsp.h"

extern "C" {
    #include "util.h"
}

Link::Link(Range r, std::string destination) :
Range(r),
destination(destination)
{}

Link::Link(int start, int end, std::string destination) :
Range(start, end),
destination(destination)
{}

Link::Link(const char* JSON_range, std::string destination) :
Range(JSON_range),
destination(destination)
{}

std::string Link::linkDestination() const {
    return destination;
}

Line::Line(std::string line_str, int line_num, int line_ind, tokens_t line_tks, ast_t line_ast) : 
std::string(line_str), 
line_number(line_num),
line_indent(line_ind),
tokens(line_tks),
line_ast(line_ast),
links()
{}

void Line::addLink(link_t* l) {
    links.push_back(l);
}

std::string Line::hasLinkAt(int column) {
    std::list<Link*>::iterator link = links.begin();

    while (link != links.end())
    {
        if((*link)->isInRange(line_number, column)) {
            return (*link)->linkDestination();
        }
        
        link++;
    }

    return std::string();
}

std::string Line::handleDelete(ContentChange change) {
    std::string out = *this;
    
    if(!change.is_single_line) return out;

    return this->substr(0, change.range.start.column) + this->substr(change.range.end.column);
}

std::string Line::handleInsert(ContentChange change) {
    std::string out = *this;
    
    if(!change.is_single_line) return out;

    return out.insert(change.range.start.column, change.text);
}

std::string Line::handleReplace(ContentChange change) {
    std::string out = *this;
    
    if(!change.is_single_line) return out;

    return out.replace(change.range.start.column, change.range.end.column - change.range.start.column, change.text);
}


Alias::Alias(const char* JSON_range, std::string name) :
Range(JSON_range),
name(name)
{}

RqmtFile::RqmtFile(const std::string& uri, const std::string& input) : _uri(uri) {
    std::istringstream stream(input);
    std::string line;
    int indent_level = 0;
    (void)addRootNode(&ast);

    while (std::getline(stream, line)) {
        line += '\n';

        tokens_t tks = tokenizeLine(line.c_str(), getLineCount());

        ast_t line_ast = parseLine(tks, &indent_level, &indent_width, &indent_type);

        Line* new_line = new Line(line, getLineCount(), indent_level, tks, line_ast);

        addLine(new_line);

        mergeAsts(ast, new_line);
    }
}

void RqmtFile::addLine(Line* l) {
    lines.push_back(l);
}

int RqmtFile::getLineCount() {
    return lines.size();
}

const char* RqmtFile::goToDefinition(const char* click_location) {
    int elements_to_decode = 2, 
        vals[elements_to_decode] = {0};

    parseColumnSeparatedInts(click_location, vals, elements_to_decode);

    int line = vals[0];
    int column = vals[1];

    if(line >= getLineCount()) return NULL;

    Line* line_p = lines[line];

    m_check_pointer(line_p);

    std::string alais_name = line_p->hasLinkAt(column);

    if(alais_name.empty()) return NULL;

    //TODO: for now references work only in file and if simple, i.e. {my_alias} 
    //      in a rich text is a reference to req{my_alias}, proposed features 
    //      like go to other file and stuff like that do not exist yet, further 
    //      process the alias name to understand if local reference or reference 
    //      across workspace
    std::vector<Alias*>::iterator alias = aliases.begin();

    while (alias != aliases.end())
    {
        if(alais_name == (*alias)) {
            int alias_line = (*alias)->getLineStart();

            bool alias_is_in_md = alias_line < getLineCount() && lines[alias_line]->isMarkdown();

            if(!alias_is_in_md) return (*alias)->getLSPRangeAsJSON().c_str(); //TODO: convert to std::string the retval
        }

        alias++;
    }
    
    return NULL;
}

void RqmtFile::printMarkdownLines() {
    std::vector<Line*>::iterator line = lines.begin();
    int prev_line = -1;

    while(line != lines.end()) {
        if(((*line)->isMarkdown())) {
            bool new_paragraph = prev_line >= 0 && ((*line)->getLineNumber() - prev_line) > 1;

            if(new_paragraph) {
                std::cout << std::endl;
            }

            std::cout << (*line)->substr((*line)->getLineIndent() * indent_width);

            prev_line = (*line)->getLineNumber();
        }

        line++;
    }

} 

void RqmtFile::applySingleChange(const ContentChange& change) {
    int line_index = change.startLine();
    
    if (line_index < 0 || line_index >= static_cast<int>(lines.size())) {
        return;
    }

    std::string new_line;

    switch (change.type) {
        case ContentChangeType::DeleteLine:
            //TODO add in destructor of line the deleteAST(&ast) and implement it in the relevant C file! 
            delete lines[line_index];
            lines.erase(lines.begin() + line_index);
            
            return;
        case ContentChangeType::Delete:
            new_line = lines[line_index]->handleDelete(change);
            break;
        case ContentChangeType::Insert:
            new_line = lines[line_index]->handleInsert(change);
            break;
        case ContentChangeType::Replace:
            new_line = lines[line_index]->handleReplace(change);
            break;
        default:
            break;
    }

    int indent_level = 0;

    tokens_t tks = tokenizeLine(new_line.c_str(), line_index);

    ast_t line_ast = parseLine(tks, &indent_level, &indent_width, &indent_type);

    delete lines[line_index];

    lines.at(line_index) = new Line(new_line, line_index, indent_level, tks, line_ast);

    recomputeAst();
}

void RqmtFile::recomputeAst() {
    std::cerr << "A\n";
    deleteAST(&ast);
    std::cerr << "B\n";

    (void)addRootNode(&ast);
    std::cerr << "C\n";

    for(Line* line : lines) {
    std::cerr << "D\n"  << line->c_str();
        mergeAsts(ast, line);
    std::cerr << "E\n";
    }
}

json RqmtFile::applyChanges(std::vector<ContentChange> content_changes) {
    json res;
    for (auto& content_change : content_changes) {
        if (content_change.is_single_line) {
            applySingleChange(content_change);
        } else {
            std::vector<ContentChange> single_line_changes = content_change.split();
            for (auto& change : single_line_changes) {
                applySingleChange(change);
            }
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MERGE HELPER FUNCTIONS /////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
syntax_node_t* RqmtFile::merger_getValidParent(syntax_node_t* into_root, syntax_node_t* from_root, int indent_level, int* found_at_level) {
    syntax_node_t* parent = lastChildAtLevel(into_root, indent_level, found_at_level);

    if(parent == NULL) {

        int i = 1;
        while (parent == NULL && indent_level - i >= 0)
        {
            parent = lastChildAtLevel(into_root, indent_level - i, found_at_level);
            i++;
        }
        
        m_check_pointer(parent);
    }

    return parent;
}

//This function handles rich text when we are merging a description into a requirement or text into an attribute
void RqmtFile::merger_handleChildRichText(syntax_node_t* parent, syntax_node_t* child) {
    if(child->id == k_st_id_source_body) {
        if(m_st_is_keyword(parent->id)) {
            semantic_token_definition_t* token_def = getDescriptionBodyDefinition();

            setTokenDefinition(child, token_def);
        }
        
        if(m_st_is_attribute(parent->id)) {
            semantic_token_definition_t* token_def = getValueBodyDefinition();

            setTokenDefinition(child, token_def);
        }
    }
}

//this function handles the edge case to assing text that is excessively indented to the right parent, e.g.:
// req description
//     normal indented description
//         excessively indented description child of req, not of the above description
//     normal again
void RqmtFile::merger_handleOverIndentOnRichText(syntax_node_t** parent, syntax_node_t* child, Line* line, int found_at_level) {

    if(merger_isLineMarkdown(child)) {

        rremoveDiagnosticOfType(line->getAST()->root, k_diagnostic_indent_not_multiple);
        rremoveDiagnosticOfType(line->getAST()->root, k_diagnostic_unexpected_indent);
        rremoveDiagnosticOfType(line->getAST()->root, k_diagnostic_unexpected_indent_depth);
        
        if(line->getLineIndent() >= found_at_level && merger_isLineMarkdown(*parent)) {

            copyTokenDefinition((*parent), child);

            line->setLineIndent(found_at_level - 1);
            if((*parent)->parent != NULL) *parent = (*parent)->parent;
        }
    }
}

//This fumction handles the case where an attribute or a keyword is defined with an indentation that is unexpected, e.g.
// req description
//     normal indented description
//         author: this author attribute is defined as being child of the description
//     author: this is the expected indentation level for a author attribute child of a requirement
void RqmtFile::merger_handleUnexpectedIndent(syntax_node_t* parent, syntax_node_t* child) {
    if(current_block != k_token_modifier_none || merger_isLineMarkdown(child)) return;

    if (((parent->token == k_semantic_token_meta || parent->token == k_semantic_token_string) && child->token == k_semantic_token_meta) ||
        ((parent->token == k_semantic_token_meta || parent->token == k_semantic_token_string) && child->token == k_semantic_token_keyword)) {
        addDiagnostic(parent, 
                            k_diagnostic_severity_error, 
                            k_diagnostic_unexpected_indent, 
                            m_diag_src, child->line, child->line, 
                            0, child->column - 1);
    }
}

void RqmtFile::merger_extractAliasesAndLinks(syntax_node_t* child, Line* line) {
    int i = 0; 

    while(i < child->attr_count) {
        node_attr_t* attr = child->attributes[i];

        if(attr->id == k_attr_alias) {
            if(i + 1 < child->attr_count && child->attributes[i + 1]->id == k_attr_alias_range) {
                aliases.push_back(new Alias(child->attributes[i + 1]->value, std::string(attr->value)));
            } else {
                //TODO error handling
                // printf("unexpected sequence of attributes\n");
                //exit(EXIT_FAILURE);
            }

        } else if(attr->id == k_attr_link) {
            if(i + 1 < child->attr_count && child->attributes[i + 1]->id == k_attr_link_range) {
                line->addLink(new Link(child->attributes[i + 1]->value, std::string(attr->value)));
            } else {
                //TODO error handling
                // printf("unexpected sequence of attributes\n");
                //exit(EXIT_FAILURE);
            }
        }

        i++;
    }
}

//This function handles the logic for multi-line strings e.g. $$ my multiline math expr.. $$, ``` my multiline backtick string ```. It assumes that the child that is being merged is being handled correctly, do not use this function for generic shifts of multiline block logic 
void RqmtFile::merger_handleCurrentBlockLogic(semantic_token_modifier_t target_block, semantic_token_modifier_t alternative_block, syntax_node_t* child, semantic_token_definition_t* alternative_block_def) {
    if(current_block == target_block) {
            //we are closing the current block
            current_block = k_token_modifier_none;
        } else if(current_block == k_token_modifier_none) {
            // we are opening a multiline block
            current_block = target_block;
        } else if(current_block == alternative_block) {
            //we are in another multiline block so we ignore this delimiter

            setTokenDefinition(child, alternative_block_def);
        } else{
            m_error_unhandled();
        }
}

void RqmtFile::merger_handleMultilineStrings(syntax_node_t* parent, syntax_node_t* child) {
    if(child->id == k_st_id_block_delim) {
        merger_handleCurrentBlockLogic(k_token_modifier_block, k_token_modifier_math, child, getStrMathDefinition());
    } else if(child->id == k_st_id_math_delim) {
        merger_handleCurrentBlockLogic(k_token_modifier_math, k_token_modifier_block, child, getStrBacktickDefinition());
    } else if(current_block == k_token_modifier_block) {
        semantic_token_definition_t* blck_def = getStrBacktickDefinition();
            
        setTokenDefinition(child, blck_def);

        for(int i = 0; i < child->child_count; i++) {
            setTokenDefinition(child->children[i], blck_def);
        }

        //invalidate reference links
        for(int i = 0; i < child->attr_count; i++) {
            if(child->attributes[i]->id == k_attr_link)
                renameAttribute(child->attributes[i], k_attr_link, k_attr_ignore);
            else if(child->attributes[i]->id == k_attr_link_range)
                renameAttribute(child->attributes[i], k_attr_link_range, k_attr_ignore);
        }
    } else if(current_block == k_token_modifier_math) {
        semantic_token_definition_t* math_def = getStrMathDefinition();

        setTokenDefinition(child, math_def);

        for(int i = 0; i < child->child_count; i++) {
            setTokenDefinition(child->children[i], math_def);
        }

        //invalidate reference links
        for(int i = 0; i < child->attr_count; i++) {
            if(child->attributes[i]->id == k_attr_link)
                renameAttribute(child->attributes[i], k_attr_link, k_attr_ignore);
            else if(child->attributes[i]->id == k_attr_link_range)
                renameAttribute(child->attributes[i], k_attr_link_range, k_attr_ignore);
        }
    } else {
        //we do nothing
    }
}

bool RqmtFile::merger_isLineMarkdown(syntax_node_t* child) {
    semantic_token_id_t id = child->id;

    return id == k_st_id_description_body  ||
           id == k_st_id_value_body        ||
           id == k_st_id_source_body       ||
           id == k_st_id_math_multiline    ||
           id == k_st_id_block_delim       ||
           id == k_st_id_math_delim        ||
           id == k_st_id_backtick_multiline;
}

//This function removes diagnostics that are no longer applicable in md text
void RqmtFile::merger_handleInvalidDiagnostics(int child_idx, syntax_node_t* child, Line* line) {
    if(child_idx == 0) {

        bool is_md = merger_isLineMarkdown(child);

        line->setIsMarkdown(is_md);

        if(is_md) {
            //rm diagnostics that are no longer applicable
            // ...

            if(current_block != k_token_modifier_none) {
                rremoveDiagnosticOfType(line->getAST()->root, k_diagnostic_wrong_ref_pattern);
            }
        }
    }
}

void RqmtFile::merger_addChildren(Line* line, syntax_node_t** parent, int found_at_level) {
    std::cerr << "    A\n";
    syntax_node_t* root = line->getAST()->root;
    std::cerr << "    B\n";

    m_check_pointer(root);
    std::cerr << "    C\n";

    for(int child_idx = 0; child_idx < root->child_count; child_idx++) {
    std::cerr << "    D\n";
        syntax_node_t* child = root->children[child_idx];
    std::cerr << "    E\n";

        merger_handleChildRichText(*parent, child);
    std::cerr << "    F\n";

        merger_handleMultilineStrings(*parent, child);
    std::cerr << "    G\n";

        merger_handleInvalidDiagnostics(child_idx, child, line);
    std::cerr << "    H\n";

        merger_handleOverIndentOnRichText(parent, child, line, found_at_level);
    std::cerr << "    I\n";
        
        merger_handleUnexpectedIndent(*parent, child);
    std::cerr << "    J\n";
        
        merger_extractAliasesAndLinks(child, line);
    std::cerr << "    K\n";
        
        addChild(*parent, child);
    std::cerr << "    L\n";
        
        // validateTree(parent, 0);
    }
}

void RqmtFile::merger_addAttributes(syntax_node_t* root, syntax_node_t** parent) {
    for(int i = 0; i < root->attr_count; i++) {
        node_attr_t* attr = root->attributes[i];

        addAttribute(*parent, attr->id, attr->value);
    }
}

void RqmtFile::merger_addDiagnostics(syntax_node_t* root, syntax_node_t** parent) {
    node_t* diagnostic_node = NULL;
    while(iter(&(root->diagnostics), &diagnostic_node)) {

        diagnostic_t* d = (diagnostic_t*)getNodeData(diagnostic_node);
  
        addDiagnostic(*parent, d->severity, d->id, d->source, d->line_start, d->line_end, d->column_start, d->column_end);
    }
}
//TODO: review merging under reduced/reviewed parser
void RqmtFile::mergeAsts(ast_t into, Line* line) {
    syntax_node_t* root = line->getAST()->root;
    int found_at_level = -1;

    m_check_pointer(root);
    
    syntax_node_t* parent = merger_getValidParent(into.root, root, line->getLineIndent(), &found_at_level);

    merger_addChildren(line, &parent, found_at_level);

    merger_addAttributes(root, &parent);

    merger_addDiagnostics(root, &parent);
}

void RqmtFile::addSyntaxNodeLSPData(syntax_node_t* n, std::vector<int>& data) {
    int delta_line = n->line - previous_line;
    int delta_column = (bool)delta_line ? 
        n->column : n->column - previous_column; 

    int token_modifiers = 0, i=0;
    semantic_token_modifier_t* modifiers = 
        (semantic_token_modifier_t*)n->modifiers;

    while(modifiers[i] != k_token_modifier_none && i < k_max_number_modifiers) {
        token_modifiers |= 1 << (modifiers[i] - 1);
        i++;
    }
    
    data.push_back(delta_line);
    data.push_back(delta_column);
    data.push_back(n->length);
    data.push_back(n->token);
    data.push_back(token_modifiers);
    
    previous_line   = n->line;
    previous_column = n->column;  
}

void RqmtFile::syntaxNodeToLSPData(syntax_node_t* node, std::vector<int>& data) {
    if (node->id != k_st_id_root) {
        addSyntaxNodeLSPData(node, data);
    }

    for (int i = 0; i < node->child_count; ++i) {        
        syntaxNodeToLSPData(node->children[i], data);
    }
}

json RqmtFile::getSemanticTokenData() {
    std::vector<int> data;
    
    syntaxNodeToLSPData(ast.root, data);

    json res = {{"data", data}};

    previous_line = 0; 
    previous_column = 0;

    return res;
}