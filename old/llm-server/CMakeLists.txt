add_executable(llm-server main.cpp src/server.cpp src/llm.cpp)
target_link_libraries(llm-server llama common)
target_include_directories(llm-server PUBLIC include)

add_subdirectory(external)