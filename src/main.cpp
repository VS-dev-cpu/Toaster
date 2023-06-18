#include <cstdio>
#include <dpp/dpp.h>
// #include <fmt/format.h>
#include <dpp/presence.h>
#include <iomanip>
// #include <nlohmann/json.hpp>
#include <sstream>

#include <dirent.h>
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

void scanForFiles(std::vector<std::string> &files) {
    files.clear();

    // List all Music Files
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("audio/")) != NULL) {
        /* print all the files and directories within directory */
        printf("Audio Files:\n");
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                const char *ext = strrchr(ent->d_name, '.');
                if (!strcmp(ext, ".mp3")) {
                    files.push_back(ent->d_name);
                    printf("%s\n", ent->d_name);
                }
            }
        }
        printf("----\n");
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
        exit(EXIT_FAILURE);
    }
}

/* For an example we will hardcode a path to some awesome music here */
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

    // onMessage()
    bot.on_message_create([&bot, &pcmdata,
                           &files](const dpp::message_create_t &event) {
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
            uint id = 0;
            int n = sscanf(data, "%i", &id);

            if (id >= files.size())
                event.reply("ID out of index!\n");
            else {
                if (n != 1)
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

                /* Decode entire file into a vector. You could do this on the
                 * fly, but if you do that you may get timing issues if your CPU
                 * is busy at the time and you are streaming to a lot of
                 * channels/guilds.
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
                    /* Stream the already decoded MP3 file. This passes the PCM
                     * data to the library to be encoded to OPUS */
                    v->voiceclient->send_audio_raw((uint16_t *)pcmdata.data(),
                                                   pcmdata.size());
                }
            }
        }
    });

    /* Start bot */
    bot.start(dpp::st_wait);

    /* Clean up */
    mpg123_exit();

    return 0;
}