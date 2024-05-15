#include <chatbot.hpp>

const struct {
    const dpp::snowflake myself = 1108128632624402434;
    const dpp::snowflake dev = 615473053840834580;
} user;

struct {
    const dpp::snowflake text = 1093135703602364416;
    const dpp::snowflake voice = 1239974554508591214;
} channel;

Chatbot::Chatbot(std::string token)
    : bot(token, dpp::i_default_intents | dpp::i_message_content) {

    // Message Callback
    bot.on_message_create([&](const dpp::message_create_t &event) {
        if (event.msg.author == user.myself ||
            event.msg.channel_id != channel.text && !event.msg.is_dm())
            return;

        auto &username = event.msg.author.username;
        auto &content = event.msg.content;

        // Add to message pool
        // TODO format text for emojis, etc
        pool.channel = channel.text;
        pool.messages.push({username, content});
        pool.length++;

        printf("[pool size: %i] ", (int)pool.messages.size());
        printf("New Message from '\e[0;35m%s\e[39m':\n", username.c_str());
        printf("\e[0;92m%s\e[39m\n", content.c_str());
    });

    // Voice Callback
    // std::vector<float> pcmf32;
    // bot.on_voice_receive([&](dpp::voice_receive_t event) {
    //     if (event.user_id == user.myself || event.user_id != user.dev)
    //         return;

    //     // convert uint8 to float
    //     // TODO
    //     for (size_t i = 0; i < event.audio_data.size(); i += 2) {
    //         pcmf32.push_back((event.audio_data[i] - 128) / 128.0f);
    //     }

    //     // Process
    //     std::string text = stt.process(pcmf32);
    //     if (text.size() > 1) {
    //         pcmf32.clear();

    //         // Add to voice pool
    //         // TODO get speaker name
    //         pool.voice.channel = channel.voice;
    //         pool.voice.data.push_back({"voice", text});

    //         printf("[pool size: %i] ", (int)pool.voice.data.size());
    //         printf("Voice from '\e[0;92m%i\e[39m':\n", event.user_id);
    //         printf("\e[0;35m%s\e[39m\n", text.c_str());
    //     }
    // });

    // Log Callback
    // bot.on_log(dpp::utility::cout_logger());
    bot.on_log([&](dpp::log_t) {});

    // Start Bot

    // LLM Processing Thread
    bot.start(dpp::st_return);
}

Chatbot::~Chatbot() {}

void Chatbot::typing(dpp::snowflake channel) {
    bot.channel_typing(channel, {});
}

void Chatbot::send(dpp::snowflake channel, std::string content) {
    dpp::message msg;
    msg.channel_id = pool.channel;
    msg.content = content;
    bot.message_create(msg);
}