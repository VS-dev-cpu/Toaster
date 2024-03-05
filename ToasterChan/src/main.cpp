#include <cstdlib>
#include <cstring>
#include <string>

#include <dpp/dpp.h>

#include <DataBase.hpp>
#include <util.hpp>

int main(int argc, char const *argv[]) {
    // Set Random Seed
    srand(time(0));
    // Attempt to update yt-dlp
    system((std::string(PROJECT_ROOT_DIR) + "/lib/yt-dlp -U").c_str());

    // Init DataBases
    DataBase tracks("tracks.db"); // Database of every downloaded music
    tracks.run("CREATE TABLE IF NOT EXISTS music ("
               "id TEXT PRIMARY KEY,"   // // Youtube ID (also file name)
               "creator TEXT NOT NULL," // User who added it
               "title TEXT NOT NULL"    // Title
               ");");

    // DataBase playlists("playlists.db"); // Database of playlists
    // playlists.run("CREATE TABLE IF NOT EXISTS playlists ("
    //               "id INTEGER PRIMARY KEY," // Playlist ID
    //               "owner TEXT NOT NULL,"    // User who owns playlist
    //               "name TEXT NOT NULL,"     // Name of the playlist
    //               ");");

    // onMessage Callback
    auto onMessage = [&](const dpp::message_create_t &event) {
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
        // if (!strcmp(cmd, ".get")) {
        //     // Download Music

        //     std::string id;
        //     int err = ytdlp(data, id);

        //     if (!err)
        //         err = !tracks.run("INSERT INTO music (id, creator, title)"
        //                           "VALUES ('%s', '%s', '%s');",
        //                           author.username.c_str(), "todo",
        //                           id.c_str());

        //     if (err)
        //         event.reply("Sorry, i couldn't get this one!");
        //     else
        //         event.reply("Done!");
        // }

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

        // Play Music
        if (!strcmp(cmd, ".play")) {
            // TODO: music from playlist
            // bool type = false;
            // char source[64];
            // int count = sscanf(data, "%63s", source);

            // if (sscanf(data, "%63s", source) <= 0)
            //     event.reply("That's not enough information to play "
            //                 "something!"); // TODO: play random
            // else {
            //     // TODO: check playlist, then music
            // }

            // std::string id;
            // ytdlp(data, id);

            system((std::string(PROJECT_ROOT_DIR) +
                    "/lib/yt-dlp -x --audio-format mp3 "
                    "--output 'audio/music' ytsearch:'" +
                    data + "'")
                       .c_str());

            event.reply("Playing...");

            std::vector<uint8_t> pcmdata;
            loadMusic(pcmdata, "audio/music.mp3");

            system("rm -r audio/music.mp3");

            dpp::voiceconn *v = event.from->get_voice(event.msg.guild_id);
            if (v && v->voiceclient && v->voiceclient->is_ready()) {
                v->voiceclient->send_audio_raw((uint16_t *)pcmdata.data(),
                                               pcmdata.size());

                v->voiceclient->insert_marker();
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

    // Discord Bot
    dpp::cluster bot( //
        getenv("TOKEN_MOCHIMIX"),
        dpp::i_default_intents | dpp::i_message_content);
    bot.on_log(dpp::utility::cout_logger());

    bot.on_message_create(onMessage);

    bot.start(dpp::st_wait);

    return 0;
}