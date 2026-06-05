#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "util.h"

#define k_max_buffer_size 4096

int getFlattenIndex(int i, int j, int dim_i, int dim_j) {
    if(i >= dim_i || i < 0) {
        m_index_out_of_bound(i, dim_i);
    }

    if(j >= dim_j || i < 0) {
        m_index_out_of_bound(j, dim_j);
    }

    return i * dim_j + j;

}

char* dsprintf(const char* formatted_string, ...) {
    va_list args; 
    va_start(args, formatted_string);

    char* buffer = (char*)malloc(sizeof(char) * k_max_buffer_size);

    m_check_pointer(buffer);

    int written_char_count = vsnprintf(buffer, k_max_buffer_size, formatted_string, args);
        
    va_end(args);

    if(written_char_count < 0) {
        m_error_unhandled();
    }
    
    buffer = (char*)realloc(buffer, sizeof(char) * (written_char_count + 1)); //TODO canocical buffer handling new_buffer, if new_buffer != NULL continue else error handling

    m_check_pointer(buffer);

    return buffer;
}

char* getDiagnosticSource(const char* file, int line) {
    const char* filename = strrchr(file, '/');
    
    if (!filename) {
        filename = strrchr(file, '\\');
    }
    if (!filename) {
        filename = file;
    } else {
        filename += 1; // Skip the '/' or '\'
    }

    return dsprintf("rqmtparser >> %s:%d", filename, line);
}

bool streq(const char* str_1, const char* str_2) {
    return strcmp(str_1, str_2) == 0;
}

void parseColumnSeparatedInts(const char* encoded_str, int vals[], int elements_to_decode) {
    char *c = (char*)encoded_str;
    int currently_decoding = 0;

    while(*c != '\0' && currently_decoding < elements_to_decode) {
        if(*c == ':') {
            currently_decoding++;
        } else {
            int val = (int)(*c) - (int)('0');
            
            if(val < 0 || 9 < val) {
                //TODO proper error handling
                printf("string contains chars that are not digits\n");
                exit(EXIT_FAILURE);
            }
    
            vals[currently_decoding] *= 10;
            vals[currently_decoding] += val;
        }
        
        c++;
    }

    if(currently_decoding == elements_to_decode) {
        //TODO proper error handling
        printf("string format is invalid\n");
        exit(EXIT_FAILURE);
    }
}