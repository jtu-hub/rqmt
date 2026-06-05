#pragma once

#include <array>

#define k_max_number_modifiers 5

enum class SemanticTokens {
    root = -2,
    invalid,
    keyword,
    meta,
    string,
    punctuation,
    source,
    space,

    // COUNT
    count
};

enum class SemanticTokenModifier {
    none=0,
    
    // TOKEN MODIFIERS ---------------------------------------------------------
    ////////////////////////////////////////////////////////////////////////////
    req,
    fr,
    br,
    nfr,
    ur,
    sr,
    attr,
    author,
    title,
    reference,
    desc,
    date,
    type,
    id,
    unquoted,
    backtick,
    separator,
    colon,
    alias,
    brace,
    delimiter,
    block,
    math,
    multiline,
    inline_token,
    
    // ...
    ////////////////////////////////////////////////////////////////////////////
    // TOKEN MODIFIERS ---------------------------------------------------------

    // COUNT
    count
};

enum class SemanticTokenId {
    none = -2,
    root = -1,

    def_kw_start = 0,
    // REQUIREMENT TOKEN IDs ---------------------------------------------------
    ////////////////////////////////////////////////////////////////////////////
    kw_req = def_kw_start,
    kw_fr,
    kw_br,
    kw_nfr,
    kw_sr,
    kw_ur,

    // ...
    ////////////////////////////////////////////////////////////////////////////
    // REQUIREMENT TOKEN IDs ---------------------------------------------------

    def_kw_end,
    def_attr_start = def_kw_end,
    
    // ATTRIBUTES TOKEN IDs ----------------------------------------------------
    ////////////////////////////////////////////////////////////////////////////
    attr_author = def_attr_start,
    attr_date,
    attr_title,
    attr_description,
    attr_type,

    // ...
    ////////////////////////////////////////////////////////////////////////////
    // ATTRIBUTES TOKEN IDs ----------------------------------------------------
    
    def_attr_end,
    def_scopes_start = def_attr_end,
    
    // SCOPES TOKEN IDs --------------------------------------------------------
    ////////////////////////////////////////////////////////////////////////////
    multiline_scope = def_scopes_start,
    
    // ...
    ////////////////////////////////////////////////////////////////////////////
    // SCOPES TOKEN IDs --------------------------------------------------------
    
    def_scopes_end,
    def_other_start = def_scopes_end,

    // OTHER TOKEN IDs ---------------------------------------------------------
    ////////////////////////////////////////////////////////////////////////////
    value_body = def_other_start,
    value,
    math_multiline,
    backtick_multiline,
    math_inline,
    backtick_inline,
    description_body,
    description,
    title,
    column_attr,
    column_title,
    source_body,
    alias_brace,
    alias_value,
    reference_brace,
    reference_delim,
    reference_value,
    id_delim,
    id_number,
    block_delim,
    math_delim,
    block_delim_ml,
    math_delim_ml,
    space,

    // ...
    // OTHER TOKEN IDs ---------------------------------------------------------
    ////////////////////////////////////////////////////////////////////////////

    def_other_end,
    // COUNT
    count = def_other_start
};

#define m_st_is_keyword(id)   (SemanticTokenId::def_kw_start     <= (id) && (id) < SemanticTokenId::def_kw_end     )
#define m_st_is_attribute(id) (SemanticTokenId::def_attr_start   <= (id) && (id) < SemanticTokenId::def_attr_end   )
#define m_st_is_scope(id)     (SemanticTokenId::def_scopes_start <= (id) && (id) < SemanticTokenId::def_scopes_end )
#define m_st_is_other(id)     (SemanticTokenId::def_other_start  <= (id) && (id) < SemanticTokenId::def_other_end  )


using STDModifierArray    = std::array<SemanticTokenModifier, k_max_number_modifiers>;

class SemanticTokenDefinition {
    public:
        SemanticTokenId id;
        std::string name;
        SemanticTokens token;
        STDModifierArray modifiers;
};

using ModifierLegendArray = std::array<std::string, static_cast<std::size_t>(SemanticTokenModifier::count)>;
using TokenLegendArray    = std::array<std::string, static_cast<std::size_t>(SemanticTokens::count)>;

const TokenLegendArray& getTokenLegend();
const ModifierLegendArray& getTokenModifiersLegend();