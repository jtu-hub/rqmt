#include <iostream>
#include <string>

extern "C" {
    #include "util.h"
}

#include "cpp_util.h"
#include "cpp_syntax_definitions.h"
#include "cpp_diagnostic_definitions.h"


SemanticTokenDefinition keywords[] = {
    {SemanticTokenId::kw_req, "req", SemanticTokens::keyword, {SemanticTokenModifier::req}},
    {SemanticTokenId::kw_fr,  "fr",  SemanticTokens::keyword, {SemanticTokenModifier::fr} },
    {SemanticTokenId::kw_br,  "br",  SemanticTokens::keyword, {SemanticTokenModifier::br} },
    {SemanticTokenId::kw_nfr, "nfr", SemanticTokens::keyword, {SemanticTokenModifier::nfr}},
    {SemanticTokenId::kw_ur,  "ur",  SemanticTokens::keyword, {SemanticTokenModifier::ur} },
    {SemanticTokenId::kw_sr,  "sr",  SemanticTokens::keyword, {SemanticTokenModifier::sr} },
    
    //...
};

SemanticTokenDefinition attributes[] = {
    {SemanticTokenId::attr_author,      "author",      SemanticTokens::meta, {SemanticTokenModifier::attr, SemanticTokenModifier::author} },
    {SemanticTokenId::attr_title,       "title",       SemanticTokens::meta, {SemanticTokenModifier::attr, SemanticTokenModifier::title}  },
    {SemanticTokenId::attr_description, "description", SemanticTokens::meta, {SemanticTokenModifier::attr, SemanticTokenModifier::desc}   },
    {SemanticTokenId::attr_date,        "date",        SemanticTokens::meta, {SemanticTokenModifier::attr, SemanticTokenModifier::date}   },
    {SemanticTokenId::attr_type,        "type",        SemanticTokens::meta, {SemanticTokenModifier::attr, SemanticTokenModifier::type}   },

    //...
};

SemanticTokenDefinition value_body      = {SemanticTokenId::value_body,         "value.body",             SemanticTokens::string,      {SemanticTokenModifier::unquoted, SemanticTokenModifier::none}                                                  };
SemanticTokenDefinition value           = {SemanticTokenId::value,              "value",                  SemanticTokens::string,      {SemanticTokenModifier::unquoted, SemanticTokenModifier::none}                                                  };
SemanticTokenDefinition str_math        = {SemanticTokenId::math_multiline,     "math.multiline",         SemanticTokens::string,      {SemanticTokenModifier::math, SemanticTokenModifier::multiline, SemanticTokenModifier::none}                          };
SemanticTokenDefinition str_blck        = {SemanticTokenId::backtick_multiline, "backtick.multiline",     SemanticTokens::string,      {SemanticTokenModifier::backtick, SemanticTokenModifier::multiline, SemanticTokenModifier::none}                      };
SemanticTokenDefinition str_math_inline = {SemanticTokenId::math_inline,        "math.inline",            SemanticTokens::string,      {SemanticTokenModifier::math, SemanticTokenModifier::inline_token, SemanticTokenModifier::none}                             };
SemanticTokenDefinition str_blck_inline = {SemanticTokenId::backtick_inline,    "backtick.inline",        SemanticTokens::string,      {SemanticTokenModifier::backtick, SemanticTokenModifier::inline_token, SemanticTokenModifier::none}                         };
SemanticTokenDefinition desc_body       = {SemanticTokenId::description_body,   "description.body",       SemanticTokens::meta,        {SemanticTokenModifier::desc, SemanticTokenModifier::none}                                                      };
SemanticTokenDefinition desc            = {SemanticTokenId::description,        "description",            SemanticTokens::meta,        {SemanticTokenModifier::desc, SemanticTokenModifier::none}                                                      };
SemanticTokenDefinition title           = {SemanticTokenId::title,              "title",                  SemanticTokens::meta,        {SemanticTokenModifier::title, SemanticTokenModifier::none}                                                     };
SemanticTokenDefinition col_attr        = {SemanticTokenId::column_attr,        "column.attr",            SemanticTokens::punctuation, {SemanticTokenModifier::separator, SemanticTokenModifier::colon,  SemanticTokenModifier::attr, SemanticTokenModifier::none} };
SemanticTokenDefinition col_title       = {SemanticTokenId::column_title,       "column.title",           SemanticTokens::punctuation, {SemanticTokenModifier::separator, SemanticTokenModifier::colon,  SemanticTokenModifier::title, SemanticTokenModifier::none}};
SemanticTokenDefinition source          = {SemanticTokenId::source_body,        "source.body",            SemanticTokens::source,      {SemanticTokenModifier::none}                                                                             };
SemanticTokenDefinition alias_b         = {SemanticTokenId::alias_brace,        "alias.brace",            SemanticTokens::punctuation, {SemanticTokenModifier::brace, SemanticTokenModifier::alias, SemanticTokenModifier::none}                             };
SemanticTokenDefinition alias_v         = {SemanticTokenId::alias_value,        "alias.value",            SemanticTokens::meta,        {SemanticTokenModifier::alias, SemanticTokenModifier::none}                                                     };
SemanticTokenDefinition ref_brace       = {SemanticTokenId::reference_brace,    "reference.brace",        SemanticTokens::punctuation, {SemanticTokenModifier::brace, SemanticTokenModifier::reference, SemanticTokenModifier::none}                         };
SemanticTokenDefinition ref_delim       = {SemanticTokenId::reference_delim,    "reference.delim",        SemanticTokens::punctuation, {SemanticTokenModifier::delimiter, SemanticTokenModifier::reference, SemanticTokenModifier::none}                     };
SemanticTokenDefinition ref_value       = {SemanticTokenId::reference_value,    "reference.value",        SemanticTokens::meta,        {SemanticTokenModifier::reference, SemanticTokenModifier::none}                                                 };
SemanticTokenDefinition id_delim        = {SemanticTokenId::id_delim,           "id.delim",               SemanticTokens::punctuation, {SemanticTokenModifier::delimiter, SemanticTokenModifier::id, SemanticTokenModifier::none}                            };
SemanticTokenDefinition id_number       = {SemanticTokenId::id_number,          "id.number",              SemanticTokens::source,      {SemanticTokenModifier::id, SemanticTokenModifier::none}                                                        };
SemanticTokenDefinition blck_delim      = {SemanticTokenId::block_delim,        "block.delim",            SemanticTokens::punctuation, {SemanticTokenModifier::block, SemanticTokenModifier::delimiter, SemanticTokenModifier::none}                         };
SemanticTokenDefinition math_delim      = {SemanticTokenId::math_delim,         "math.delim",             SemanticTokens::punctuation, {SemanticTokenModifier::math, SemanticTokenModifier::delimiter, SemanticTokenModifier::none}                          };
SemanticTokenDefinition blck_delim_ml   = {SemanticTokenId::block_delim_ml,     "block.delim.multiline",  SemanticTokens::punctuation, {SemanticTokenModifier::block, SemanticTokenModifier::delimiter, SemanticTokenModifier::multiline, SemanticTokenModifier::none}                         };
SemanticTokenDefinition math_delim_ml   = {SemanticTokenId::math_delim_ml,      "math.delim.multiline",   SemanticTokens::punctuation, {SemanticTokenModifier::math, SemanticTokenModifier::delimiter, SemanticTokenModifier::multiline, SemanticTokenModifier::none}                          };
SemanticTokenDefinition delim_sp        = {SemanticTokenId::space,              "space.delim",            SemanticTokens::space,       {SemanticTokenModifier::delimiter, SemanticTokenModifier::none}                          };
SemanticTokenDefinition multiline_scope = {SemanticTokenId::multiline_scope,    "meta.multiline",         SemanticTokens::meta,        {SemanticTokenModifier::multiline, SemanticTokenModifier::none}                          };


