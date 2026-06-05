#pragma once

#include <string>

#define m_file_src getCodeSource(__FILE__, __LINE__)

/**
 * \brief Generates a formatted diagnostic source string for error reporting.
 * \param file  The source file path from which the diagnostic originates.
 * \param line  The line number in the source file where the diagnostic originates.
 * \return A statically allocated string containing the formatted diagnostic source in the format
 *         "rqmtparser:<filename>:<line>".
 */
std::string getCodeSource(const char* file, int line);

bool parseInt(std::string_view s, int& value);