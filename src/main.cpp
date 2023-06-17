#include <dpp/dpp.h>
#include <sqeel.h>

const std::string BOT_TOKEN =
    "ODkyNDY4MTAzMjkwNjkxNjM1.G9NDKd._GbntknqDIV3tu5OScSa0EaWPgFeIt2-d46v9o";

int main() {
    Sqeel sql("users.db");

    // Create Admins Table
    sql.exec("CREATE TABLE IF NOT EXISTS admins (\n"
             "userid INTEGER PRIMARY KEY\n"
             ");");

    // Create Social Credit Table
    sql.exec("CREATE TABLE IF NOT EXISTS social_credit (\n"
             "userid INTEGER PRIMARY KEY,\n"
             "balance REAL NOT NULL\n"
             ");");

    dpp::cluster bot(BOT_TOKEN);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([](const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
    });

    bot.on_ready([&bot](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(
                dpp::slashcommand("ping", "Ping pong!", bot.me.id));
        }
    });

    bot.start(dpp::st_wait);
}