SemanticTokenDefinition& getValueBodyDefinition() {
    return value_body;
}

SemanticTokenDefinition& getDescriptionBodyDefinition() {
    return desc_body;
}

SemanticTokenDefinition& getSourceDefinition() {
    return source;
}

SemanticTokenDefinition& getStrMathDefinition() {
    return str_math;
}

SemanticTokenDefinition& getStrBacktickDefinition() {
    return str_blck;
}

/// @brief Generates a dynamically allocated string containing the line range 
///        with the following format: "`column_start`:`column_end`"
/// @param column_start     the column at which the range starts on given start 
///                         line
/// @param column_end       the column at which the range ends on given end line

std::string rangeAttributeString(int column_start, int column_end) {
    std::stringstream out;
    
    out << column_start << ":" << column_end;
    
    return out.str();
}

const char* requirementRangeAttributeJSON(int line_start, int column_start, int line_end, int column_end) {
    return dsprintf(
        "{"
            "\"range\":{"
                "\"start\":{\"line\":%d,\"character\":%d},"
                "\"end\":{\"line\":%d,\"character\":%d}"
            "}"
        "}",
        line_start, column_start, line_end, column_end
    );
}

bool blockDelimiterIsValid(const token_t& token) {
    if(token.char_count > 0) {
        if(token.value[0] == '$') {
            //validate math fences either "$" or  "$$"
            return (token.char_count == 1 || token.char_count == 2) && token.type == k_token_type_math_delim; 
            
        } else if(token.value[0] == '`') {
            //validate block fences either "`" or "```"
            return (token.char_count == 1 || token.char_count == 3) && token.type == k_token_type_backtick_delim; 

        }
    }
    
    return false;
}

SyntaxNode& handleSequenceStart(SyntaxId& id, SyntaxNode& parent, const token_t& token, int* indent_level) {
    if(id != SyntaxId::none) return parent;

    SemanticTokenDefinition* keyword = NULL;

    bool is_token_attr = tokenIsAttribute(token, &keyword);  

    if(is_token_attr || tokenIsKeyword(token, &keyword)) {
        id = is_token_attr ? 
            SyntaxId::attribute_def  : 
            SyntaxId::requirement_def;
    
        SyntaxNode& child = parent.newChild();
        
        child = *keyword;
        
        return child;
    } else if(isTokenOfType(SyntaxElement::text_like, token)) {
        SyntaxNode& child = parent.newChild();

        id = SyntaxId::rich_text;
            
        child = source;  
    }

    return parent;
}

