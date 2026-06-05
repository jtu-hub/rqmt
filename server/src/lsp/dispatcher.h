#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include "lsp.h"
#include "workspace.h"

class Message;

using RequestCallbackHandler = std::function<void(const Message&, Workspace&)>;
using RequestMethod = std::string;

// Accessor, not the map itself
const std::unordered_map<RequestMethod, RequestCallbackHandler>& getRequestCallbackRegistry();