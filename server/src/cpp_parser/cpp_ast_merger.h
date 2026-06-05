#include <vector>
#include <unordered_map>

extern "C" {
    #include "c_token_definitions.h"
    #include "list.h"
}

#include "cpp_attribute_definitions.h"
#include "cpp_diagnostic_definitions.h"
#include "cpp_semantic_token_definitions.h"
#include "cpp_parser_util.h"

class ASTMerger {
    private:
        SemanticTokenId current_multiline_block = SemanticTokenId::none;
        int current_multiline_indent = -1;
    public:
        void merge(AbstractSyntaxTree& destination, const AbstractSyntaxTree& origin, int origin_indent_level, int line_number = -1);
        void resetMultiline();
        void setMultiline(const SemanticTokenId& id, int indent_level);
};