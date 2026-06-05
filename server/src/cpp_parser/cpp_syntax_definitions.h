#pragma once

extern "C" {
    #include "c_tokenizer.h"
    #include "list.h"
};

#include "cpp_parser_util.h"
#include "cpp_semantic_token_definitions.h"

#define k_max_sequence_lenght 10

enum class SyntaxElement {
    unknown = -1,
    EOS, //end of sequence
    new_line,
    indent,
    keyword,
    brace_ref_o,
    brace_ref_c,
    block_delim,
    dot,
    column,
    text,
    text_like,
    hyphen,
    number,
    attribute,
    ref_name,
    ref_num,
    space,
    escape,
    any
};

enum class SyntaxId {
    none = 0,
    requirement_def,
    requirement_with_id_def,
    requirement_with_name_def,
    reference,
    requirement_description,
    attribute_def,
    rich_text,
    multiline_block,
    multiline_math,
    inline_math,
    inline_block,
    // count
    count,
};

bool tokenIsKeyword(token_t t, SemanticTokenDefinition** kw);

bool tokenIsAttribute(token_t t, SemanticTokenDefinition** kw);

bool isTokenOfType(SyntaxElement type, token_t t);

SyntaxNode& handleSequenceStart(SyntaxId& id, SyntaxNode& parent, const token_t& token, int* indent_level);

void decodeRequirement(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token, int* indent_level);

void decodeInlineTitleAndDescription(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token);

void decodeInlineReferenceName(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token);

void decodeReference(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token);

void decodeAttribute(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token);

void decodeRichText(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token);

void decodeMultilineBlock(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token);

void decodeInlineBlock(bool* skip_get_token, bool* escape_next_token, int* pos_in_seq, SyntaxId& id, SyntaxNode& parent, const token_t& token);

SemanticTokenDefinition& getValueBodyDefinition();

SemanticTokenDefinition& getDescriptionBodyDefinition();

SemanticTokenDefinition& getSourceDefinition();

SemanticTokenDefinition& getStrMathDefinition();

SemanticTokenDefinition& getStrBacktickDefinition();

void printTokenLegendJSON();

