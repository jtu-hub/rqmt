add_library(workspace workspace.cpp rqmtfile.cpp logger.cpp)

target_include_directories(workspace PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(workspace tokenizer util cpp_parser lsp)