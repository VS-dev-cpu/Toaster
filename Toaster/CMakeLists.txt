# Source Files
set(SOURCES
    src/main.cpp
)

# Libraries
set(LIBRARIES
    dpp
)

# Project
project(Toaster VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
