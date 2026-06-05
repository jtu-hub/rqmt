#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <fcntl.h>
#include <io.h>

#include "lsp.h"
#include "util.h"
#include "workspace.h"
#include "dispatcher.h"

using json = nlohmann::json;

Message readMessage(Workspace& wsp) {
    std::string line;
    Message message;

    wsp.log << "Decoding New Message:";

    while (std::getline(std::cin, line)) {
        wsp.log << line;

        if (line == "\r" || line.empty())
            break;

        message.addHeader(line);
    }

    if (std::cin.fail() && !std::cin.eof()) {
        wsp.log << "Error reading headers: " << std::strerror(errno);
        return Message();
    }

    if(message.empty()) return message;

    std::string body(message.getContentLength(), '\0');
    std::cin.read(&body[0], message.getContentLength());
    wsp.log << body;

    message.addBody(body);

    return message;
}

//TODO: rply class, refactor,
void handleMessage(const Message& msg, Workspace& wsp) {
    std::string method = msg.getMethod();

    wsp.log << "handleMessage: " + method;

    const auto& handlers = getRequestCallbackRegistry();

    auto it = handlers.find(method);
    if (it != handlers.end()) {
        it->second(msg, wsp);
    }
}

int main() {
    // Disable automatic newline translation
    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);
    std::cin.tie(nullptr);

    _setmode(_fileno(stdout), _O_BINARY);

    Workspace wsp;

    while (true) {
        Message msg = readMessage(wsp);

        if(!msg.empty()) handleMessage(msg, wsp);
    }
}