void decodeRequirement(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token, int* indent_level) {
    m_check_pointers(skip_get_token, escape_next_token, pos_in_seq, indent_level);

    switch (*pos_in_seq)
    {
    case 0:
        //token.value = "req" | "fr" | ...

        parent = token;

        (*pos_in_seq)++;

        return; //handled in c_parser.c
    case 1:
        if(isTokenOfType(SyntaxElement::hyphen, token)) {
            id = SyntaxId::requirement_with_id_def;
            
            SyntaxNode& child = parent.newChild();

            child = token;
            child = id_delim;

            (*pos_in_seq)++;
        } else if(isTokenOfType(SyntaxElement::brace_ref_o, token)) {
            //Here a name attribute is being set implicitly, change to relevant
            //sequence (e.g. for syntax req{my_req_name} )
            id = SyntaxId::requirement_with_name_def;
            *pos_in_seq = 0;
            *skip_get_token = true; //reprocess token in other sequence
        } else if(isTokenOfType(SyntaxElement::space, token)) {
            //Title or description must start with text token!
            id = SyntaxId::requirement_description;
            *pos_in_seq = 0;
            *skip_get_token = true; //reprocess token in other sequence
        } else {
            id = SyntaxId::none;

            //TODO: quick fix: add an empty child to not raise the unexpected 
            //      indent error after a definition of a req without any inline 
            //      title or description defined. 
            //      See c_parser.c handleIndentation(...)
            //      Potential soultion, add child as description and retrieve 
            //      when adding description
            SyntaxNode& child = parent.newChild();

            
            child = token_t{
                .line = token.line, 
                .column = token.column, 
                .char_count = 0
            };

            child = desc;
            
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token, 
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
        }

        return;
    case 2:
        if(isTokenOfType(SyntaxElement::number, token)) {
            SyntaxNode& child = parent.newChild();

            
            child = token;
            child = id_number;

            parent.addAttribute(AttributeId::req_id, std::string(token.value));
            parent.addAttribute(AttributeId::req_id_range, rangeAttributeString(token.column, token.column + token.char_count));

            id = SyntaxId::requirement_description;
            *pos_in_seq = 0;            
        } else {
            id = SyntaxId::none;

            parent.addDiagnostic(DiagnosticSeverity::error,
                                 DiagnosticId::unexpected_token,
                                 m_file_src, token.line, token.line, 
                                 token.column, token.column + 
                                 token.char_count);
            return;
        }

        return;
    default:
        id = SyntaxId::none;

        parent.addDiagnostic(DiagnosticSeverity::error,
                             DiagnosticId::unexpected_token,
                             m_file_src, token.line, token.line, 
                             token.column, token.column + token.char_count);
        return;
    }
}

void decodeInlineReferenceName(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token) {
    m_check_pointers(skip_get_token, escape_next_token, pos_in_seq);

    switch (*pos_in_seq)
    {
    case 0:
        //Reference must start with '{'
        if(!isTokenOfType(SyntaxElement::brace_ref_o, token)) {
            id = SyntaxId::none;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
        } else {

            SyntaxNode& child = parent.newChild();

            
            child = token;
            child = alias_b;

            (*pos_in_seq)++;
        }

        return;

    case 1:
        // the name of the alias has to start with a k_ref_name token type
        if(isTokenOfType(SyntaxElement::ref_name, token)) {

            SyntaxNode& child = parent.newChild();

            
            parent.addAttribute(AttributeId::alias, std::string(token.value));

            child = token;
            child = alias_v;

            (*pos_in_seq)++;            
        } else {
            id = SyntaxId::none;
            
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token,
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
        }
        return;
    case 2:
        //as long as the name is composed of k_ref_name
        if(isTokenOfType(SyntaxElement::ref_name, token)) {
            NodeAttribute& alias = parent.lastAttribute();
            SyntaxNode& child = parent.lastChild();
            
            
            alias += std::string(token.value);
            child.recomputeLength(token.column, token.char_count);

        } else if (!isTokenOfType(SyntaxElement::brace_ref_c, token)) {
            id = SyntaxId::none;
            //TODO add more diagnostic that explains why unexpected token
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
            
        } else {
            SyntaxNode& ref_name = parent.lastChild();
            
            parent.addToLastAttributeOfTypeOrCreate(AttributeId::alias, ":" + rangeAttributeString(parent.getColumn(), ref_name.getColumn() + ref_name.getLength() + 1));
            parent.lastAttributeOfType(AttributeId::alias).valid = true;
            
            SyntaxNode& closing_brace = parent.newChild();

            closing_brace = token;
            closing_brace = alias_b;
            
            (*pos_in_seq)++; 
        }

        return;
    case 3:
        
        if(isTokenOfType(SyntaxElement::hyphen, token)) {
            id = SyntaxId::requirement_with_id_def;
            
            SyntaxNode& child = parent.newChild();

            
            child = token;
            child = id_delim;

            *pos_in_seq = 2;

        } else if(isTokenOfType(SyntaxElement::space, token)) {
            //Title or description must start with text token after a space!
            id = SyntaxId::requirement_description;
            *pos_in_seq = 0;
            *skip_get_token = true; //reprocess token in other sequence

        } else {
            id = SyntaxId::none;

            //TODO: quick fix: add an empty child to not raise the unexpected 
            //      indent error after a definition of a req without any inline 
            //      title or description defined. 
            //      See c_parser.c handleIndentation(...)
            //      Potential soultion, add child as description and retrieve 
            //      when adding description
            SyntaxNode& child = parent.newChild();

            
            child = token_t{
                .line = token.line, 
                .column = token.column, 
                .char_count = 0
            };

            child = desc;
            
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token, 
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
        }

        return;

    default:
        id = SyntaxId::none;

        parent.addDiagnostic(DiagnosticSeverity::error,
                             DiagnosticId::unexpected_token,
                             m_file_src, token.line, token.line, 
                             token.column, token.column + token.char_count);
        return;
    }
}

