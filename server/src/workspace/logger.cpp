#include "logger.h"
#include "version.h"


Logger::Logger(const std::string& file_name) : std::ofstream(file_name, std::ios::out | std::ios::app) {
    if (!is_open()) {
        throw std::runtime_error("Failed to open log file: " + file_name);
    }
    
    std::ofstream& base = *this;

    base  << "================================================================================\n";
    *this << "// --- Main Stratup ------------------------------------------------";
    *this << "////////////////////////////////////////////////////////////////////";
    *this << "RQMT Language Protocol Server";
    *this << "(c) rqmt by Jonas T. Ulbrich - Version: " << k_version_str;
}

Logger::Logger(const char* file_name) : std::ofstream(file_name, std::ios::out | std::ios::app) {
    if (!is_open()) {
        throw std::runtime_error("Failed to open log file: " + std::string(file_name));
    }

    std::ofstream& base = *this;

    base  << "================================================================================\n";
    *this << "// --- Main Stratup ------------------------------------------------";
    *this << "////////////////////////////////////////////////////////////////////";
    *this << "RQMT Language Protocol Server";
    *this << "(c) rqmt by Jonas T. Ulbrich - Version: " << k_version_str;
}