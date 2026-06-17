#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>

#include "c_tokenizer.h"
#include "c_token_definitions.h"
#include "errors.h"
#include "util.h"
#include "list.h"

typedef enum TokenizerState {
    k_tokenizer_state_error,
    k_tokenizer_state_new_line = 0,
    k_tokenizer_state_new_token,
    k_tokenizer_state_add_char,
    k_tokenizer_state_EOF,

    //state count
    k_tokenizer_state_count
} tokenizer_state_t;


bool addCharToToken(char c, token_t* token) {
    m_check_pointer(token);

    //only add if charcount does not exceed max count
    if(token->char_count >= k_max_value_str_size) return false;

    token->value[token->char_count] = c;
    (token->char_count)++;

    return true;
}

void addNewToken(int token_line_number, int token_column, tokens_t* tokens) {
    m_check_pointers(tokens);

    token_t* new_token = (token_t*)calloc(1, sizeof(token_t));

    m_check_pointer(new_token);
    
    new_token->line       = token_line_number;
    new_token->column     = token_column;

    append(tokens->tokens, new_token);
    tokens->token_count += 1;
}

token_t* getCurrentToken(tokens_t* tokens) {
    m_check_pointer(tokens);

    return getNodeData(getTail(tokens->tokens));
}

void getPotentialCandidates(char c, tokens_t* tokens,
                            list_t* potential_candidates) {
    int td_count = 0;
    const token_definition_t* tds = getTokenDefinitions(&td_count);
    
    m_check_pointers(tokens, tds, potential_candidates);

    freeConstantData(potential_candidates);
    
    token_t* t = getCurrentToken(tokens);
    
    for(int i = 0; i < td_count; i++) {
        token_definition_t* td = (token_definition_t*)&tds[i];

        bool td_valid = td->depends_on != td->depends_non;
        bool td_depends_on_none = td->depends_on == k_token_type_none;
        bool td_depends_non_none = td->depends_non == k_token_type_none;

        bool td_depends_on_ok = t != NULL && td->depends_on == t->type;
        bool td_depends_non_ok = t != NULL && td->depends_non != t->type;
        
        if(td->isStartChar(c) && (td_depends_on_none || td_depends_on_ok) && (td_depends_non_none || td_depends_non_ok)) {
            push(potential_candidates, td);
            
            //if(td->length == 1) return; //token found
        }
    }
}

tokenizer_state_t foundEndOfToken(char c, bool* skip_char, tokens_t* tokens, 
                                  list_t* potential_candidates) {
    m_check_pointers(skip_char, tokens, potential_candidates);

    token_t* t = getCurrentToken(tokens);
    node_t* node = NULL;

    m_check_pointer(t); //we should never have NULL here...

    while(iter(potential_candidates, &node) != NULL) {
        token_definition_t* td = (token_definition_t*)getNodeData(node);
        
        m_check_pointer(td);

        int t_lenght = td->include_end_char ? t->char_count + 1 : t->char_count;
        
        bool td_is_match = 
            td->isEndChar(c) &&    
            (td->length == k_variable_token_length || td->length == t_lenght);

        bool td_is_potential = 
            !td->isEndChar(c) &&
            (td->length == k_variable_token_length || td->length > t_lenght); 
                
        if(td_is_match) {
            if(td->include_end_char) {
                addCharToToken(c, t);                
            }
            t->type = td->type;
            *skip_char = !td->include_end_char;
            return k_tokenizer_state_new_token;
        }

        if(!td_is_potential) {
            (void)pop(potential_candidates, &node);
        }
    }

    addCharToToken(c, t);
    
    if(potential_candidates->length == 0) {
        t->type = k_token_type_none;
        return k_tokenizer_state_new_token;
    }

    return k_tokenizer_state_add_char;
}

void joinTokens(token_t* previous_token, token_t* current_token, 
                             int* token_count) {
    m_check_pointers(previous_token, current_token, token_count);

    for(int i = 0; i < current_token->char_count; i++) {
        addCharToToken(current_token->value[i], previous_token);
    }

    previous_token->type = current_token->type;

    free(current_token);

    *token_count--;
}

