#pragma once

#include "whisper/whisper.h"

#include <string>
#include <vector>

// Wrapper Class for the whisper.cpp library
class STT {
  public:
    STT(const char *model);
    ~STT();

    void setLang(std::string lang = "auto", bool transl = false);

    std::string process(std::vector<float> &pcm32f);

  private:
    std::string language = "auto";
    bool translate = false;

    struct whisper_context *ctx{};
};