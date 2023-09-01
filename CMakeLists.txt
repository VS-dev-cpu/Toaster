cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(DataBase)

add_subdirectory(music_management)

add_subdirectory(Toaster)
add_subdirectory(ToasterChan)
