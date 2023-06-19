#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <dpp/dpp.h>
#include <dpp/presence.h>

#include <DataBase.hpp>

#include <fileutil.hpp>
#include <string>

void onMessage(const dpp::message_create_t &event) {}

int main(int argc, char const *argv[]) {
    std::vector<std::string> files;
    srand(time(0));

    scanForFiles(files);

    // Setup Bot
    dpp::cluster bot(getToken("../TOKEN"),
                     dpp::i_default_intents | dpp::i_message_content);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_message_create(onMessage);

    auto lOnMessage = [&](const dpp::message_create_t &event) {
        char cmd[64], data[256];
        sscanf(event.msg.content.c_str(), "%s %[^\n]", cmd, data);

        // Join Voice Channel
        if (!strcmp(cmd, ".join")) {
            dpp::guild *g = dpp::find_guild(event.msg.guild_id);
            if (!g->connect_member_voice(event.msg.author.id)) {
                bot.message_create(dpp::message(
                    event.msg.channel_id,
                    "You don't seem to be on a voice channel! :("));
            }
        }

        // Get Music
        if (!strcmp(cmd, ".get")) {
            // Download Music
            std::string url =
                //"https://www.youtube.com/watch?v=" +
                std::string(data);

            system(("cd audio && yt-dlp -x --audio-format mp3 " + url).c_str());
        }

        // Get Music
        if (!strcmp(cmd, ".fetch")) {
            uint id = 0;
            int n = sscanf(data, "%i", &id);

            if (n != 1 || id >= files.size())
                event.reply("ID out of index!\n");
            else
                event.reply(files[id]);
        }

        // List Musics
        if (!strcmp(cmd, ".list")) {
            scanForFiles(files);

            if (!strcmp(data, "number"))
                event.reply("There are " + std::to_string(files.size()) +
                            " Audio Files Available");
            else {
                std::string list;

                for (size_t f = 0; f < files.size(); f++)
                    list += std::to_string(f) + ": " + files[f] + "\n";

                event.reply("Available Audio Files:\n" + list);
            }
        }

        // Play Music from YouTube
        if (!strcmp(cmd, ".play")) {
            uint id = 0, count = 0;
            int n = sscanf(data, "%i %i", &id, &count);

            if (count == 0)
                count = 1;

            if (id >= files.size())
                event.reply("ID out of index!\n");
            else {
                if (n < 1)
                    id = rand() % files.size();

                event.reply("Playing: " + files[id]);

                std::vector<uint8_t> pcmdata;
                loadMusic(pcmdata, ("audio/" + files[id]).c_str());

                // Play
                dpp::voiceconn *v = event.from->get_voice(event.msg.guild_id);
                if (v && v->voiceclient && v->voiceclient->is_ready()) {
                    /* Stream the already decoded MP3 file. This passes the
                     * PCM data to the library to be encoded to OPUS */
                    for (int i = 0; i < count; i++)
                        v->voiceclient->send_audio_raw(
                            (uint16_t *)pcmdata.data(), pcmdata.size());

                    v->voiceclient->insert_marker();

                    // TODO: Real-Time Streaming for Better Performance
                }
            }
        }

        if (!strcmp(cmd, ".pause")) {
            dpp::voiceconn *v = event.from->get_voice(event.msg.guild_id);

            uint id = 0, count = 0;
            if (sscanf(data, "%i", &id) < 1)
                id = !v->voiceclient->is_paused();

            if (v && v->voiceclient && v->voiceclient->is_ready())
                v->voiceclient->pause_audio(id);
        }

        if (!strcmp(cmd, ".stop")) {
            dpp::voiceconn *v = event.from->get_voice(event.msg.guild_id);

            if (v && v->voiceclient && v->voiceclient->is_ready())
                v->voiceclient->stop_audio();
        }

        if (!strcmp(cmd, ".skip")) {
            dpp::voiceconn *v = event.from->get_voice(event.msg.guild_id);

            if (v && v->voiceclient && v->voiceclient->is_ready())
                v->voiceclient->skip_to_next_marker();
        }
    };

    // onMessage()
    bot.on_message_create(lOnMessage);

    // Start Bot
    bot.start(dpp::st_wait);

    return 0;
}