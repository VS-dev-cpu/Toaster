#include <atomic>
#include <cstdlib>
#include <string>
#include <thread>

#include <dpp/dpp.h>

#include <llm.hpp>
#include <util.hpp>

int main(int argc, char const *argv[]) {
    // Set Random Seed
    srand(time(0));

    // Init LLM
    LLM llm("../models/zephyr.gguf", loadPrompt("../prompts/prompt.txt"));

    // TODO conversation database

    const auto myself = dpp::snowflake("1108128632624402434");

    // Discord Bot
    dpp::cluster bot(getenv("TOKEN_TOASTERCHAN"),
                     dpp::i_default_intents | dpp::i_message_content);

    std::atomic<bool> isTyping = false;

    // onMessage Callback
    auto onMessage = [&](const dpp::message_create_t &event) {
        const auto &author = event.msg.author;

        if (author == myself)
            return;

        const auto &content = event.msg.content;
        const auto &mentions = event.msg.mentions;

        // Normalize Mentions
        // int open = -1;
        // std::string num, msg;
        // for (size_t i = 0; i < content.size(); i++) {
        //     const char c = content[i];
        //     switch (c) {
        //     case '<':
        //         open = i;
        //         break;
        //     case '>':
        //         if (open != -1) {
        //             int id;
        //             sscanf(content.c_str() + open + 1, "%i", &id);
        //             for (auto m : mentions) {
        //                 if (m.first.id == id) {
        //                     msg += '@';
        //                     msg += m.first.username;
        //                     break;
        //                 }
        //             }
        //             open = -1;
        //         }
        //         break;
        //     }
        // }
        // for (auto c : content) {
        //     switch (c) {
        //     case '<':
        //         open = true;
        //         num.clear();
        //         break;

        //     case '>':
        //         if (open) {
        //             open = false;
        //             // Get Username
        //             dpp::snowflake id = num;
        //             for (auto m : mentions) {
        //                 if (m.first.id == id) {
        //                     msg += '@';
        //                     msg += m.first.username;
        //                     break;
        //                 }
        //             }
        //         } else {
        //             msg += c;
        //         }
        //         break;

        //     default:
        //         if (open) {
        //             if (c != '@')
        //                 num += c;
        //         } else {
        //             msg += c;
        //         }
        //     }
        // }
        std::string msg = content;

        // Is Typing Callback
        std::function<void(const dpp::confirmation_callback_t &)> cllbck;
        cllbck = [&](const dpp::confirmation_callback_t &result) {
            if (!result.is_error()) {
                if (isTyping)
                    bot.channel_typing(event.msg.channel_id, cllbck);
                // TODO
            }
        };

        // std::thread *t = new std::thread([&] {
        struct {
            bool open;     // is action in progress
            bool complete; // is action type complete
            std::string type, data;
        } action;

        printf("Input message: %s\n", msg.c_str());
        std::string response =
            llm.reply(author.username, msg, [&](const std::string &token) {
                for (auto c : token) {
                    if (c == '[')
                        action = {1, 0, "", ""};
                    else {
                        if (action.open) {
                            if (c == ':') {
                                action.complete = true;

                                // Reply to previous comment
                                if (!strncasecmp(action.type.c_str(), "REPLY",
                                                 5) ||
                                    !strncasecmp(action.type.c_str(), "SEND",
                                                 4)) {
                                    isTyping = true;
                                    bot.channel_typing(event.msg.channel_id,
                                                       cllbck);
                                }
                            } else if (c == ']') {
                                action.open = false;
                                isTyping = false;

                                if (action.type == "REPLY")
                                    event.reply(action.data);
                                else if (action.type == "SEND")
                                    event.send(action.data);
                                else if (action.type == "REACT")
                                    // bot.message_add_reaction(event.msg,
                                    //  action.data);
                                    event.reply(action.data);

                                else
                                    printf("Invalid Action: %s, %s\n",
                                           action.type.c_str(),
                                           action.data.c_str());
                            } else {
                                if (!action.complete)
                                    action.type += c;
                                else
                                    action.data += c;
                            }
                        }
                    }
                }
            });

        printf("Generated message: %s\n", response.c_str());
    };

    bot.on_log(dpp::utility::cout_logger());
    bot.on_message_create(onMessage);

    bot.start(dpp::st_wait);

    return 0;
}