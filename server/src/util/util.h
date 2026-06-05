#pragma once

#include <string.h>
#include <stdbool.h>

#include "errors.h"

#define m_check_pointer(p) \
    if(!(p)) { \
        error_nullptr(#p, __FILE__, __LINE__); \
    };

#define m_check_pointers(...) /*TODO: find better impl*/ \
    do { \
        const void *args[] = {__VA_ARGS__}; \
        const int argc = sizeof(args) / sizeof(args[0]); \
        for (int i = 0; i < argc; i++) { \
            m_check_pointer(args[i]); \
        } \
    } while(0);


#define m_index_out_of_bound(i, size) \
    do { \
        error_index_out_of_bound(i, size, #i, __FILE__, __LINE__); \
    } while(0);

#define m_diag_src getDiagnosticSource(__FILE__, __LINE__)

#define m_error_generic(str) error_generic(str, __FILE__, __LINE__)

#define m_error_unhandled() m_error_generic("UNHANDLED ERROR!")

#define _m_str_helper(x) #x
#define m_str(x) _m_str_helper(x)

/**
 * \brief Returns the index of a flattened 2D array, e.g.: 
 *        int arr[dim_i][dim_j]; 
 *        int* p_arr = &arr[0]; 
 *        ==> arr[i][j] == p_arr[getFlattenIndex(i, j, dim_i, dim_j)] 
 * \param i         index of first dimention
 * \param j         index of second dimention
 * \param dim_i     size of first dimention
 * \param dim_j     size of second dimention
 * \return flattend index
 */
int getFlattenIndex(int i, int j, int dim_i, int dim_j);

/**
 * \brief Dynamic sprintf allocates the space for the resulting string with 
 *        formatting applied in a dynamically allocated buffer and restruns the 
 *        pointer to the output string.
 * \param formatted_string  A printf like formatted string
 * \param ...               Additional arguments that are inserted in the 
 *                          formatted string
 * \return The pointer to the dynamically allocated buffer, containing the 
 *         output string
 */
char* dsprintf(const char* formatted_string, ...);

/**
 * \brief Generates a formatted diagnostic source string for error reporting.
 * \param file  The source file path from which the diagnostic originates.
 * \param line  The line number in the source file where the diagnostic originates.
 * \return A statically allocated string containing the formatted diagnostic source in the format
 *         "rqmtparser:<filename>:<line>".
 */
char* getDiagnosticSource(const char* file, int line);

/**
 * \brief Checks if two strings are equal by means of strcmp, just to increase readability
 * \param str_x  the two strngs to compare
 * \return true if strcmp returns 0 else false -> true if equal false otherwise.
 */
bool streq(const char* str_1, const char* str_2);

/**
 * \brief Takes a string of #elements_to_decode integer numbers separated by 
 *        column ':' and returns these as parsed ints stored in the 
 *        vals[elements_to_decode] array
 */
void parseColumnSeparatedInts(const char* encoded_str, int vals[], int elements_to_decode);