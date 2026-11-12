#include "parser.h"

enum class ParserMainState
{
    Invalid = -1,
    Text,
};

struct Indent
{
    int level = 0;
    int depth = 0;

    int level_change = 0;
    bool irregular_depth = false;
};

class ParserContext
{
    AST &ast;
    SyntaxNode *parent;

    const LineOffsets &offs;
    const Tokens &tks;
    const std::string &input;
    int current_token_idx = -1;

public:
    Indent indent{0};
    ParserMainState main_state;
    static constexpr const TokenDefinitions::Token sentinel_token = TokenDefinitions::Token{.type = TokenDefinitions::TokenType::Invalid};
    ParserContext(const std::string &input, AST &ast, const LineOffsets &offs, const Tokens &tks) : ast(ast),
                                                                                                    parent(ast.getRoot()),
                                                                                                    offs(offs),
                                                                                                    tks(tks),
                                                                                                    input(input) {}
    bool next()
    {
        if (current_token_idx + 1 < tks.size())
        {
            current_token_idx++;
            return true;
        }

        return false;
    }

    const TokenDefinitions::Token &peek(int peek_idx = 1) const
    {
        int look_at = current_token_idx + peek_idx;

        if (0 <= look_at && look_at < tks.size())
            return tks[look_at];
        else
            return sentinel_token;
    }

    bool advance(const int advance_offset)
    {
        if (current_token_idx + advance_offset < tks.size())
        {
            current_token_idx += advance_offset;
            return true;
        }
        else
        {
            return false;
        }
    }

    const TokenDefinitions::Token &currentToken() const { return tks[current_token_idx]; }

    std::string_view tokenView() const
    {
        const TokenDefinitions::Token &token = currentToken();

        return std::string_view(input).substr(
            token.offset,
            token.length);
    }
};

bool lineStart(const TokenDefinitions::Token &t, ParserContext &ctx)
{

    // handle indent
    if (t.type == TokenDefinitions::TokenType::Spaces)
    {
        if (!ctx.indent.depth)
        {
            ctx.indent.depth = t.length;
            ctx.indent.level = 1;
            ctx.indent.irregular_depth = false;
            ctx.indent.level_change = 1;
        }
        else
        {
            int new_indent_level = t.length / ctx.indent.depth;

            ctx.indent.level_change = new_indent_level - ctx.indent.level;

            ctx.indent.level = new_indent_level;
            ctx.indent.irregular_depth = static_cast<bool>(t.length % ctx.indent.depth);

            if (!ctx.advance(1))
                return false;
        }
    }

    // At line start branch off into different sequences:
    //  Text:
    //    - entity_keyword -> add a new syntax node
    //    - attribute_keyword -> add a new syntax node
    //    - other text -> add to previous node
    //  Modifier:
    //    - % -> if(peek(1) == [) add new syntax node else consider text
    //    - @ -> if(peek(1) == decorator keyword) add new syntax node else consider text
    //  Default:
    //    - consider text -> add to previous node
    const TokenDefinitions::Token &tt = ctx.currentToken();

    switch (tt.type)
    {
    case TokenDefinitions::TokenType::Text:
    {
    }
    case TokenDefinitions::TokenType::Modifier:
    {
    }
    case TokenDefinitions::TokenType::NewLine:
    {
    }
    default:
    }
}

void parse(const std::string &input, TokenizerResult res, AST &ast)
{
    ParserContext ctx(input, ast, res.line_offsets, res.tokens);

    while (ctx.next())
    {
        const TokenDefinitions::Token &t = ctx.currentToken();

        std::cout << "TOKEN VIEW `" << ctx.tokenView() << "`" << std::endl;
    }
}