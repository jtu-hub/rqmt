#pragma once

#include <vector>
#include <variant>
#include <nlohmann/json.hpp>

#include "capabilities_registry.h"
#include "lsp_util.h"
#include "cpp_semantic_token_definitions.h"

using json = nlohmann::json;

enum class Capability;

enum class ContentChangeType
{
    Unknown = -1,
    NoOp,
    Insert,
    Delete,
    Replace,
};

class ContentChange {
    public:
        bool has_range;
        bool has_text;
        Range range;
        int range_length;
        std::string text;
        ContentChangeType type;

        ContentChange(Range range, int range_length, std::string text);
        ContentChange(Range range, int range_length, std::string text, ContentChangeType type);
        ContentChange(const json& content_change);

    protected:
        void computeType();
    public:    
        std::vector<ContentChange> split();
        int startLine() const { return range.start.line; };
        bool isValid() const;

        bool isDelete()  const { return type == ContentChangeType::Delete; };
        bool isInsert()  const { return type == ContentChangeType::Insert; };
        bool isReplace() const { return type == ContentChangeType::Replace;};

        std::string applyChange(const std::vector<std::string>& affected_lines) const;
};

class Reply {
    private:
        std::string body;
        std::vector<std::string> headers;
    public:
        Reply(int id, json result, std::string jsonrpc = "2.0");
        Reply() : Reply(0, nullptr) {}

        void write() const;
        std::string getReplyString() const;
};

class Message {
    private:
        int content_length = 0;
        std::vector<std::string> headers;
        json body;
    
    public:
        void addHeader(const std::string& line);
        void addBody(const std::string& body);
        int getContentLength() { return content_length; };
        const json& getBody() const { return body; };
        bool empty();
        void printHeaders() const;
        void printBody() const;
        std::string getMethod() const;
        int getId() const;
        std::string getTextDocumentParameter(std::string param_key) const;
        std::vector<ContentChange> getContentChanges() const;
        TextLocation getTextLocation() const;
};

void handleOptionalCapabilities(json& capabilities, const std::vector<Capability>& selected_headers, TokenLegendArray& tokenTypes, ModifierLegendArray& tokenModifiers);

json buildLSPCapabilities(const std::vector<Capability>& headers, TextDocumentSyncKind text_doc_sync_type = TextDocumentSyncKind::Incremental);