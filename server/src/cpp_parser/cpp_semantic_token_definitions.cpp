#include <vector>
#include <string>

extern "C" {
    #include "util.h"
}

#include "cpp_semantic_token_definitions.h"

constexpr std::size_t tk_mdf_count = static_cast<std::size_t>(SemanticTokenModifier::count);

constexpr std::size_t tk_count = static_cast<std::size_t>(SemanticTokens::count);

const ModifierLegendArray tokenModifierLegend = [] {
    ModifierLegendArray arr{};

    arr[static_cast<std::size_t>(SemanticTokenModifier::none)]         = "none";
    arr[static_cast<std::size_t>(SemanticTokenModifier::req)]          = "req";
    arr[static_cast<std::size_t>(SemanticTokenModifier::fr)]           = "fr";
    arr[static_cast<std::size_t>(SemanticTokenModifier::br)]           = "br";
    arr[static_cast<std::size_t>(SemanticTokenModifier::nfr)]          = "nfr";
    arr[static_cast<std::size_t>(SemanticTokenModifier::ur)]           = "ur";
    arr[static_cast<std::size_t>(SemanticTokenModifier::sr)]           = "sr";
    arr[static_cast<std::size_t>(SemanticTokenModifier::attr)]         = "attribute";
    arr[static_cast<std::size_t>(SemanticTokenModifier::author)]       = "author";
    arr[static_cast<std::size_t>(SemanticTokenModifier::title)]        = "title";
    arr[static_cast<std::size_t>(SemanticTokenModifier::desc)]         = "description";
    arr[static_cast<std::size_t>(SemanticTokenModifier::date)]         = "date";
    arr[static_cast<std::size_t>(SemanticTokenModifier::type)]         = "type";
    arr[static_cast<std::size_t>(SemanticTokenModifier::id)]           = "id";
    arr[static_cast<std::size_t>(SemanticTokenModifier::unquoted)]     = "unquoted";
    arr[static_cast<std::size_t>(SemanticTokenModifier::backtick)]     = "backtick";
    arr[static_cast<std::size_t>(SemanticTokenModifier::separator)]    = "separator";
    arr[static_cast<std::size_t>(SemanticTokenModifier::colon)]        = "colon";
    arr[static_cast<std::size_t>(SemanticTokenModifier::alias)]        = "alias";
    arr[static_cast<std::size_t>(SemanticTokenModifier::reference)]    = "reference";
    arr[static_cast<std::size_t>(SemanticTokenModifier::brace)]        = "brace";
    arr[static_cast<std::size_t>(SemanticTokenModifier::delimiter)]    = "delimiter";
    arr[static_cast<std::size_t>(SemanticTokenModifier::block)]        = "block";
    arr[static_cast<std::size_t>(SemanticTokenModifier::multiline)]    = "multiline";
    arr[static_cast<std::size_t>(SemanticTokenModifier::inline_token)] = "inline";
    arr[static_cast<std::size_t>(SemanticTokenModifier::math)]         = "math";

    return arr;
}();

const TokenLegendArray tokenLegend = [] {
    TokenLegendArray arr{};

    arr[static_cast<std::size_t>(SemanticTokens::keyword)]     = "keyword";
    arr[static_cast<std::size_t>(SemanticTokens::meta)]        = "meta";
    arr[static_cast<std::size_t>(SemanticTokens::string)]      = "string";
    arr[static_cast<std::size_t>(SemanticTokens::punctuation)] = "punctuation";
    arr[static_cast<std::size_t>(SemanticTokens::source)]      = "source";
    arr[static_cast<std::size_t>(SemanticTokens::space)]       = "space";

    return arr;
}();

const TokenLegendArray& getTokenLegend() {
    return tokenLegend;
}

const ModifierLegendArray& getTokenModifiersLegend() {
    return tokenModifierLegend;
}