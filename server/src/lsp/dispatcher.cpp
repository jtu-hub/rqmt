#include "dispatcher.h"
#include "version.h"
#include "lsp.h"

////////////////////////////////////////////////////////////////////////////////
/// ADD CALLBACK IMPLEMENTATIONS ///////////////////////////////////////////////

static void handleInitialize(const Message& msg, Workspace& wsp) {

    json capabilities = buildLSPCapabilities(std::vector<Capability>({
        Capability::SemanticTokens,
        Capability::Diagnostic,
        Capability::Definition,
    }));

    json res = {
        {"serverInfo", {
            {"name", "RQMT Language Server"},
            {"version", k_version_str}
        }},
        capabilities
    };

    Reply(msg.getId(), res).write();
}

static void handleSemanticTokens(const Message& msg, Workspace& wsp) {
    std::string uri = msg.getTextDocumentParameter("uri");
    
    auto finder = wsp.files.find(uri);

    if(finder == wsp.files.end()) {
        Reply().write();
        wsp.log << "Got request \"" << msg.getMethod() << "\" for " << uri << ": No open file to provide tokens";
        return;
    }

    json res = finder->second->getAST().getSemanticTokenData();

    Reply(msg.getId(), res).write();
}

static void handleOpenFile(const Message& msg, Workspace& wsp) {
    wsp.openRqmtFile(
        msg.getTextDocumentParameter("uri"),
        msg.getTextDocumentParameter("text")
    );
}

static void handleChangeFile(const Message& msg, Workspace& wsp) {
    std::string uri = msg.getTextDocumentParameter("uri");
    
    auto finder = wsp.files.find(uri);

    if(finder == wsp.files.end()) {
        Reply().write();
        wsp.log << "Got request \"" << msg.getMethod() << "\" for " << uri << ": No open file to track changes";
        return;
    }

    finder->second->applyChanges(msg.getContentChanges());
}

static void handleDiagnostics(const Message& msg, Workspace& wsp) {
    std::string uri = msg.getTextDocumentParameter("uri");
    
    auto finder = wsp.files.find(uri);

    if(finder == wsp.files.end()) {
        Reply().write();
        wsp.log << "Got request \"" << msg.getMethod() << "\" for " << uri << ": No open file to track changes";
        return;
    }

    json res = finder->second->getDiagnostics();

    Reply(msg.getId(), res).write();
}

static void handleDefinition(const Message& msg, Workspace& wsp) {
    std::string uri = msg.getTextDocumentParameter("uri");
    
    auto finder = wsp.files.find(uri);

    if(finder == wsp.files.end()) {
        Reply().write();
        wsp.log << "Got request \"" << msg.getMethod() << "\" for " << uri << ": No open file to track changes";
        return;
    }

    json res = finder->second->getGoToDefinition(msg.getTextLocation());

    wsp.log << res.dump();
    
    Reply(msg.getId(), res).write();
}

//...
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const std::unordered_map<RequestMethod, RequestCallbackHandler>&
getRequestCallbackRegistry()
{
    static const std::unordered_map<RequestMethod, RequestCallbackHandler> registry = {
        /// REGISTER CALLBACKS /////////////////////////////////////////////////
        {"initialize", handleInitialize},
        {"textDocument/semanticTokens/full", handleSemanticTokens},
        {"textDocument/didOpen", handleOpenFile},
        {"textDocument/didChange", handleChangeFile},
        {"textDocument/diagnostic", handleDiagnostics},
        {"textDocument/definition", handleDefinition}
        
        //...
        ////////////////////////////////////////////////////////////////////////
    };

    return registry;
}