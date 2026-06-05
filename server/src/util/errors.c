#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "errors.h"

void error_nullptr(const char* ptr_name, const char* file_name, 
                   const int line_number) {
    printf("%s:%d Pointer %s is NULL\n", file_name, line_number, ptr_name);
    exit(EXIT_FAILURE);
}

void error_index_out_of_bound(int index, int size, const char* index_name, const char* file_name, const int line_number) {
    printf("%s:%d Index %s=%d is out of bound for array of size %d\n", file_name, line_number, index_name, index, size);
    exit(EXIT_FAILURE);
}

void error_generic(const char* message, const char* file_name, const int line_number) {
    printf("%s:%d %s\n", file_name, line_number, message);
    exit(EXIT_FAILURE);
}