add_library(parser parser.cpp semantic_tokens.cpp syntax_node.cpp parser_context.cpp node_decoders.cpp abstract_syntax_tree.cpp)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(parser tokenizer util nlohmann_json::nlohmann_json)