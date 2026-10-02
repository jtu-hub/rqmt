#include <iostream>

extern "C" {
    #include "util.h"
    #include "list.h"
}

#include "cpp_ast_merger.h"
#include "cpp_syntax_definitions.h"

void ASTMerger::resetMultiline() {
    current_multiline_block = SemanticTokenId::none;
    current_multiline_indent = -1;
}

void ASTMerger::setMultiline(const SemanticTokenId& id, int indent_level) {
    current_multiline_block = id;
    current_multiline_indent = indent_level;
}

void ASTMerger::merge(AbstractSyntaxTree& destination, const AbstractSyntaxTree& origin, int origin_indent_level, int line_number) {
    //SyntaxNode* getValidParent(AbstractSyntaxTree& destination, int origin_indent_level)
    int found_at_level = -1;

    SyntaxNode* parent      = destination.lastChildAtLevel(origin_indent_level, &found_at_level).asPointer();
    SyntaxNode* grandparent = destination.lastChildAtLevel(origin_indent_level - 1, &found_at_level).asPointer();

    m_check_pointers(parent, grandparent);

    if(m_st_is_other(parent->getDefinition().id) || (parent == destination.getRootPointer()  && found_at_level > 0)) {
        
        int i = 1;
        // std::cerr << "I: " << i <<"; Found at level: " << found_at_level << "; Is other semantic token: " << (m_st_is_other(parent->getDefinition().id) ? "True" : "False") << "; Is Root: " << (parent == destination.getRootPointer() ? "True" : "False") << std::endl;
        while ((m_st_is_other(parent->getDefinition().id) || (parent == destination.getRootPointer()  && found_at_level > 0)) && origin_indent_level - i >= 0)
        {
            parent      = destination.lastChildAtLevel(origin_indent_level - i, &found_at_level).asPointer();
            grandparent = destination.lastChildAtLevel(origin_indent_level - i - 1, &found_at_level).asPointer();

            m_check_pointers(parent, grandparent);

            i++;
            // std::cerr << "I: " << i <<"; Found at level: " << found_at_level << "; Is other semantic token: " << (m_st_is_other(parent->getDefinition().id) ? "True" : "False") << "; Is Root: " << (parent == destination.getRootPointer() ? "True" : "False") << std::endl;
        }        
    }

    SemanticTokenId parent_id      = parent->getDefinition().id,
                    grandparent_id = grandparent->getDefinition().id;

    // Copy attributes set to the root of the line ast to the parent node
    SyntaxNode* copy_target = parent;
    if(m_st_is_attribute(parent_id) && (m_st_is_keyword(grandparent_id) || m_st_is_root(grandparent_id))) {
        copy_target = grandparent;
    } else if(m_st_is_attribute(parent_id)) {
        parent->addDiagnostic(DiagnosticSeverity::error, DiagnosticId::attribute_child_of_attribute, m_diag_src, 
                              parent->getLine(), parent->getLine(), 
                              parent->getColumn(), parent->getColumn() + parent->getLength());
    }

    AttributeId text_override_id = AttributeId::description_body;
    if(m_st_is_attribute(parent_id)) {
        text_override_id = getAttributeId(parent_id);
    }

    if(line_number >= 0) {
        origin.getRoot().copyAttributesTo(copy_target->asReference(), line_number, text_override_id);
    } else {
        origin.getRoot().copyAttributesTo(copy_target->asReference(), origin.getRoot().getLine(), text_override_id);
    }

    //TODO: define proper helper functions again <3
    for(SyntaxNode child : origin.getRoot().getChildren()) {
        SemanticTokenId child_id  = child.getDefinition().id;

        //update line number if required
        if(line_number >= 0) {
            child.recursiveSetLine(line_number);
        }
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// HANDLE MULTILINE BLOCKS ////////////////////////////////////////////////////////////////////////////////////
        bool is_in_multiline_block = current_multiline_block != SemanticTokenId::none;

        if(is_in_multiline_block && current_multiline_block == child_id) {
            //we reached the end of the multiline block
            resetMultiline();
        } else if(is_in_multiline_block) {
            if(origin_indent_level >= current_multiline_indent) {
                //we are within a multiline block, valid indentation -> override all token defs
                SemanticTokenDefinition multiline_def = current_multiline_block == SemanticTokenId::block_delim_ml ?
                    getStrBacktickDefinition():
                    getStrMathDefinition()    ;

                child.roverrideDefinition(multiline_def);
                child_id = multiline_def.id;
            } else {
                resetMultiline();
                //TODO add diagnostic
            }

        } else if(child_id == SemanticTokenId::block_delim_ml || child_id == SemanticTokenId::math_delim_ml) {
            //we are at the start of a multiline block
            setMultiline(child_id, origin_indent_level);
        } else {
            //we are outisde a multiline block -> do nothing
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// HANDLE RICH TEXT ///////////////////////////////////////////////////////////////////////////////////////////
        if(child_id == SemanticTokenId::source_body) {
            std::vector<NodeAttribute> refs = child.getAttributesById(AttributeId::link);

            for(NodeAttribute ref : refs) 
                if(ref.valid) destination.addReference(child.getLine(), ref.value<TextSpan>());

            if(m_st_is_keyword(parent_id)) {
                SemanticTokenDefinition new_def = getDescriptionBodyDefinition();
                child = new_def;
                child_id = new_def.id;
            } else if(m_st_is_attribute(parent_id)) {
                SemanticTokenDefinition new_def = getValueBodyDefinition();
                child = new_def;
                child_id = new_def.id;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// HANDLE ALIASES /////////////////////////////////////////////////////////////////////////////////////////////
        if(m_st_is_keyword(child_id)) {
            std::vector<NodeAttribute> als = child.getAttributesById(AttributeId::alias);

            for(NodeAttribute al : als) 
                if(al.valid) destination.addAlias(child.getLine(), al.value<TextSpan>());
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// HANDLE HIERARCHY  DIAGNOSTICS //////////////////////////////////////////////////////////////////////////////
        if(m_st_is_attribute(parent_id) && m_st_is_attribute(child_id)) {
            parent->addDiagnostic(DiagnosticSeverity::error, DiagnosticId::attribute_child_of_attribute, m_diag_src, 
                                  child.getLine(), child.getLine(), 
                                  child.getColumn(), child.getColumn() + child.getLength());
        }

        parent->addChild(child);
    }
}