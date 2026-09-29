add_library(util errors.c list.c util.c cpp_util.cpp text_util.cpp)

target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(util nlohmann_json::nlohmann_json)