void decodeReference(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token) {
    m_check_pointers(skip_get_token, escape_next_token, pos_in_seq);

    switch (*pos_in_seq)
    {
    case 0:
        //Reference must start with '{'
        if(!isTokenOfType(SyntaxElement::brace_ref_o, token)) {
            id = SyntaxId::none;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.lastChild().addDiagnostic( 
                                     DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
        } else {
            SyntaxNode& child = parent.lastChild().newChild();

                        
            parent.lastChild().addAttribute(AttributeId::link, "");
            
            child = token;
            child = ref_brace;

            (*pos_in_seq)++;
        }

        return;

    case 1:
        
        if(isTokenOfType(SyntaxElement::dot, token)) {
            (void)parent.lastChild().newChildFromToken(token, ref_delim);
            
            parent.lastChild().addToLastAttributeOfTypeOrCreate(AttributeId::link, std::string(token.value));

        } else if(isTokenOfType(SyntaxElement::ref_name, token)) {
            (void)parent.lastChild().newChildFromToken(token, ref_value);
            
            parent.lastChild().addToLastAttributeOfTypeOrCreate(AttributeId::link, std::string(token.value));

            (*pos_in_seq)++;            
        } else {
            id = SyntaxId::rich_text;//SyntaxId::none;
            
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.lastChild().addDiagnostic( 
                                    DiagnosticSeverity::error,
                                    DiagnosticId::wrong_ref_pattern,
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
        }

        return;
    case 2:
        if(isTokenOfType(SyntaxElement::dot, token)) {
            (void)parent.lastChild().newChildFromToken(token, ref_delim);
            
            parent.lastChild().addToLastAttributeOfTypeOrCreate(AttributeId::link, std::string(token.value));
            
            (*pos_in_seq)++;
        } else if(isTokenOfType(SyntaxElement::ref_name, token)) {
            SyntaxNode& child = parent.lastChild().lastChild();

            
            child.recomputeLength(token.column, token.char_count);
            
            parent.lastChild().addToLastAttributeOfTypeOrCreate(AttributeId::link, std::string(token.value));

        } else if(isTokenOfType(SyntaxElement::brace_ref_c, token)) {
            SyntaxNode& child = parent.lastChild().lastChild();
                   
            parent.lastChild().addToLastAttributeOfTypeOrCreate(AttributeId::link, ":" + rangeAttributeString(child.getColumn(), child.getColumn() + child.getLength()));
            parent.lastChild().lastAttributeOfType(AttributeId::link).valid = true;

            (void)parent.lastChild().newChildFromToken(token, ref_brace);
            
            //end of reference reached -> go back to description! 
            id = SyntaxId::rich_text;
            *pos_in_seq = 2;
        } else {
            id = SyntaxId::rich_text;//SyntaxId::none;
            
            if(!isTokenOfType(SyntaxElement::new_line, token))                
                parent.lastChild().addDiagnostic( 
                                    DiagnosticSeverity::error,
                                    DiagnosticId::wrong_ref_pattern,
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
        }

        return;

    case 3:
        if(isTokenOfType(SyntaxElement::ref_name, token)) {
            (void)parent.lastChild().newChildFromToken(token, ref_value);
            
            parent.lastChild().addToLastAttributeOfTypeOrCreate(AttributeId::link, std::string(token.value));
            
            (*pos_in_seq)--;
        } else {
            id = SyntaxId::rich_text;
            
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.lastChild().addDiagnostic( 
                                    DiagnosticSeverity::error,
                                    DiagnosticId::wrong_ref_pattern,
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
        }

        return;

    default:
        id = SyntaxId::none;

        parent.lastChild().addDiagnostic( 
                             DiagnosticSeverity::error,
                             DiagnosticId::unexpected_token,
                             m_file_src, token.line, token.line, 
                             token.column, token.column + token.char_count);
        return;
    }
}

void decodeInlineTitleAndDescription(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token) {
    m_check_pointers(skip_get_token, escape_next_token, pos_in_seq);

    switch (*pos_in_seq)
    {
    case 0:
        //before the title after the requirement definition sequence e.e req{alias}-000 a space must be used as delimiter
        if(!isTokenOfType(SyntaxElement::space, token)) {
            id = SyntaxId::none;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
        } else {

            SyntaxNode& child = parent.newChild();

            
            child = token;
            child = delim_sp;

            (*pos_in_seq)++;
        }

        return;
    case 1:
        //Title or description must start with k_text tokens
        if(!isTokenOfType(SyntaxElement::text, token)) {
            id = SyntaxId::none;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
        } else {

            SyntaxNode& child = parent.newChild();

            parent.addToLastAttributeOfTypeOrCreate(AttributeId::description, std::string(token.value));

            
            child = token;
            child = desc;

            (*pos_in_seq)++;
        }

        return;

    case 2:
        if(isTokenOfType(SyntaxElement::column, token) && !(*escape_next_token)) {
            SyntaxNode& child = parent.lastChild();
            
            child = title;

            parent.renameCurrentAttribute(AttributeId::description, AttributeId::title);

            SyntaxNode& column = parent.newChild();
            
            column = token;
            column = col_title;

            (*pos_in_seq)++;            
        } else if(isTokenOfType(SyntaxElement::text_like, token)) {
            SyntaxNode& child = parent.lastChild();
            
            child.recomputeLength(token.column, token.char_count);
            parent.addToLastAttributeOfTypeOrCreate(AttributeId::description, std::string(token.value));
        } else {
            id = SyntaxId::none;
            
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token,
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
        }
        return;
    case 3:
        //before the title after the requirement definition sequence e.e req{alias}-000 a space must be used as delimiter
        if(!isTokenOfType(SyntaxElement::space, token)) {
            id = SyntaxId::none;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
        } else {

            SyntaxNode& child = parent.newChild();

            
            child = token;
            child = delim_sp;

            (*pos_in_seq)++;
        }
        
        return;
    case 4:
        //Description must start with k_text or number tokens
        if(!(isTokenOfType(SyntaxElement::text, token) || isTokenOfType(SyntaxElement::number, token))) {
            id = SyntaxId::none;
            
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
            
        } else {
            parent.addToLastAttributeOfTypeOrCreate(AttributeId::description, std::string(token.value));

            SyntaxNode& child = parent.newChild();
            
            child = token;
            child = desc;

            (*pos_in_seq)++;
        }

        return;
    case 5:
        if(isTokenOfType(SyntaxElement::text_like, token)) {
            SyntaxNode& child = parent.lastChild();

            child.recomputeLength(token.column, token.char_count);
            parent.addToLastAttributeOfTypeOrCreate(AttributeId::description, std::string(token.value));
        } else {
            id = SyntaxId::none;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token,
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
        }
        return;
    default:
        id = SyntaxId::none;

        parent.addDiagnostic(DiagnosticSeverity::error,
                             DiagnosticId::unexpected_token,
                             m_file_src, token.line, token.line, 
                             token.column, token.column + token.char_count);
        return;
    }
}

void decodeAttribute(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token) {
    m_check_pointers(skip_get_token, escape_next_token, pos_in_seq);
    
    switch (*pos_in_seq)
    {
    case 0:
        //token.value = "title" | "description" | ...

        parent = token;

        (*pos_in_seq)++;

        return; //handled in c_parser.c  
    case 1:
        if(isTokenOfType(SyntaxElement::column, token)) {
            (void)parent.newChildFromToken(token, col_attr);

            (*pos_in_seq)++;
        } else {
            parent.addDiagnostic(DiagnosticSeverity::error,
                                 DiagnosticId::unexpected_token,
                                 m_file_src, token.line, token.line, 
                                 token.column, token.column + 
                                 token.char_count);

            id = SyntaxId::none;
        }

        return;
    case 2:
        //Value text like tokens
        if(!isTokenOfType(SyntaxElement::text_like, token)) {
            //TODO: quick fix: add an empty child to not raise the unexpected 
            //      indent error after a definition of an attr without any inline 
            //      value defined. 
            //      See c_parser.c handleIndentation(...)
            //      Potential soultion, add child as description and retrieve 
            //      when adding description
            SyntaxNode& child = parent.newChild();

            
            child = token_t{
                .line = token.line, 
                .column = token.column, 
                .char_count = 0
            };

            child = value;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);

            id = SyntaxId::none;
            return;
        } else {           
            (void)parent.newChildFromToken(token, value);
            
            // parent.addAttribute(AttributeId::text, token.value);

            (*pos_in_seq)++;
        }

        return;
    case 3:
        //Value text like tokens
        if(!isTokenOfType(SyntaxElement::text_like, token)) {
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);

            id = SyntaxId::none;
            return;
        } else {
            SyntaxNode& child = parent.lastChild();

            
            child.recomputeLength(token.column, token.char_count);
        }
        return;
    default:
        id = SyntaxId::none;

        parent.addDiagnostic(DiagnosticSeverity::error,
                             DiagnosticId::unexpected_token,
                             m_file_src, token.line, token.line, 
                             token.column, token.column + token.char_count);
        return;
    }
}

void decodeRichText(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token) {
    m_check_pointers(skip_get_token, escape_next_token, pos_in_seq);

    switch (*pos_in_seq)
    {
    case 0: 
        if(isTokenOfType(SyntaxElement::brace_ref_o, token)) {
            
            SyntaxNode& child = parent.lastChild();
            
            child = token_t{
                .line = token.line, 
                .column = token.column, 
                .char_count = 0
            };

            id = SyntaxId::reference;
            *pos_in_seq = 0;
            *skip_get_token = true;
            
        } else if(isTokenOfType(SyntaxElement::block_delim, token) && blockDelimiterIsValid(token)) {
            //TODO make a function out of this and replace in this function where occurs
             if(token.char_count > 0 && token.value[0] == '`') {
                if(token.char_count > 1) {
                    // TODO add error: no multiline allowed in rich text
                    // SyntaxId::multiline_block;
                    const token_type_t delim_type = k_token_type_backtick_delim;
                    SemanticTokenDefinition delim_def = blck_delim_ml;
                    SemanticTokenDefinition current_rich_text = parent.lastChild().getDefinition();

                    parent.lastChild() = delim_def;
                    parent.lastChild() = token;

                    SyntaxNode& text_continuation = parent.newChild();

                    text_continuation = current_rich_text;
                    text_continuation = token_t{
                        .line = token.line, 
                        .column = token.column, 
                        .char_count = 0
                    };

                } else {
                    SyntaxNode& child = parent.lastChild();
            
                    child = token_t{
                        .line = token.line, 
                        .column = token.column, 
                        .char_count = 0
                    };

                    *pos_in_seq = 0;
                    *skip_get_token = true;
                    id = SyntaxId::inline_block;
                }
            } else {
                if(token.char_count > 1) {
                    // TODO add error: no multiline allowed in rich text
                    // SyntaxId::multiline_math;
                    const token_type_t delim_type = k_token_type_math_delim;
                    SemanticTokenDefinition delim_def = math_delim_ml;
                    SemanticTokenDefinition body_def = str_math;
                    SemanticTokenDefinition current_rich_text = parent.lastChild().getDefinition();

                    parent.lastChild() = delim_def;
                    parent.lastChild() = token;

                    SyntaxNode& text_continuation = parent.newChild();

                    text_continuation = current_rich_text;
                    text_continuation = token_t{
                        .line = token.line, 
                        .column = token.column, 
                        .char_count = 0
                    };

                } else {
                    SyntaxNode& child = parent.lastChild();
            
                    child = token_t{
                        .line = token.line, 
                        .column = token.column, 
                        .char_count = 0
                    };
                    
                    *pos_in_seq = 0;
                    *skip_get_token = true; 
                    id = SyntaxId::inline_math;
                }
            }
            
        } else {
            SyntaxNode& child = parent.lastChild();
            
            child = token;
            (*pos_in_seq)++;     

        }

        return; //handled in c_parser.c
    case 1:
        if(isTokenOfType(SyntaxElement::brace_ref_o, token) && !(*escape_next_token)) {
            id = SyntaxId::reference;
            *pos_in_seq = 0;
            *skip_get_token = true;

        } else if(isTokenOfType(SyntaxElement::block_delim, token) && blockDelimiterIsValid(token) && !(*escape_next_token)) {
            if(token.char_count > 0 && token.value[0] == '`') {
                if(token.char_count > 1) {
                    // SyntaxId::multiline_block;
                    // TODO add error: no multiline allowed in rich text
                    parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token, 
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);

                    SyntaxNode& child = parent.lastChild();
    
                    child.recomputeLength(token.column, token.char_count);

                } else {
                    *pos_in_seq = 0;
                    *skip_get_token = true;
                    id = SyntaxId::inline_block;
                }
            } else {
                if(token.char_count > 1) {
                    // SyntaxId::multiline_math;
                    // TODO add error: no multiline allowed in rich text
                    parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token, 
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);

                    SyntaxNode& child = parent.lastChild();
    
                    child.recomputeLength(token.column, token.char_count);
                } else {
                    *pos_in_seq = 0;
                    *skip_get_token = true; 
                    id = SyntaxId::inline_math;
                }
            }
            
        } else if(isTokenOfType(SyntaxElement::text_like, token)) {
            SyntaxNode& child = parent.lastChild();

            
            child.recomputeLength(token.column, token.char_count);
        } else {
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);

            id = SyntaxId::none;
        }

        return;
    case 2:
        // this step of the sequence is to return after the decoding of a reference or a block
        if(isTokenOfType(SyntaxElement::brace_ref_o, token)) {
            id = SyntaxId::reference;
            *pos_in_seq = 0;
            *skip_get_token = true;

        } else if(isTokenOfType(SyntaxElement::block_delim, token) && blockDelimiterIsValid(token)) {
             if(token.char_count > 0 && token.value[0] == '`') {
                if(token.char_count > 1) {
                    // SyntaxId::multiline_block;
                    // TODO add error: no multiline allowed in rich text
                    parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token, 
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
                    
                    SyntaxNode& child = parent.newChildFromToken(token, source);
            
                    (*pos_in_seq)--;

                } else {
                    *pos_in_seq = 0;
                    *skip_get_token = true;
                    id = SyntaxId::inline_block;
                }
            } else {
                if(token.char_count > 1) {
                    // SyntaxId::multiline_math;
                    // TODO add error: no multiline allowed in rich text
                    parent.addDiagnostic(DiagnosticSeverity::error,
                                    DiagnosticId::unexpected_token, 
                                    m_file_src, token.line, token.line, 
                                    token.column, token.column + 
                                    token.char_count);
                    
                    SyntaxNode& child = parent.newChildFromToken(token, source);
            
                    (*pos_in_seq)--;
                    
                } else {
                    *pos_in_seq = 0;
                    *skip_get_token = true; 
                    id = SyntaxId::inline_math;
                }
            }

        } else if(isTokenOfType(SyntaxElement::text_like, token)) {
            
            SyntaxNode& child = parent.newChildFromToken(token, source);
            
            (*pos_in_seq)--;
        } else {
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);

            id = SyntaxId::none;
        }

        return;

    default:
        id = SyntaxId::none;

        parent.addDiagnostic(DiagnosticSeverity::error,
                             DiagnosticId::unexpected_token,
                             m_file_src, token.line, token.line, 
                             token.column, token.column + token.char_count);
        return;
    }
}

