#include <string>
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

Line::Line(std::string line_str, int line_ind, tokens_t line_tks, AbstractSyntaxTree line_ast) : 
std::string(line_str), 
line_indent(line_ind),
tokens(line_tks),
line_ast(line_ast),
links()
{}

void Line::addLink(Link* l) {
    links.push_back(l);
}

// std::string Line::hasLinkAt(int column) {
//     std::list<Link*>::iterator link = links.begin();

//     while (link != links.end())
//     {
//         if((*link)->isInRange(line_number, column)) {
//             return (*link)->linkDestination();
//         }
        
//         link++;
//     }

//     return std::string();
// }

Alias::Alias(const char* JSON_range, std::string name) :
Range(JSON_range),
name(name)
{}

RqmtFile::RqmtFile(const std::string& uri, const std::string& input) : _uri(uri) {
    std::istringstream stream(input);
    std::string line;
    int indent_level = 0;

    while (std::getline(stream, line)) {
        int line_count = static_cast<int>(lines.size());
        
        // std::cerr << "LINE " << line_count << ": " << line << "\n";

        tokens_t tks = tokenizeLine(line.c_str(), line_count);

        // std::cerr << "Tokens: ";
        // node_t* n = NULL;

        // while(iter(tks.tokens, &n)) {
        //     token_t* data = (token_t*)getNodeData(n);
            
        //     std::cerr << data->type << " ";
        // }

        // std::cerr << "\n";

        AbstractSyntaxTree line_ast = parseLine(tks, &indent_level, &indent_width, &indent_type);

        // std::cerr << "Computed line ast:\n";
        // line_ast.debugPrintTree();

        Line& new_line = lines.emplace_back(line, indent_level, tks, line_ast);
        
        // std::cerr << "Merging new line:\n";
        merger.merge(ast, new_line.getAST(), new_line.getLineIndent());
        // ast.debugPrintTree();

        // std::cerr << "Continue;\n\n";
    }
}


std::vector<std::string> RqmtFile::getLineRange(const Range& range) const {
    std::vector<std::string> result;

    if(lines.empty()) return result;

    const int start = std::max(0, range.start.line);
    const int end = std::min(static_cast<int>(lines.size()) - 1, range.end.line);

    if(start > end) return result;

    result.reserve(end - start + 1);

    for(int i = start; i <= end; ++i)
        result.push_back(lines[i]);

    return result;
}

void RqmtFile::replaceLineRangeWith(const Range& range_to_replace, const std::vector<Line>& replacing_lines) {
    if(lines.empty()) {
        lines = replacing_lines;
        return;
    }

    const int start = std::max(0, range_to_replace.start.line);
    const int end = std::min(static_cast<int>(lines.size()) - 1, range_to_replace.end.line);

    if(start > end) return;
    //TODO maybe optimize, overwrite the ones that are already allocated, delete if any to be deleted num_to_insert < num_to_delete, insert new onse if num_to_insert > num_to_delete
    lines.erase(lines.begin() + start, lines.begin() + end + 1);
    lines.insert(lines.begin() + start, replacing_lines.begin(), replacing_lines.end());
}

void RqmtFile::applyChange(const ContentChange& change)
{
    if(change.type == ContentChangeType::NoOp) return;

    std::vector<std::string> affected_lines = getLineRange(change.range);

    std::string new_text = change.applyChange(affected_lines);

    //iterate over new lines of string to tokenize, parse and store in line vector
    std::istringstream stream(new_text);
    std::string line;
    int indent_level = 0;
    std::vector<Line> new_lines;

    while (std::getline(stream, line)) {
        int line_count = change.range.start.line + static_cast<int>(new_lines.size());

        tokens_t tks = tokenizeLine(line.c_str(), line_count);

        AbstractSyntaxTree line_ast = parseLine(tks, &indent_level, &indent_width, &indent_type);

        new_lines.emplace_back(line, indent_level, tks, line_ast);
    }

    replaceLineRangeWith(change.range, new_lines);

    //recompute full ast
    AbstractSyntaxTree new_ast;
    merger.resetMultiline();

    for(int line_number = 0; line_number < lines.size(); line_number++) {
        Line line =  lines[line_number];

        merger.merge(new_ast, line.getAST(), line.getLineIndent(), line_number);
    }
    
    // new_ast.debugPrintTree();
    ast = new_ast;
}

void RqmtFile::applyChanges(const std::vector<ContentChange>& changes) {
    for(const auto& change : changes) applyChange(change);
}

json RqmtFile::getDiagnostics(DiagnosticReportKind kind) const {
    json res = {
        {"kind", kind},
        {"items", ast.getRoot().getDiagnosticLSPJSON()}
    };
    return res;
}

json RqmtFile::getGoToDefinition(TextLocation location) const {
    auto got_references = ast.getReferences(location.line);

    if(got_references) {
        const std::vector<RangeEntry>& potential_refs = *got_references;

        for(const RangeEntry& ref : potential_refs) {
            if(ref.range.isInRange(location)) {
                //TODO: implement refs to other files in workspace 
                auto got_alias_range = ast.getAliasRange(ref.value);
                
                if(got_alias_range) {
                    const Range& goto_location = *got_alias_range;
                    
                    json out = {
                        {"uri", _uri},
                        goto_location.getLSPRangeAsJSON()
                    };

                    return out;
                }
            }
        }
    }

    return {nullptr};
}
