cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories("src/include")

# Source Files
set(SOURCES
    src/main.cpp
)

# Libraries
set(LIBRARIES
    dpp
    sqlite3
    mpg123
)

# Project
project(Toaster VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
