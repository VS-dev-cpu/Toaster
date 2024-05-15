#include <atomic>
#include <cstdlib>
#include <string>

#include <llm.hpp>

#include <chatbot.hpp>

void sleep(int s) {
    int start = time(0);
    while (start + s > time(0))
        ;
}

int main(int argc, char const *argv[]) {
    // Set Random Seed
    srand(time(0));

    // Init Speech
    // TTS tts(getenv("MODEL_PIPER"), "amy");
    // STT stt(getenv("MODEL_WHISPER_BASE"));

    // Init LLM
    LLM llm(getenv("MODEL_ZEPHYR_7B_BETA"), "../prompts/prompt.txt");
    printf("Loaded models\n");

    Chatbot chatbot(getenv("TOKEN_TOASTERCHAN"));

    bool processed = false;

    while (true) {
        // Check for Messages
        if (chatbot.pool.length > 0) {
            // Process Voice
            auto &msg = chatbot.pool.messages.front();

            llm.decode(msg.author, msg.content);
            chatbot.pool.messages.pop();
            chatbot.pool.length--;

            printf("[pool size: %i] Decoded message\n",
                   (int)chatbot.pool.length);

            processed = true;
        }

        if (chatbot.pool.length == 0)
            sleep(4);

        if (chatbot.pool.length == 0 && processed) {
            processed = false;

            // Actions
            struct {
                const char *str;
                bool typed = false;
                int status = 0;
            } action[] = {{"[NOTE]", 0},  {"[SEND]", 1}, {"[REPLY]", 1},
                          {"[REACT]", 0}, {"[JOIN]", 0}, {"[TALK]", 0}};
            const size_t actions = sizeof(action) / sizeof(*action);

            std::string tmp;
            int type = -1;
            auto send = [&](const char *message, size_t length) {
                if (type < 0 || !message || length == 0)
                    return;

                const auto &a = action[type];

                switch (type) {
                case 0: // note
                    printf("NOTE: %.*s\n", length, message);
                    // nothing
                    break;

                case 1: { // send
                    chatbot.send(chatbot.pool.channel,
                                 std::string(message, length));
                } break;

                case 2: // reply
                    // event.reply(str);
                    break;

                case 3: // react
                    // bot.message_add_reaction(msg, str);
                    break;

                case 4: { // join
                    // dpp::guild *g = dpp::find_guild(msg.guild_id);
                    // if (!g)
                    //                         printf("failed to get guild
                    //                         id\n");
                    //                     else {
                    //     if (!g->connect_member_voice(msg.author.id)) {
                    //                             printf("Author is not in
                    //                             channel\n");
                    //     } else {
                    //                             printf("Connected to
                    //                             voice\n");
                    //     }
                    //                     }
                } break;

                case 5: { // talk
                    // auto buf = tts.say(str);
                    // const size_t len = buf.size() * 4;
                    // uint16_t buff[len];

                    // // Convert 44100 MONO to "STEREO"
                    // for (size_t i = 0; i < buf.size(); i++) {
                    //     buff[i * 4 + 0] = buf[i];
                    //     buff[i * 4 + 1] = buf[i];

                    //     buff[i * 4 + 2] = buf[i];
                    //     buff[i * 4 + 3] = buf[i];
                    // }

                    // auto client = bot.get_shard(0);
                    // dpp::voiceconn *v = client->get_voice(msg.guild_id);
                    // if (v && v->voiceclient &&
                    // v->voiceclient->is_ready())
                    //     v->voiceclient->send_audio_raw(buff, len * 2);
                    // printf("TALK: %s\n", str);
                } break;
                }

                tmp.clear();
            };

            // (de)Generate
            printf("Generating message...\n\e[0;36m");
            std::string text = llm.generate([&](const std::string &token) {
                printf("%s", token.c_str());
                for (auto c : token) {
                    // Check Commands
                    for (size_t i = 0; i < actions; i++) {
                        auto &a = action[i];

                        if (c == a.str[a.status])
                            a.status++;
                        else if (a.status >= strlen(a.str)) {
                            int atl = type >= 0 ? strlen(action[type].str) : 0;
                            send(tmp.c_str() + atl,
                                 tmp.size() - atl - a.status);

                            type = i;

                            // Start typing
                            chatbot.typing(chatbot.pool.channel);

                            a.status = 0;
                        } else {
                            a.status = 0;
                        }
                    }

                    tmp += c;
                }
            });
            send(tmp.c_str() + strlen(action[type].str),
                 tmp.size() - strlen(action[type].str));

            printf("\e[39m\n");
        }
    }

    return 0;
}

