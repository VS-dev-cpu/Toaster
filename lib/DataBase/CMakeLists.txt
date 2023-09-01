# Source Files
set(SOURCES
    src/DataBase.cpp
)

# Libraries
set(LIBRARIES sqlite3)

# Project
project(DataBase VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
