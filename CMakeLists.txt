cmake_minimum_required(VERSION 3.0.0)

project(ToasterChan LANGUAGES C CXX)

set(SOURCES
    main.cpp

    src/llm.cpp

    src/piper.cpp
    src/stt.cpp
    src/tts.cpp

    src/chatbot.cpp
)

set(LIBRARIES
    dpp

    llama
    common

    espeak-ng
    onnxruntime
    piper_phonemize

    whisper
    wcommon
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(external)