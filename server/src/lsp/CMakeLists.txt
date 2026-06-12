add_library(lsp lsp.cpp dispatcher.cpp lsp_util.cpp)

target_include_directories(lsp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lsp util cpp_parser workspace nlohmann_json::nlohmann_json)