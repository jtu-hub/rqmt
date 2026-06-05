#pragma once

void error_nullptr(const char* ptr_name, const char* file_name, 
                   const int line_number);

void error_index_out_of_bound(int index, int size, const char* index_name, 
                              const char* file_name, const int line_number);

void error_generic(const char* message, const char* file_name, 
                   const int line_number);