#pragma once

#include <dpp/dpp.h>

#include <stt.hpp>
#include <tts.hpp>

#include <atomic>
#include <list>

struct Message {
    std::string author, content;
};

// Discord Chatbot base class
class Chatbot {
  public:
    Chatbot(std::string token);
    ~Chatbot();

    void typing(dpp::snowflake channel);

    void send(dpp::snowflake channel, std::string content);
    // void talk(std::string text); // TODO

    // Message Pool
    struct {
        dpp::snowflake channel;
        std::list<Message> messages;
        std::atomic<uint8_t> length;
    } pool{};
    // TODO
    // channels to 'Message', voice messages
    // text formatting

  private:
    dpp::cluster bot;
};