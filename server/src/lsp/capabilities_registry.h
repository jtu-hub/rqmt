#pragma once

#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class TextDocumentSyncKind {
    None = 0,      
    Full = 1,
    Incremental = 2
};

using DiagnosticReportKind = const char*;

namespace DocumentDiagnosticReportKind
{
    inline constexpr DiagnosticReportKind Full = "full";
    inline constexpr DiagnosticReportKind Unchanged = "unchanged";
}

enum class WorkspaceFolderSupport {
    Disabled = 0,
    Enabled = 1
};

enum class DynamicRegistration {
    Disabled = false,
    Enabled = true
};

enum class Capability {
    Initialize,
    Initialized,
    Shutdown,
    Exit,
    DidOpen,
    DidChange,
    DidClose,
    DidSave,
    Completion,
    Hover,
    SignatureHelp,
    Declaration,
    Definition,
    TypeDefinition,
    Implementation,
    References,
    DocumentHighlight,
    DocumentSymbol,
    WorkspaceSymbol,
    CodeAction,
    CodeLens,
    Formatting,
    RangeFormatting,
    OnTypeFormatting,
    Rename,
    PrepareRename,
    FoldingRange,
    SelectionRange,
    DocumentLink,
    DocumentColor,
    ColorPresentation,
    ExecuteCommand,
    DidChangeWorkspaceFolders,
    DidChangeConfiguration,
    DidChangeWatchedFiles,
    PublishDiagnostics,
    LogMessage,
    ShowMessage,
    Telemetry,
    WorkDoneProgressCreate,
    WorkDoneProgressCancel,
    SetTrace,
    InlayHint,
    InlineValue,
    Moniker,
    LinkedEditingRange,
    CallHierarchy,
    TypeHierarchy,
    SemanticTokens,
    NotebookDocument,
    Diagnostic,
    Custom
};

// Hash function for Capability (required for std::unordered_map)
struct CapabilityHash {
    size_t operator()(Capability header) const {
        return static_cast<size_t>(header);
    }
};

// Type for capability handlers
using CapabilityHandler = std::function<void(json&)>;