void decodeMultilineBlock(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token) {
    m_check_pointers(skip_get_token, escape_next_token, pos_in_seq);
    
    const token_type_t delim_type = id == SyntaxId::multiline_block ? 
        k_token_type_backtick_delim : 
        k_token_type_math_delim     ;
    
    SemanticTokenDefinition delim_def = id == SyntaxId::multiline_block ? 
        blck_delim_ml :
        math_delim_ml ;

    SemanticTokenDefinition body_def = id == SyntaxId::multiline_block ? 
        str_blck :
        str_math ;

    switch (*pos_in_seq)
    {
    case 0:
        //Multiline block must start with delim_type
        if(token.type != delim_type) {
            id = SyntaxId::none;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
        } else {
            parent = token_t{
                .line = token.line, 
                .column = token.column, 
                .char_count = 0
            };

            (void)parent.newChildFromToken(token, delim_def);

            (*pos_in_seq)++;
        }
        break;
    case 1:
        //Multiline block ends with delim_type
        if(token.type == delim_type) {
            (void)parent.newChildFromToken(token, delim_def);

            id = SyntaxId::none;
            *pos_in_seq = 0;

        } else if(isTokenOfType(SyntaxElement::text_like, token)) {
            
            (void)parent.newChildFromToken(token, body_def);

            (*pos_in_seq)++;
        } else {
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);

            id = SyntaxId::none;
        }
    case 2:
        //Multiline block ends with delim_type
        if(token.type == delim_type) {
            (void)parent.newChildFromToken(token, delim_def);

            id = SyntaxId::none;
            *pos_in_seq = 0;

        } else if(isTokenOfType(SyntaxElement::text_like, token)) {
            SyntaxNode& child = parent.lastChild();

            
            child.recomputeLength(token.column, token.char_count);
        } else {
            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);

            id = SyntaxId::none;
        }
    default:
        break;
    }
}

