#include <cstdlib>
#include <dpp/dpp.h>
#include <dpp/presence.h>

#include <mpg123.h>
#include <out123.h>

#include <DataBase.hpp>

#include <fileutil.hpp>

void onMessage(const dpp::message_create_t &event) {}

int main(int argc, char const *argv[]) {
    std::vector<std::string> files;
    srand(time(0));

    scanForFiles(files);

    // RAW PCM: data, 2 channel stereo, 16 bit signed 48000Hz.
    std::vector<uint8_t> pcmdata;

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

                for (auto f : files)
                    list += f + "\n";

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

                // Load Music
                pcmdata.clear();
                mpg123_init();

                int err = 0;
                unsigned char *buffer;
                size_t buffer_size, done;
                int channels, encoding;
                long rate;

                /* Note it is important to force the frequency to 48000 for
                 * Discord compatibility */
                mpg123_handle *mh = mpg123_new(NULL, &err);
                mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);

                /* Decode entire file into a vector. You could do this on
                 * the fly, but if you do that you may get timing issues if
                 * your CPU is busy at the time and you are streaming to a
                 * lot of channels/guilds.
                 */
                buffer_size = mpg123_outblock(mh);
                buffer = new unsigned char[buffer_size];

                /* Note: In a real world bot, this should have some error
                 * logging */
                mpg123_open(mh, ("audio/" + files[id]).c_str());
                mpg123_getformat(mh, &rate, &channels, &encoding);

                unsigned int counter = 0;
                for (int totalBytes = 0; mpg123_read(mh, buffer, buffer_size,
                                                     &done) == MPG123_OK;) {
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
                    /* Stream the already decoded MP3 file. This passes the
                     * PCM data to the library to be encoded to OPUS */
                    for (int i = 0; i < count; i++)
                        v->voiceclient->send_audio_raw(
                            (uint16_t *)pcmdata.data(), pcmdata.size());

                    // TODO: Real-Time Streaming for Better Performance
                }
            }
        }
    };

    // onMessage()
    bot.on_message_create(lOnMessage);

    // Start Bot
    bot.start(dpp::st_wait);

    return 0;
}