tokenizer_state_t handleNewLine(char c, tokenizer_state_t current_decoder_state, 
                                tokens_t* tokens) {
    m_check_pointers(tokens);

    //get current token
    node_t* last_node = getTail(tokens->tokens);
    token_t* t = (token_t*)getNodeData(last_node);

    if(t == NULL) {
        return k_tokenizer_state_new_line;
    } else if(t->type == k_token_type_nreturn) {
        // Note: new lines are detected only with '\n'
        
        t->type = k_token_type_new_line;

        return k_tokenizer_state_new_line;
    }

    return current_decoder_state;
}

void addInitialNewLine(tokens_t* tokens) {
    
    m_check_pointers(tokens);

    addNewToken(-1, 0, tokens);
    
    getCurrentToken(tokens)->type = k_token_type_new_line;
}

void addEOF(int token_line_number, int token_column, tokens_t* tokens) {
    m_check_pointers(tokens);

    if(handleNewLine('\0', k_tokenizer_state_EOF, tokens) ==
       k_tokenizer_state_new_line) {
        token_line_number++;
        token_column=0;
    }

    addNewToken(token_line_number, token_column, tokens);
    
    getCurrentToken(tokens)->type = k_token_type_EOF;
}

tokens_t tokenizeLine(const char* input, int line_count) {
    m_check_pointer(input);

    int column_count = 0;
    tokenizer_state_t decoder_state = k_tokenizer_state_new_line;
    list_t token_candidates = {0};
    list_t* token_list = (list_t*)calloc(1, sizeof(list_t));
    tokens_t tokens = {token_list, 0};
    bool skip_getc = false;

    addInitialNewLine(&tokens);
    //loop throug chars to tokenize
    char c = input[column_count];
    
    while(c != '\0') {
        
        decoder_state = handleNewLine(c, decoder_state, &tokens);

        switch (decoder_state)
        {
        case k_tokenizer_state_new_line:
            //nothing to do in single line

            //!!! FALL THROUGH
        case k_tokenizer_state_new_token:
            skip_getc = false;
            
            getPotentialCandidates(c, &tokens, &token_candidates);
            
            addNewToken(line_count, column_count, &tokens);
            //!!! FALL THROUGH
        case k_tokenizer_state_add_char:
            
            decoder_state = foundEndOfToken(c, &skip_getc, &tokens, 
                                            &token_candidates);
            break;
        
        default:
            decoder_state = k_tokenizer_state_new_token;
            break;
        }

        //get next char
        if(!skip_getc) {
            column_count++;
            c = input[column_count];
        }
    }

    handleNewLine('\0', k_tokenizer_state_EOF, &tokens);

    return tokens;
}

void printTokenAsJSON(token_t token) {
    char * value = token.type == k_token_type_new_line ? 
                       "NEW_LINE" : token.type == k_token_type_EOF ? 
                           "END_OF_FILE" : token.value;
    printf(
        "{"
        "\"type\": %d,"
        "\"value\": \"%s\","
        "\"line\": %d,"
        "\"column\": %d,"
        "\"length\": %d"
        "}", 
        token.type, value, token.line, token.column, token.char_count
    );
}

void printTokensAsJSON(tokens_t tokens) {
    printf("{\"tokens\":[");

    node_t* token_node = NULL;

    while(iter(tokens.tokens, &token_node) != NULL) {
        printTokenAsJSON(*(token_t*)getNodeData(token_node));
        
        if(token_node != getTail(tokens.tokens)) printf(",");
    }
    printf("]}");
}

void printTokens(tokens_t tks) {
    printf("TOKENS:");
    
    node_t* node = NULL;
    
    while(iter(tks.tokens, &node) != NULL) {
        printf(" %02d", ((token_t*)(getNodeData(node)))->type);
    }

    printf("\n");
}

token_t* getTokenFromNode(node_t* token) {
    return (token_t*)(getNodeData(token));
}