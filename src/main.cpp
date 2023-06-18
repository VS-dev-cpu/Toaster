#include <cstdio>
#include <dpp/dpp.h>
// #include <fmt/format.h>
#include <iomanip>
// #include <nlohmann/json.hpp>
#include <sstream>

#include <fstream>
#include <iostream>
#include <mpg123.h>
#include <out123.h>
#include <vector>

// Get Token from File
std::string getToken(std::string path) {
    std::ifstream f(path);

    if (!f.is_open()) {
        fprintf(stderr, "ERROR: Failed to read Token File\n");
        exit(1);
    }

    std::string token((std::istreambuf_iterator<char>(f)),
                      (std::istreambuf_iterator<char>()));

    return token;
}

/* For an example we will hardcode a path to some awesome music here */
int main(int argc, char const *argv[]) {
    /* This will hold the decoded MP3.
     * The D++ library expects PCM format, which are raw sound
     * data, 2 channel stereo, 16 bit signed 48000Hz.
     */
    std::vector<uint8_t> pcmdata;

    /* Setup the bot */
    dpp::cluster bot(getToken("../TOKEN"),
                     dpp::i_default_intents | dpp::i_message_content);

    bot.on_log(dpp::utility::cout_logger());

    /* Use the on_message_create event to look for commands */
    bot.on_message_create([&bot, &pcmdata](const dpp::message_create_t &event) {
        char cmd[64], data[256];
        sscanf(event.msg.content.c_str(), "%s %[^\n]", cmd, data);

        /* Tell the bot to join the discord voice channel the user is on.
         * Syntax: .join */
        if (!strcmp(cmd, ".join")) {
            dpp::guild *g = dpp::find_guild(event.msg.guild_id);
            if (!g->connect_member_voice(event.msg.author.id)) {
                bot.message_create(dpp::message(
                    event.msg.channel_id,
                    "You don't seem to be on a voice channel! :("));
            }
        }

        // Play Music from YouTube
        if (!strcmp(cmd, ".play")) {
            // Download Music
            std::string url = "yt-dlp -x --audio-format mp3 "
                              "https://www.youtube.com/watch?v=" +
                              std::string(data) + " -o audio.mp3";

            system("rm -rf audio.mp3");
            system(url.c_str());

            // Load Music
            pcmdata.clear();
            mpg123_init();

            int err = 0;
            unsigned char *buffer;
            size_t buffer_size, done;
            int channels, encoding;
            long rate;

            /* Note it is important to force the frequency to 48000 for Discord
             * compatibility */
            mpg123_handle *mh = mpg123_new(NULL, &err);
            mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);

            /* Decode entire file into a vector. You could do this on the fly,
             * but if you do that you may get timing issues if your CPU is busy
             * at the time and you are streaming to a lot of channels/guilds.
             */
            buffer_size = mpg123_outblock(mh);
            buffer = new unsigned char[buffer_size];

            /* Note: In a real world bot, this should have some error logging */
            mpg123_open(mh, "audio.mp3");
            mpg123_getformat(mh, &rate, &channels, &encoding);

            unsigned int counter = 0;
            for (int totalBytes = 0;
                 mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK;) {
                for (auto i = 0; i < buffer_size; i++) {
                    pcmdata.push_back(buffer[i]);
                }
                counter += buffer_size;
                totalBytes += done;
            }
            delete buffer;
            mpg123_close(mh);
            mpg123_delete(mh);

            // Play
            dpp::voiceconn *v = event.from->get_voice(event.msg.guild_id);
            if (v && v->voiceclient && v->voiceclient->is_ready()) {
                /* Stream the already decoded MP3 file. This passes the PCM data
                 * to the library to be encoded to OPUS */
                v->voiceclient->send_audio_raw((uint16_t *)pcmdata.data(),
                                               pcmdata.size());
            }
        }
    });

    /* Start bot */
    bot.start(dpp::st_wait);

    /* Clean up */
    mpg123_exit();

    return 0;
}