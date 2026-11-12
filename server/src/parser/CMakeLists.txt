add_library(parser parser.cpp parser_util.cpp)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(parser tokenizer util nlohmann_json::nlohmann_json)