// Registry of all capabilities
const std::unordered_map<Capability, CapabilityHandler, CapabilityHash> k_capability_library = {
    {Capability::Completion, [](json& c) {
        c["completionProvider"] = {
            {"resolveProvider", true},
            {"triggerCharacters", {".", ":"}}
        };
    }},

    {Capability::Hover, [](json& c) {
        c["hoverProvider"] = true;
    }},

    {Capability::SignatureHelp, [](json& c) {
        c["signatureHelpProvider"] = {
            {"triggerCharacters", {"(", ","}},
            {"retriggerCharacters", {")"}}
        };
    }},

    {Capability::Declaration, [](json& c) {
        c["declarationProvider"] = true;
    }},

    {Capability::Definition, [](json& c) {
        c["definitionProvider"] = true;
    }},

    {Capability::TypeDefinition, [](json& c) {
        c["typeDefinitionProvider"] = true;
    }},

    {Capability::Implementation, [](json& c) {
        c["implementationProvider"] = true;
    }},

    {Capability::References, [](json& c) {
        c["referencesProvider"] = true;
    }},

    {Capability::DocumentHighlight, [](json& c) {
        c["documentHighlightProvider"] = true;
    }},

    {Capability::DocumentSymbol, [](json& c) {
        c["documentSymbolProvider"] = true;
    }},

    {Capability::WorkspaceSymbol, [](json& c) {
        c["workspaceSymbolProvider"] = true;
    }},

    {Capability::CodeAction, [](json& c) {
        c["codeActionProvider"] = true;
    }},

    {Capability::CodeLens, [](json& c) {
        c["codeLensProvider"] = {
            {"resolveProvider", false}
        };
    }},

    {Capability::Formatting, [](json& c) {
        c["documentFormattingProvider"] = true;
    }},

    {Capability::RangeFormatting, [](json& c) {
        c["documentRangeFormattingProvider"] = true;
    }},

    {Capability::OnTypeFormatting, [](json& c) {
        c["documentOnTypeFormattingProvider"] = {
            {"firstTriggerCharacter", ";"},
            {"moreTriggerCharacter", {"\n"}}
        };
    }},

    {Capability::Rename, [](json& c) {
        c["renameProvider"] = {
            {"prepareProvider", true}
        };
    }},

    {Capability::PrepareRename, [](json& c) {
        c["renameProvider"] = {
            {"prepareProvider", true}
        };
    }},

    {Capability::FoldingRange, [](json& c) {
        c["foldingRangeProvider"] = true;
    }},

    {Capability::SelectionRange, [](json& c) {
        c["selectionRangeProvider"] = true;
    }},

    {Capability::DocumentLink, [](json& c) {
        c["documentLinkProvider"] = {
            {"resolveProvider", false}
        };
    }},

    {Capability::DocumentColor, [](json& c) {
        c["documentColorProvider"] = true;
    }},

    {Capability::ColorPresentation, [](json& c) {
        c["colorPresentationProvider"] = true;
    }},

    {Capability::ExecuteCommand, [](json& c) {
        c["executeCommandProvider"] = {
            {"commands", {"command.id"}}
        };
    }},

    {Capability::DidChangeWorkspaceFolders, [](json& c) {
        c["workspace"]["workspaceFolders"] = {
            {"supported", true},
            {"changeNotifications", true}
        };
    }},

    {Capability::DidChangeConfiguration, [](json& c) {
        (void)c;
    }},

    {Capability::DidChangeWatchedFiles, [](json& c) {
        c["workspace"]["didChangeWatchedFiles"] = {
            {"dynamicRegistration", true}
        };
    }},

    {Capability::SemanticTokens, [](json& c) {
        c["semanticTokensProvider"] = {
            {"legend", {
                {"tokenTypes", {}},
                {"tokenModifiers", {}}
            }},
            {"full", true},
            // {"range", true},
            // {"full", {
            //     {"delta", true}
            // }}
        };
    }},

    {Capability::InlayHint, [](json& c) {
        c["inlayHintProvider"] = true;
    }},

    {Capability::InlineValue, [](json& c) {
        c["inlineValueProvider"] = true;
    }},

    {Capability::Moniker, [](json& c) {
        c["monikerProvider"] = true;
    }},

    {Capability::LinkedEditingRange, [](json& c) {
        c["linkedEditingRangeProvider"] = true;
    }},

    {Capability::CallHierarchy, [](json& c) {
        c["callHierarchyProvider"] = true;
    }},

    {Capability::TypeHierarchy, [](json& c) {
        c["typeHierarchyProvider"] = true;
    }},

    {Capability::NotebookDocument, [](json& c) {
        c["notebookDocumentSync"] = {
            {"dynamicRegistration", true},
            {"executionSummarySupport", true}
        };
    }},

    {Capability::Diagnostic, [](json& c) {
        c["diagnosticProvider"] = {
            {"interFileDependencies", false},
            {"workspaceDiagnostics", false}
        };
    }},

    // Ignored headers (no capability)
    {Capability::Initialize, [](json&) {}},
    {Capability::Initialized, [](json&) {}},
    {Capability::Shutdown, [](json&) {}},
    {Capability::Exit, [](json&) {}},
    {Capability::DidOpen, [](json&) {}},
    {Capability::DidChange, [](json&) {}},
    {Capability::DidClose, [](json&) {}},
    {Capability::DidSave, [](json&) {}},
    {Capability::PublishDiagnostics, [](json&) {}},
    {Capability::LogMessage, [](json&) {}},
    {Capability::ShowMessage, [](json&) {}},
    {Capability::Telemetry, [](json&) {}},
    {Capability::WorkDoneProgressCreate, [](json&) {}},
    {Capability::WorkDoneProgressCancel, [](json&) {}},
    {Capability::SetTrace, [](json&) {}},

    // Custom hook
    {Capability::Custom, [](json&) {
        // User-defined extension point
    }},
};