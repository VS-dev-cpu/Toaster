add_executable(dc-bot main.cpp src/client.cpp src/tts.cpp src/stt.cpp src/piper.cpp)
target_link_libraries(dc-bot dpp espeak-ng onnxruntime piper_phonemize whisper wcommon)
target_include_directories(dc-bot PUBLIC include)

add_subdirectory(external)