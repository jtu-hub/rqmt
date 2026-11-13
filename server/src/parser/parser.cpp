#include "parser.h"
#include "parser_context.h"
#include "node_decoders.h"

bool decodeContent(ParserContext &ctx, ASTContext &ast_ctx)
{

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
    const Token &tt = ctx.currentToken();

    switch (tt.type)
    {
    case TokenType::Text:
    {
        const int idx_of_entity_kw = indexOf<entity_keywords>(ctx.tokenView());
        const int idx_of_attr_kw = indexOf<attribute_keywords>(ctx.tokenView());

        const bool is_entity = idx_of_entity_kw < entity_keywords.size();
        const bool is_attr = idx_of_attr_kw < attribute_keywords.size();

        if (false && is_entity)
        {
            // add new entity decoder here and set as parent, increase indent level
        }
        else if (false && is_attr)
        {
            // add new attribute decoder and set as parent, increase indent level
        }
        else
        {
            // get the last (or add) a rich text decoder set as parent
            if (!ast_ctx.currentParent()->isLastChildOfType<RichText>())
            {
                ast_ctx.currentParent()->addChild<RichText>(ctx.currentToken());
                // Note: we do not advance here, it simplifies the break logic in the decode function and does not
                //       require to add additional checks
            }

            ast_ctx.levelDown()->as<RichText>()->decode(ctx, ast_ctx);
        }

        break;
    }
    case TokenType::Modifier:
    {
        break;
    }
    case TokenType::NewLine:
    {
        break;
    }
    default:
        break;
    }

    ctx.main_state = ParserMainState::NewLine;

    return true;
}

void parse(const std::string &input, TokenizerResult res, AST &ast)
{
    ParserContext ctx(input, res.line_offsets, res.tokens);
    ASTContext ast_ctx(ast);

    while (ctx.next())
    {
        std::cout << "END OF CONTENT REACHED, STARTING NEW WITH: `" << ctx.tokenView() << "`" << std::endl;
        const Token &t = ctx.currentToken();

        switch (ctx.main_state)
        {
        case ParserMainState::NewLine:
            std::cout << "NewLine " << std::endl;

            ctx.main_state = ParserMainState::Indent;

            //!\ FALL THROUGH !!!
        case ParserMainState::Indent:
            std::cout << "Indent " << std::endl;
            if (t.type == TokenType::Spaces)
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
                }

                if (!ctx.advance(1))
                    throw std::runtime_error("TODO proper error handling");
            }

            ctx.main_state = ParserMainState::Content;

            //!\ FALL THROUGH !!!
        case ParserMainState::Content:
            std::cout << "Content" << std::endl;
            decodeContent(ctx, ast_ctx);
            //!\ FALL THROUGH !!!
        default:
            std::cout << std::endl;
            break;
        }
    }
}