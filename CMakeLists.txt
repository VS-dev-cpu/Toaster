cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

configure_file(lib/config.h.in lib/config.h)
include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(lib)

add_subdirectory(Toaster)
add_subdirectory(ToasterChan)
