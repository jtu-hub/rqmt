#pragma once

#define m_check_pointer(p)                     \
    if (!(p))                                  \
    {                                          \
        error_nullptr(#p, __FILE__, __LINE__); \
    };

#define m_check_pointers(...) /*TODO: find better impl*/ \
    do                                                   \
    {                                                    \
        const void *args[] = {__VA_ARGS__};              \
        const int argc = sizeof(args) / sizeof(args[0]); \
        for (int i = 0; i < argc; i++)                   \
        {                                                \
            m_check_pointer(args[i]);                    \
        }                                                \
    } while (0);

#define m_index_out_of_bound(i, size)                              \
    do                                                             \
    {                                                              \
        error_index_out_of_bound(i, size, #i, __FILE__, __LINE__); \
    } while (0);

#define m_error_generic(str) error_generic(str, __FILE__, __LINE__)

#define m_error_unhandled() m_error_generic("UNHANDLED ERROR!")

void error_nullptr(const char *ptr_name, const char *file_name,
                   const int line_number);

void error_index_out_of_bound(int index, int size, const char *index_name,
                              const char *file_name, const int line_number);

void error_generic(const char *message, const char *file_name,
                   const int line_number);