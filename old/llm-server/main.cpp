#include <llm.hpp>
#include <server.hpp>

#include <atomic>
#include <list>

#include <string.h>

struct Message {
    std::string author;
    std::string content;
};

void sleep(int s) {
    int start = time(0);
    while (start + s > time(0))
        ;
}

int main(int argc, char const *argv[]) {
    // Set Random Seed
    srand(time(0));

    // Message Pool
    struct {
        std::atomic<uint8_t> length;
        std::list<Message> messages;
    } pool{};

    // Start Server
    Server server([&](std::string msg) {
        // TODO determine type; add to pool
        char author[32], text[256];
        sscanf(msg.c_str(), "%[^:]s: %[^\n]s", author, text);
        pool.messages.push_back({author, text});
        pool.length++;

        printf("[pool size: %i] New Message:\n\e[0;92m%s\e[39m\n",
               (int)pool.length, msg.c_str());
    });

    // // Init LLM
    LLM llm(getenv("MODEL_ZEPHYR_7B_BETA"), "../../prompts/prompt.txt");

    // Main Loop
    Message msg;
    bool has_msg = false;
    while (1) {
        // Decode messages (if any)
        // TODO make audio messages priority
        if (pool.length > 0) {
            msg = pool.messages.front();

            llm.decode(msg.author, msg.content);
            pool.messages.pop_front();
            pool.length--;

            has_msg = true;

            printf("Decoded message, pool size: %i\n", (int)pool.length);
        }

        // Generate Text (if pool is empty)
        if (pool.length == 0 && has_msg) {
            has_msg = false;
            printf("Generating message...\n\e[0;36m");
            // Actions
            struct {
                const char *str;
                bool typed = false;
                int status = 0;
            } action[] = {{"[NOTE]", 0},  {"[SEND]", 1}, {"[REPLY]", 1},
                          {"[REACT]", 0}, {"[JOIN]", 0}, {"[TALK]", 0}};
            const size_t actions = sizeof(action) / sizeof(*action);

            int type = -1;
            std::string tmp;

            auto send = [&](const char *msg, size_t len) {
                server.send(std::string(msg, len));
            };

            // Generate Text
            std::string text = llm.generate([&](const std::string &token) {
                printf("%s", token.c_str());
                for (auto c : token) {
                    // Check Commands
                    for (size_t i = 0; i < actions; i++) {
                        auto &a = action[i];

                        if (c == a.str[a.status])
                            a.status++;
                        else if (a.status >= strlen(a.str)) {
                            send(tmp.c_str(), tmp.size() - a.status);
                            type = i;

                            // Start typing
                            // if (a.typed) {
                            //     isTyping = true;
                            //     bot.channel_typing(msg.channel_id,
                            // cllbck);
                            // }

                            a.status = 0;
                        } else {
                            a.status = 0;
                        }
                    }

                    tmp += c;
                }
            });
            send(tmp.c_str(), tmp.size());

            printf("\e[39m");
        }
    }

    return 0;
}

// const auto &a = action[type];
// const char *str = tmp.c_str(); // + strlen(a.str) +
// 2;

// switch (type) {
// case 0: // note
//     printf("NOTE: %s\n", str);
//     // nothing
//     break;

// case 1: { // send
//     dpp::message out;
//     out.content = str;
//     out.channel_id = msg.channel_id;
//     bot.message_create(out);
// } break;

// case 2: // reply
//     // event.reply(str);
//     break;

// case 3: // react
//     bot.message_add_reaction(msg, str);
//     break;

// case 4: { // join
//     dpp::guild *g = dpp::find_guild(msg.guild_id);
//     if (!g)
//         printf("failed to get guild id\n");
//     else {
//         if
// (!g->connect_member_voice(msg.author.id)) {
//             printf("Author is not in channel\n");
//         } else {
//             printf("Connected to voice\n");
//         }
//     }
//     } break;

// case 5: { // talk
//     auto buf = tts.say(str);
//     const size_t len = buf.size() * 4;
//     uint16_t buff[len];

//     // Convert 44100 MONO to "STEREO"
//     for (size_t i = 0; i < buf.size(); i++) {
//         buff[i * 4 + 0] = buf[i];
//         buff[i * 4 + 1] = buf[i];

//         buff[i * 4 + 2] = buf[i];
//         buff[i * 4 + 3] = buf[i];
//     }

//     auto client = bot.get_shard(0);
//     dpp::voiceconn *v =
// client->get_voice(msg.guild_id);
//     if (v && v->voiceclient &&
//     v->voiceclient->is_ready())
//         v->voiceclient->send_audio_raw(buff, len *
// 2);
//     printf("TALK: %s\n", str);
// } break;
// }
//
// tmp.clear();
// isTyping = false;