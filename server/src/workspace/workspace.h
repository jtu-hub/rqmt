#pragma once

#include <unordered_map>

#include "rqmtfile.hpp"
#include "logger.h"

class Workspace {
    private:
        
    public:
        std::unordered_map<std::string, RqmtFile*> files;
        // Logger log;

        Workspace();
        void openRqmtFile(std::string uri, std::string text);
};