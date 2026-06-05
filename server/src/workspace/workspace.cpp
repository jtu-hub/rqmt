#include "workspace.h"
#include "workspace_path.h" //Note: add this file and set it to your root workspace

Workspace::Workspace() : log(WORKSPACE_PATH "test\\test.log") {};

void Workspace::openRqmtFile(std::string uri, std::string text) {
    auto finder = files.find(uri);

    if(finder == files.end()) {
        log << "Open new file, set as active file: " << uri;

        files.insert(std::pair<std::string, RqmtFile*>(uri, new RqmtFile(uri, text)));
    }
}