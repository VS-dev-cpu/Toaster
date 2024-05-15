add_subdirectory(include)
add_subdirectory(lib)

target_link_directories(llm-server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories(llm-server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)