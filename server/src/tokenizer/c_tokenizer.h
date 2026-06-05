#pragma once

#include <stdio.h>
#include "c_token_definitions.h"
#include "list.h"

// Structure containing a collection of tokens
typedef struct Tokens {
    list_t* tokens;
    int token_count;
} tokens_t;

tokens_t tokenize(FILE* input);

tokens_t tokenizeLine(const char* input, int line_count);

void joinLastTokenToPrevious(token_t* previous_token, token_t* current_token, 
                             int* token_count);

void printTokensAsJSON(tokens_t tokens);

void printTokens(tokens_t tks);

token_t* getTokenFromNode(node_t* token);
