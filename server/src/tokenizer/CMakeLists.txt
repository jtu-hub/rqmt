add_library(tokenizer c_tokenizer.c c_token_definitions.c)

target_include_directories(tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tokenizer util)