void decodeInlineBlock(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token) {
    m_check_pointers(skip_get_token, escape_next_token, pos_in_seq);
    
    const token_type_t delim_type = id == SyntaxId::inline_block ? 
        k_token_type_backtick_delim : 
        k_token_type_math_delim     ;
    
    SemanticTokenDefinition& delim_def = id == SyntaxId::inline_block ? 
        blck_delim :
        math_delim ;

    SemanticTokenDefinition& body_def = id == SyntaxId::inline_block ? 
        str_blck_inline :
        str_math_inline ;

    switch (*pos_in_seq)
    {
    case 0:
        //Multiline block must start with delim_type
        if(token.type != delim_type) {
            id = SyntaxId::none;

            if(!isTokenOfType(SyntaxElement::new_line, token))
                parent.lastChild().addDiagnostic( 
                                     DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
        } else {
            (void)parent.lastChild().newChildFromToken(token, delim_def);

            (*pos_in_seq)++;
        }
        break;
    case 1:
        //Multiline block ends with delim_type
        if(token.type == delim_type) {
            (void)parent.lastChild().newChildFromToken(token, delim_def);

            id = SyntaxId::rich_text;
            *pos_in_seq = 2;

        } else if(isTokenOfType(SyntaxElement::text_like, token)) {
            
            (void)parent.lastChild().newChildFromToken(token, body_def);

            (*pos_in_seq)++;
        } else {
            if(!isTokenOfType(SyntaxElement::new_line, token)) {
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
            } else {
                //TODO missing delimiter at end of line for inline block
            }

            id = SyntaxId::none;
        }
    case 2:
        //Multiline block ends with delim_type
        if(token.type == delim_type) {
            (void)parent.lastChild().newChildFromToken(token, delim_def);

            id = SyntaxId::rich_text;
            *pos_in_seq = 2;

        } else if(isTokenOfType(SyntaxElement::text_like, token)) {
            SyntaxNode& child = parent.lastChild().lastChild();

            
            child.recomputeLength(token.column, token.char_count);
        } else {
            if(!isTokenOfType(SyntaxElement::new_line, token)) {
                parent.addDiagnostic(DiagnosticSeverity::error,
                                     DiagnosticId::unexpected_token, 
                                     m_file_src, token.line, token.line, 
                                     token.column, token.column + 
                                     token.char_count);
            } else {
                //TODO missing delimiter at end of line for inline block
            }
        }
    default:
        break;
    }
}


int getKeywordCount() {
    return sizeof(keywords) / sizeof(keywords[0]);
}

int getAttributeCount() {
    return sizeof(attributes) / sizeof(attributes[0]);
}

SemanticTokenDefinition* tokenInKeywords(SemanticTokenDefinition* kws, int keyword_count, token_t t) {
    m_check_pointer(kws);
    
    if(t.type == k_token_type_pot_keyword || 
       t.type == k_token_type_text && t.line == 1 && t.column == 1) {
        
        for(int i = 0; i < keyword_count; i++) {
                        
            SemanticTokenDefinition kw = kws[i];
            
            int j = 0;
            bool is_keyword = t.value[j] == kw.name[j];
            
            while(is_keyword && j < k_max_value_str_size && t.value[j] != '\0' && kw.name[j] != '\0') {
                j++;
                is_keyword = t.value[j] == kw.name[j];
            }
            
            if(is_keyword) 
                return &kws[i];
            
        }
    }

    return NULL;
}

bool tokenIsKeyword(token_t t, SemanticTokenDefinition** kw) {
    *kw = tokenInKeywords(keywords, getKeywordCount(), t);
    
    return *kw != NULL;
}

bool tokenIsAttribute(token_t t, SemanticTokenDefinition** kw) {
    m_check_pointer(kw);

    *kw = tokenInKeywords(attributes, getAttributeCount(), t);

    return *kw != NULL;
}

bool isTokenOfType(SyntaxElement type, token_t t) {
    SemanticTokenDefinition* kw = NULL;

    switch (type)
    {
    case SyntaxElement::unknown:
        return t.type == k_token_type_invalid;
    case SyntaxElement::EOS:
        return false;
    case SyntaxElement::new_line:
        return t.type == k_token_type_new_line;
    case SyntaxElement::indent:
        return t.type == k_token_type_sindent || t.type == k_token_type_tindent;
    case SyntaxElement::keyword:
        return tokenIsKeyword(t, &kw);
    case SyntaxElement::column:
        return t.type == k_token_type_column;
    case SyntaxElement::dot:
        return t.type == k_token_type_dot;
    case SyntaxElement::text:
        return t.type == k_token_type_text;
    case SyntaxElement::text_like:
        return t.type == k_token_type_text           || 
               t.type == k_token_type_number         || 
               t.type == k_token_type_pot_keyword    ||
               t.type == k_token_type_column         ||
               t.type == k_token_type_pot_ref_name   || 
               t.type == k_token_type_pot_ref_num    || 
               t.type == k_token_type_brace_c        ||
               t.type == k_token_type_brace_ref_c    ||
               t.type == k_token_type_brace_o        ||
               t.type == k_token_type_brace_ref_o    ||
               t.type == k_token_type_dot            ||
               t.type == k_token_type_space          ||
               t.type == k_token_type_backtick_delim ||
               t.type == k_token_type_math_delim     ||
               t.type == k_token_type_escape_char    ||
               t.type == k_token_type_hyphen;
    case SyntaxElement::hyphen:
        return t.type == k_token_type_hyphen;
    case SyntaxElement::number:
        return t.type == k_token_type_number;
    case SyntaxElement::attribute:
        return tokenIsAttribute(t, &kw);
    case SyntaxElement::brace_ref_o:    
        return t.type == k_token_type_brace_ref_o;
    case SyntaxElement::brace_ref_c:
        return t.type == k_token_type_brace_ref_c;
    case SyntaxElement::ref_name:
        return t.type == k_token_type_pot_ref_name ||
               t.type == k_token_type_pot_ref_num;
    case SyntaxElement::ref_num:
        return t.type == k_token_type_pot_ref_num;
    case SyntaxElement::space:
        return t.type == k_token_type_space;
    case SyntaxElement::block_delim:
        return t.type == k_token_type_backtick_delim  ||
               t.type == k_token_type_math_delim;
    case SyntaxElement::escape:
        return t.type == k_token_type_escape_char;
    case SyntaxElement::any:
        return true;
    default:
        return false;
    }
}