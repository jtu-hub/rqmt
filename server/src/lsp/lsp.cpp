#include <iostream>

#include "lsp.h"
#include "cpp_semantic_token_definitions.h"
#include "cpp_util.h"

extern "C" {
    #include "util.h"
};

Reply::Reply(int id, json result, std::string jsonrpc)
{
    json body_temp = {
        {"jsonrpc", jsonrpc},
        {"id", id},
        {"result", result}
    };
    
    body = body_temp.dump(-1, ' ', false, json::error_handler_t::replace);

    headers.push_back("Content-Type: application/vscode-jsonrpc; charset=utf-8");
    headers.push_back("Content-Length: " + std::to_string(body.size()));
}

void Reply::write() const {
    std::string full = getReplyString();

    std::cout << full;
    std::cout.flush();
}

std::string Reply::getReplyString() const {
    std::vector<std::string>::const_iterator header = headers.begin();

    std::ostringstream out;

    for(; header != headers.end(); header++) {
        out << header->c_str() << "\r\n";
    }

    out << "\r\n" << body;

    return out.str();
}

ContentChange::ContentChange(Range range, int range_length, std::string text, ContentChangeType type) :
range(range),
range_length(range_length),
text(text),
type(type)
{

}

ContentChange::ContentChange(Range range, int range_length, std::string text) :
range(range),
range_length(range_length),
text(text),
type(ContentChangeType::NoOp)
{
    computeType();
}

ContentChange::ContentChange(const json& content_change) :
    range(),
    range_length(0),
    text(),
    type(ContentChangeType::NoOp),
    has_range(false),
    has_text(false)
{
    if(content_change.contains("range")) {
        range = Range(content_change["range"]);
        has_range = true;
    }

    if(content_change.contains("rangeLength") && content_change["rangeLength"].is_number_integer()) {
        range_length = content_change["rangeLength"].get<int>();
    }

    if(content_change.contains("text") && content_change["text"].is_string()) {
        text = content_change["text"].get<std::string>();
        has_text = !text.empty();
    }

    computeType();
}

void ContentChange::computeType()
{
    if(!has_range) {
        type = has_text ? ContentChangeType::NoOp : ContentChangeType::Unknown; // TODO NoOp here is actually more a FullDocument chage, TBD upsteram how to handel
        return;
    }

    const bool is_empty_range = range.start.line   == range.end.line   &&
                                range.start.column == range.end.column  ; //TODO: Move to Range class: bool Range::isEmpty() const;

    // INSERT
    if(is_empty_range && has_text) {
        type = ContentChangeType::Insert;
        return;
    }

    // DELETE
    if(!is_empty_range && !has_text)
    {
        type = ContentChangeType::Delete; 
        return;
    }

    // REPLACE
    if(!is_empty_range && has_text)
    {
        type = ContentChangeType::Replace;
        return;
    }

    type = ContentChangeType::Unknown;
}

std::string ContentChange::applyChange(const std::vector<std::string>& affected_lines) const {
    std::string front = affected_lines.front();
    std::string back  = affected_lines.back();

    return front.substr(0, range.start.column) + text + back.substr(range.end.column);
}

std::string Message::getMethod() const {
    if(body.contains("method")) return body["method"].get<std::string>();

    return "";
}

std::string Message::getTextDocumentParameter(std::string param_key) const {
    if(body.contains("params") && 
       body["params"].contains("textDocument") && 
       body["params"]["textDocument"].contains(param_key)) 
       return body["params"]["textDocument"][param_key].get<std::string>();

    return "";
}

std::vector<ContentChange> Message::getContentChanges() const
{
    std::vector<ContentChange> out;

    if(body.contains("params") && body["params"].contains("contentChanges"))
    {
        for(const json& content_change : body["params"]["contentChanges"])
        {
            out.emplace_back(ContentChange(content_change));
        }
    }

    return out;
}

TextLocation Message::getTextLocation() const
{
    if(body.contains("params") && body["params"].contains("position"))
    {
        return TextLocation(body["params"]["position"]);
    }

    return TextLocation{};
}

int Message::getId() const {
    if(body.contains("id")) return body["id"].get<int>();

    return 0;
}

bool Message::empty() {
    return headers.empty();
}

void Message::printHeaders() const {
    std::vector<std::string>::const_iterator h = headers.begin();

    while(h != headers.end()) {
        std::cout << "HEADER: " << (*h) << std::endl;
        h++;
    }
}

void Message::printBody() const {
    std::cout << body << std::endl;
}

void Message::addHeader(const std::string& line) {
    if(line == "\r" || line.empty()) return;

    headers.push_back(line);

    if (line.rfind("Content-Length:", 0) == 0) {
        content_length = std::stoi(line.substr(15));
    }

    //.. more rules to handle headers if needed
}

void Message::addBody(const std::string& body) {
    this->body = json::parse(body);
}

void handleOptionalCapabilities(json& capabilities, const std::vector<Capability>& selected_headers, TokenLegendArray& tokenTypes, ModifierLegendArray& tokenModifiers)
{
    bool has_semantic_tokens = false;

    for (const auto& header : selected_headers) {
        auto it = k_capability_library.find(header);
        
        has_semantic_tokens = has_semantic_tokens || header == Capability::SemanticTokens;

        if (it != k_capability_library.end()) {
            it->second(capabilities);
        }
    }

    // Special case: SemanticTokens (requires external tokenTypes/tokenModifiers)
    if (has_semantic_tokens) {
        capabilities["semanticTokensProvider"]["legend"]["tokenTypes"] = tokenTypes;
        capabilities["semanticTokensProvider"]["legend"]["tokenModifiers"] = tokenModifiers;
    }
}

json buildLSPCapabilities(const std::vector<Capability>& headers, TextDocumentSyncKind text_doc_sync_type) {
    json capabilities = {
        {"textDocumentSync", {
            {"openClose", true},
            {"change", text_doc_sync_type},
            {"save", {
                {"includeText", true}
            }}
        }},
        {"workspace", {
            {"workspaceFolders", {
                {"supported", true},
                {"changeNotifications", true}
            }}
        }}
    };

    int token_count = 0, token_modifier_count = 0;
    
    TokenLegendArray    tl = getTokenLegend();
    ModifierLegendArray tml = getTokenModifiersLegend();

    handleOptionalCapabilities(capabilities, headers, tl, tml);

    return {"capabilities", capabilities};
}