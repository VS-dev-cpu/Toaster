#include <dpp/dpp.h>
#include <dpp/presence.h>

#include <util.hpp>

int main(int argc, char const *argv[]) {

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
    };

    // Discord Bot
    dpp::cluster bot(getToken("../../Toaster/TOKEN"),
                     dpp::i_default_intents | dpp::i_message_content);
    bot.on_log(dpp::utility::cout_logger());

    bot.on_message_create(onMessage);

    bot.start(dpp::st_wait);

    return 0;
}