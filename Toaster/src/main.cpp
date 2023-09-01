#include <dpp/dpp.h>
#include <dpp/presence.h>

int main(int argc, char const *argv[]) {

    // Setup Bot
    dpp::cluster bot(getToken("../TOKEN"),
                     dpp::i_default_intents | dpp::i_message_content);

    bot.on_log(dpp::utility::cout_logger());

    // onMessage()
    bot.on_message_create([&](const dpp::message_create_t &event) {
        char cmd[64], data[256];
        sscanf(event.msg.content.c_str(), "%s %[^\n]", cmd, data);

        dpp::user author = event.msg.author;

        // Join Voice Channel
        if (!strcmp(cmd, ".join")) {
            dpp::guild *g = dpp::find_guild(event.msg.guild_id);
            if (!g->connect_member_voice(event.msg.author.id)) {
                event.reply(dpp::message(
                    event.msg.channel_id,
                    "You don't seem to be on a voice channel! :("));
            }
        }

        // Get Music
        if (!strcmp(cmd, ".get")) {
            // Download Music

            std::string id;
            int err = ytdlp(data, id);
        }

        // // List Musics
        // if (!strcmp(cmd, ".list")) {
        //     scanForFiles(files);

        //     if (!strcmp(data, "number"))
        //         event.reply("There are " + std::to_string(files.size()) +
        //                     " Audio Files Available");
        //     else {
        //         std::string list;

        //         for (size_t f = 0; f < files.size(); f++)
        //             list += std::to_string(f) + ": " + files[f] + "\n";

        //         event.reply("Available Audio Files:\n" + list);
        //     }
        // }

        // // Play Music from YouTube
        // if (!strcmp(cmd, ".play")) {
        //     uint id = 0;
        //     if (sscanf(data, "%i", &id) < 1)
        //         id = rand() % files.size();

        //     if (id >= files.size())
        //         event.reply("ID out of index!\n");
        //     else {
        //         event.reply("Playing: " + files[id]);

        //         std::vector<uint8_t> pcmdata;
        //         loadMusic(pcmdata, ("audio/" + files[id]).c_str());

        //         dpp::voiceconn *v =
        //         event.from->get_voice(event.msg.guild_id); if (v &&
        //         v->voiceclient && v->voiceclient->is_ready()) {
        //             v->voiceclient->send_audio_raw((uint16_t
        //             *)pcmdata.data(),
        //                                            pcmdata.size());

        //             v->voiceclient->insert_marker();
        //         }
        //     }
        // }

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
    });

    // Start Bot
    bot.start(dpp::st_wait);

    return 0;
}