#include "node_decoders.h"

NodeDecoder::~NodeDecoder() = default;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Entity /////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    const std::string_view &entity_kw_modifier = ctx.tokenView();
    definition.setModifier(entity_kw_modifier);

    // sequence start
    // kw already set for the length
    ctx.next();

    // optional alias
    if (ctx.currentToken().type == TokenType::Fence && ctx.tokenView() == "{")
    {
        // decode alias
    }

    // optional id
    if (ctx.currentToken().type == TokenType::Punctuation && ctx.tokenView() == "-")
    {
        // decode id
    }

    // space(s) after entity declaraion are mandatory before optional title or description start
    if (ctx.currentToken().type == TokenType::Spaces)
    {
        ctx.next();

        int text_tokens = 0;
        bool found_eol = ctx.peek(text_tokens + 1).type == TokenType::NewLine;
        bool found_eot = (ctx.peek(text_tokens + 1).type == TokenType::Punctuation && ctx.peekView(text_tokens + 1) == ":");

        while (!found_eol || !found_eot)
        {
            text_tokens++;

            found_eol = ctx.peek(text_tokens + 1).type == TokenType::NewLine;
            found_eot = (ctx.peek(text_tokens + 1).type == TokenType::Punctuation && ctx.peekView(text_tokens + 1) == ":");
        }

        if (found_eot)
        {
            // decode title
            // handle error where text tokens == 0 -> a column withou any title
        }

        if (found_eol || ctx.currentToken().type != TokenType::NewLine)
        {
            // decode descripion
            // handle case where text_token is 0 -> nothing special happens here I guess
            // handle case where no space is inserted after the ":" of the title
        }
    }

    // handle final new line(?)
    do
    {
        *this += ctx.currentToken();
        // adapt end of while to if next is RichText end condition (reference start, new line, more to come in the future)
    } while (ctx.currentToken().type != TokenType::NewLine && ctx.next());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Entity /////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Attribute::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    do
    {
        *this += ctx.currentToken();

    } while (ctx.currentToken().type != TokenType::NewLine && ctx.next());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RichText ///////////////////////////////////////////////////////////////////////////////////////////////////////////
void RichText::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    do
    {
        *this += ctx.currentToken();
        // adapt end of while to if next is RichText end condition (reference start, new line, more to come in the future)
    } while (ctx.currentToken().type != TokenType::NewLine && ctx.next());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Root ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
