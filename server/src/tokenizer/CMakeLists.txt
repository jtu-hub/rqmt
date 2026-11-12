add_library(tokenizer tokenizer.cpp token_definition.cpp)

target_include_directories(tokenizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tokenizer util)