#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include <io.h>

class Logger : public std::ofstream {
    private:
        const int padded_log_type_width = 12;
    public:
        Logger(const std::string& file_name);
        Logger(const char* file_name);
        template<typename T>
        void log(const std::string& log_type, const T& message);

        template<typename T>
        Logger& operator<<(const T& message) {
            log("INFO", message);
            return *this;
        }

        // Overload for std::ostream manipulators (e.g., std::endl)
        Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
            manip(*this);
            return *this;
        }
};

#include "logger.tpp"