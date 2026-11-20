#include "node_decoders.h"

NodeDecoder::~NodeDecoder() = default;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Entity /////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Entity::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    const std::string_view &entity_kw_modifier = ctx.currentView();
    definition.setModifier(entity_kw_modifier);

    // sequence start
    // kw already set for the length
    ctx.next();

    // optional alias
    if (ctx.isCurrentToken(TokenType::Fence, "{"))
    {
        // decode alias
        Alias *alias = addChild<Alias>(entity_kw_modifier);

        alias->decode(ctx, ast_ctx);
    }

    // optional id
    if (ctx.currentToken().type == TokenType::Punctuation && ctx.currentView() == "-")
    {
        // decode id
        EntityId *id = addChild<EntityId>(entity_kw_modifier);

        id->decode(ctx, ast_ctx);
    }

    //  space(s) after entity declaraion are mandatory before optional title or description start
    if (ctx.currentToken().type == TokenType::Spaces)
    {
        ctx.next();

        int text_tokens = 0;
        bool found_eol = ctx.peek(text_tokens + 1).type == TokenType::NewLine;
        bool found_eot = (ctx.peek(text_tokens + 1).type == TokenType::Punctuation && ctx.peekView(text_tokens + 1) == ":");

        while (!found_eol && !found_eot)
        {
            text_tokens++;
            
            found_eol = ctx.peek(text_tokens + 1).type == TokenType::NewLine;
            found_eot = (ctx.peek(text_tokens + 1).type == TokenType::Punctuation && ctx.peekView(text_tokens + 1) == ":");
        }

        if (found_eot)
        {
            // decode title
            // TODO handle error where text tokens == 0 -> a column withou any title
            Title *title = addChild<Title>(text_tokens);

            title->decode(ctx, ast_ctx);
        }

        if (found_eol || ctx.currentToken().type != TokenType::NewLine)
        {
            // decode descripion
            // TODO handle case where text_token is 0 -> nothing special happens here I guess
            // TODO handle case where no space is inserted after the ":" of the title
            RichText *rich_text = addChild<RichText>(ctx.currentToken(), nullptr, SemanticTokenModifierHandler{{"documentation", "entity"}});

            rich_text->decode(ctx, ast_ctx);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Alias //////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Alias::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    // TODO: add diagnostic if not starting with a fence
    if (ctx.isCurrentToken(TokenType::Fence, "{"))
    {
        addChild<SyntaxNode>(ctx.currentToken(), "fence", ModifierList{"alias", "open", parent_keyword});
        ctx.next();
    }

    if (ctx.isCurrentToken(TokenType::Text) || ctx.isCurrentToken(TokenType::Number) || ctx.isCurrentToken(TokenType::Punctuation, "_") || ctx.isCurrentToken(TokenType::Punctuation, "-"))
    {
        SyntaxNode *alias = addChild<SyntaxNode>(ctx.currentToken(), "identifier", ModifierList{"alias", "declaration", parent_keyword});

        while (ctx.isCurrentToken(TokenType::Text) || ctx.isCurrentToken(TokenType::Number) || ctx.isCurrentToken(TokenType::Punctuation, "_") || ctx.isCurrentToken(TokenType::Punctuation, "-"))
        {
            alias->operator+=(ctx.currentToken());
            ctx.next();
        }
    }
    // TODO: add diagnostic if not ending with a fence
    if (ctx.isCurrentToken(TokenType::Fence, "}"))
    {
        addChild<SyntaxNode>(ctx.currentToken(), "fence", ModifierList{"alias", "close", parent_keyword});
        ctx.next();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// EntityId ///////////////////////////////////////////////////////////////////////////////////////////////////////////
void EntityId::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    // TODO: add diagnostic if not starting with a fence
    if (ctx.isCurrentToken(TokenType::Punctuation, "-"))
    {
        addChild<SyntaxNode>(ctx.currentToken(), "delimiter", ModifierList{"id", parent_keyword});
        ctx.next();
    }

    // TODO: add diagnostics if not a number
    if (ctx.isCurrentToken(TokenType::Number))
    {
        addChild<SyntaxNode>(ctx.currentToken(), "identifier", ModifierList{"id", "declaration", parent_keyword});
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Title //////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Title::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    SyntaxNode *title = addChild<SyntaxNode>(ctx.currentToken(), "text", ModifierList{"entity", "title"});

    //TODO check if number_of_title_tokens > 0, do not allow empty titles
    ctx.advance(number_of_title_tokens);

    title->operator+=(ctx.currentToken());

    ctx.next();

    if(ctx.isCurrentToken(TokenType::Punctuation, ":")) {
        addChild<SyntaxNode>(ctx.currentToken(), "delimiter", ModifierList{"entity", "title"});
        ctx.next();
    } else {
    //TODO sanity check failed add diagnostic
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TagList ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO YOU ARRIVED HERE
void TagList::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    //TODO add sanity checks if current and next are "%" and "[" respectively
    SyntaxNode *fence_o = addChild<SyntaxNode>(ctx.currentToken(), "fence", ModifierList{"tag-list", "open"});
    ctx.next();

    fence_o->operator+=(ctx.currentToken());
    ctx.next();

    //consume optional spaces
    if (ctx.currentToken().type == TokenType::Spaces)
        ctx.next();


    while(!ctx.isCurrentToken(TokenType::Fence, "]") && !ctx.isEOF()){
        PathDecoder *p_dec = addChild<PathDecoder>(ctx.currentToken(), nullptr, SemanticTokenModifierHandler{{"filepath", "tag-list"}});
        p_dec->decode(ctx, ast_ctx);

        //consume optional spaces
        if (ctx.currentToken().type == TokenType::Spaces)
            ctx.next();

        //add optional "," delimiter
        if(ctx.isCurrentToken(TokenType::Punctuation, ","))
        {
            addChild<SyntaxNode>(ctx.currentToken(), "delimiter", ModifierList{"tag-list"});
            ctx.next();
        }

        //consume optional spaces
        if (ctx.currentToken().type == TokenType::Spaces)
            ctx.next();

    }

    if(ctx.isCurrentToken(TokenType::Fence, "]") )
    {
        addChild<SyntaxNode>(ctx.currentToken(), "fence", ModifierList{"tag-list", "close"});
        ctx.next();
    }

    

    do
    {
        *this += ctx.currentToken();
        // adapt end of while to if next is RichText end condition (reference start, new line, more to come in the future)
    } while (ctx.currentToken().type != TokenType::NewLine && ctx.next());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PathDecoder ////////////////////////////////////////////////////////////////////////////////////////////////////////
void PathDecoder::decode(ParserContext &ctx, ASTContext &ast_ctx)
{
    if (ctx.isCurrentToken(TokenType::Punctuation, "."))
    {
        modifiers.setModifier("relative");

        SyntaxNode *prefix = addChild<SyntaxNode>(ctx.currentToken(), "prefix", modifiers);
        int dot_count = 1;
        ctx.next();

        while (ctx.isCurrentToken(TokenType::Punctuation, "."))
        {
            prefix->operator+=(ctx.currentToken());
            dot_count++;
            ctx.next();
        }
    }
    else if (ctx.isCurrentToken(TokenType::Punctuation, "~"))
    {
        modifiers.setModifier("absolute");

        addChild<SyntaxNode>(ctx.currentToken(), "prefix", modifiers);
        ctx.next();
    }

    while (ctx.isCurrentToken(TokenType::Text) || ctx.isCurrentToken(TokenType::Number) || ctx.isCurrentToken(TokenType::Punctuation, "_"))
    {
        file_offset_t link_element_start = ctx.currentToken().offset;
        file_offset_t link_element_length = ctx.currentToken().length;

        ctx.next();
        while (ctx.isCurrentToken(TokenType::Text) || ctx.isCurrentToken(TokenType::Number) || ctx.isCurrentToken(TokenType::Punctuation, "_"))
        {
            link_element_length = ctx.currentToken().length + ctx.currentToken().offset - link_element_start;
            ctx.next();
            std::cout << ctx.currentView() << std::endl;
        }

        OffsetRange link_element_range = OffsetRange{.offset = link_element_start, .length = link_element_length};

        if (ctx.isCurrentToken(TokenType::Punctuation, "."))
        {
            addChild<SyntaxNode>(link_element_range, "directory", modifiers);
            addChild<SyntaxNode>(ctx.currentToken(), "delimiter", modifiers);
            ctx.next();
        }
        else
        {
            addChild<SyntaxNode>(link_element_range, "resource", modifiers);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Attribute //////////////////////////////////////////////////////////////////////////////////////////////////////////
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