//  std::thread t([&] {
//         bool has_msg = false;
//         dpp::message msg;

//         // Is Typing Callback
//         std::function<void(const dpp::confirmation_callback_t &)> cllbck;
//         cllbck = [&](const dpp::confirmation_callback_t &result) {
//             if (!result.is_error()) {
//                 if (isTyping)
//                     bot.channel_typing(msg.channel_id, cllbck);
//             }
//         };

//         while (true) {
//             if (length > 0) {
//                 msg = messages.front();
//                 has_msg = true;

//                 std::string author =
//                     msg.author.is_bot() ? "[BOT] " : "" +
//                     msg.author.username;

//                 llm.decode(author, msg.content);
//                 messages.pop_front();
//                 length--;
//                 printf("Decoded message, pool size: %i\n", (int)length);
//             } else if (has_msg) {
//                 printf("Generating message...\n");
//                 // Actions
//                 struct {
//                     const char *str;
//                     bool typed = false;
//                     int status = 0;
//                 } action[] = {{"[NOTE]", 0},  {"[SEND]", 1}, {"[REPLY]", 1},
//                               {"[REACT]", 0}, {"[JOIN]", 0}, {"[TALK]", 0}};
//                 const size_t actions = sizeof(action) / sizeof(*action);

//                 int type = -1;
//                 std::string tmp;

//                 auto send = [&]() {
//                     const auto &a = action[type];
//                     const char *str = tmp.c_str(); // + strlen(a.str) + 2;

//                     switch (type) {
//                     case 0: // note
//                         printf("NOTE: %s\n", str);
//                         // nothing
//                         break;

//                     case 1: { // send
//                         dpp::message out;
//                         out.content = str;
//                         out.channel_id = msg.channel_id;
//                         bot.message_create(out);
//                     } break;

//                     case 2: // reply
//                         // event.reply(str);
//                         break;

//                     case 3: // react
//                         bot.message_add_reaction(msg, str);
//                         break;

//                     case 4: { // join
//                         dpp::guild *g = dpp::find_guild(msg.guild_id);
//                         if (!g)
//                             printf("failed to get guild id\n");
//                         else {
//                             if (!g->connect_member_voice(msg.author.id)) {
//                                 printf("Author is not in channel\n");
//                             } else {
//                                 printf("Connected to voice\n");
//                             }
//                         }
//                     } break;

//                     case 5: { // talk
//                         auto buf = tts.say(str);
//                         const size_t len = buf.size() * 4;
//                         uint16_t buff[len];

//                         // Convert 44100 MONO to "STEREO"
//                         for (size_t i = 0; i < buf.size(); i++) {
//                             buff[i * 4 + 0] = buf[i];
//                             buff[i * 4 + 1] = buf[i];

//                             buff[i * 4 + 2] = buf[i];
//                             buff[i * 4 + 3] = buf[i];
//                         }

//                         auto client = bot.get_shard(0);
//                         dpp::voiceconn *v = client->get_voice(msg.guild_id);
//                         if (v && v->voiceclient &&
//                         v->voiceclient->is_ready())
//                             v->voiceclient->send_audio_raw(buff, len * 2);
//                         printf("TALK: %s\n", str);
//                     } break;
//                     }

//                     tmp.clear();
//                     isTyping = false;
//                 };

//                 // Generate Text
//                 std::string text = llm.generate([&](const std::string &token)
//                 {
//                     printf("%s", token.c_str());
//                     for (auto c : token) {
//                         // Check Commands
//                         for (size_t i = 0; i < actions; i++) {
//                             auto &a = action[i];

//                             if (c == a.str[a.status])
//                                 a.status++;
//                             else if (a.status >= strlen(a.str)) {
//                                 send();
//                                 type = i;

//                                 // Start typing
//                                 if (a.typed) {
//                                     isTyping = true;
//                                     bot.channel_typing(msg.channel_id,
//                                     cllbck);
//                                 }

//                                 a.status = 0;
//                             } else {
//                                 a.status = 0;
//                             }
//                         }

//                         tmp += c;
//                     }
//                 });
//                 send();

//                 printf("Generated Message: %s\n", text.c_str());

//                 has_msg = false;
//             }

//             if (length == 0)
//                 sleep(1); // sleep 1 second
//         }
//     });