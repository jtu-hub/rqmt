
#include <iostream>

extern "C" {
    #include "list.h"
    #include "util.h"
}

#include "cpp_util.h"
#include "cpp_parser.h"
#include "cpp_diagnostic_definitions.h"
#include "cpp_attribute_definitions.h"
#include "cpp_syntax_definitions.h"

typedef enum ParserState {
    k_parser_state_error = -1,
    k_parser_state_new_line,
    k_parser_state_handle_indent,
    k_parser_state_new_sequence,
    k_parser_state_parse_sequence,
} parser_state_t;

void checkIndentation(SyntaxNode& parent, token_type_t indent_type, 
                      int indent_width, int change_level, token_t t) {
    if(t.type != indent_type)
        parent.addDiagnostic(DiagnosticSeverity::error, 
                             DiagnosticId::mixed_indent, 
                             m_file_src, t.line, t.line, t.column, 
                             t.column + t.char_count);
    
    if(t.char_count % indent_width)
        parent.addDiagnostic(DiagnosticSeverity::error, 
                             DiagnosticId::indent_not_multiple, 
                             m_file_src, t.line, t.line, t.column, 
                             t.column + t.char_count);

    if(change_level > 1)
        parent.addDiagnostic(DiagnosticSeverity::error, 
                             DiagnosticId::unexpected_indent_depth, 
                             m_file_src, t.line, t.line, t.column, 
                             t.column + t.char_count);
}

parser_state_t handleIndentation(AbstractSyntaxTree& ast, token_t t,
                                 parser_state_t parser_state, 
                                 SyntaxNode** parent, int* indent_level, 
                                 int* indent_width, token_type_t* indent_type,
                                 bool is_single_line) {
    m_check_pointers(parent, *parent, indent_level, indent_width, indent_type);

    if(t.type == k_token_type_sindent || t.type == k_token_type_tindent) {
                        
        if(*indent_type == k_token_type_none) {
            if(t.char_count == 1 && t.type == k_token_type_sindent) {
                (*parent)->addDiagnostic(DiagnosticSeverity::warning, 
                             DiagnosticId::shallow_indent, 
                             m_file_src, t.line, t.line, t.column, 
                             t.column + t.char_count);
                (*parent)->addDiagnostic(DiagnosticSeverity::hint, 
                             DiagnosticId::shallow_indent_hint, 
                             m_file_src, t.line, t.line, t.column, 
                             t.column + t.char_count);
            }
            *indent_width = t.char_count;
            *indent_type = t.type;
        }

        if(parser_state == k_parser_state_new_line) {
            *indent_level =     (float)(t.char_count) / *indent_width;            
        } else {
            if(t.type != *indent_type)
                (*parent)->addDiagnostic( 
                                    DiagnosticSeverity::error, 
                                     DiagnosticId::mixed_indent, 
                                     m_file_src, t.line, t.line, t.column, 
                                     t.column + t.char_count);
        }

        return k_parser_state_handle_indent;
    } else if(t.type == k_token_type_new_line) {
        return k_parser_state_new_line;
    } else if(parser_state == k_parser_state_new_line) {
        *parent = ast.getRootPointer();
        *indent_level = 0;
    }
    
    return k_parser_state_new_sequence;   
}

void parseSequence(SyntaxNode& parent, int* pos_in_sequence, SyntaxId& id, 
                   const token_t& token, int* indent_level, bool* skip_get_token, 
                   bool* escape_next_token) {
    m_check_pointers(pos_in_sequence, indent_level, escape_next_token, skip_get_token);
        
    switch (id)
    {
    case SyntaxId::requirement_def:
    case SyntaxId::requirement_with_id_def:
        decodeRequirement(skip_get_token, escape_next_token, pos_in_sequence, id, parent, token, indent_level);
        break;
    case SyntaxId::requirement_description:
        decodeInlineTitleAndDescription(skip_get_token, escape_next_token, pos_in_sequence, id, parent, token);
        break;
    case SyntaxId::requirement_with_name_def:
        decodeInlineReferenceName(skip_get_token, escape_next_token, pos_in_sequence, id, parent, token);
        break;
    case SyntaxId::attribute_def:
        decodeAttribute(skip_get_token, escape_next_token, pos_in_sequence, id, parent, token);
        break;
    case SyntaxId::rich_text:
        decodeRichText(skip_get_token, escape_next_token, pos_in_sequence, id, parent, token);
        break;
    case SyntaxId::reference: 
        decodeReference(skip_get_token, escape_next_token, pos_in_sequence, id, parent, token);
        break;
    case SyntaxId::multiline_block:
    case SyntaxId::multiline_math:
        decodeMultilineBlock(skip_get_token, escape_next_token, pos_in_sequence, id, parent, token);
        break;
    case SyntaxId::inline_block:
    case SyntaxId::inline_math:
        decodeInlineBlock(skip_get_token, escape_next_token, pos_in_sequence, id, parent, token);
        break;
    default:
        break;
    }

    *escape_next_token = isTokenOfType(SyntaxElement::escape, token);
}

inline bool skipToken(token_t t) {
    return t.type == k_token_type_invalid || t.type == k_token_type_none;
}

AbstractSyntaxTree parseLine(tokens_t tokens, int* indent_level, int* indent_width, token_type_t* indent_type) {
    m_check_pointers(indent_level, indent_type, indent_width);

    AbstractSyntaxTree ast;
    parser_state_t parser_state = k_parser_state_new_line; 
    SyntaxNode *parent = ast.getRootPointer(), *temp_node = NULL;
    list_t sequence_candidates = {0}, discarded_candidates = {0};
    token_t* t = (token_t*)getNodeData(getHead(tokens.tokens));
    SyntaxId id = SyntaxId::none;
    bool skip_get_token = false, escape_next_token = false;
    int token_index = 0, position_in_sequence = 0;

    while(token_index < tokens.token_count) {
        SyntaxElement start_element = SyntaxElement::any;
        SemanticTokenDefinition* kw = NULL;
        skip_get_token = false;
        
        switch (parser_state)
        {
        case k_parser_state_new_line:
            if(t->type == k_token_type_new_line) break; 

            //!!! FALL THROUGH

        case k_parser_state_handle_indent:    
            parser_state = handleIndentation(ast, *t, parser_state, &parent, 
                                             indent_level, indent_width, 
                                             indent_type, true);  
  
            if(parser_state != k_parser_state_new_sequence) break;
            
            //!!! FALL THROUGH
            
        case k_parser_state_new_sequence:      
            if(t->type == k_token_type_new_line) {
                parser_state = k_parser_state_new_line;
                break;
            }    
            
            position_in_sequence = 0;

            parent = handleSequenceStart(id, parent->asReference(), *t, indent_level).asPointer();

            parser_state = k_parser_state_parse_sequence;    
            
            //!!! FALL THROUGH

        case k_parser_state_parse_sequence:    
            if(t->type == k_token_type_new_line) {
                parser_state = k_parser_state_new_line;
            }   

            if(!skipToken(*t))    
                parseSequence(parent->asReference(), &position_in_sequence, id, *t, 
                              indent_level, &skip_get_token, &escape_next_token);    
            
            break;
            
        case k_parser_state_error:
        default:    
            parser_state = k_parser_state_new_line;
            break;
        }

        if(!skip_get_token) {
            t = (token_t*)getNodeData(getNodeAt(tokens.tokens, ++token_index));
        }
    }

    return ast;
}
