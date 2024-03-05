#include <dpp/dpp.h>

#include <DataBase.hpp>
#include <util.hpp>

bool dbInit(DataBase &db) {
    return db.run("CREATE TABLE IF NOT EXISTS music ("
                  "id INTEGER PRIMARY KEY,"

                  "author TEXT,"
                  "title TEXT NOT NULL,"

                  "path TEXT,"
                  "url TEXT,"

                  "tags TEXT"
                  ");");
}

int dbInsert(DataBase &db, const char *author, const char *title,
             const char *path = "", const char *url = "",
             const char *tags = "") {
    if (author == nullptr || strlen(author) == 0) {
        fprintf(stderr, "DATABASE WARNING: Author is not set\n");
        author = "";
    }

    if (title == nullptr || strlen(title) == 0) {
        fprintf(stderr, "DATABASE ERROR: TITLE is not set\n");
        return 0;
    }

    if (path == nullptr)
        path = "";

    if (url == nullptr)
        url = "";

    if (tags == nullptr)
        tags = "";

    const size_t len = strlen("x.mp3");
    if (strlen(path) < len && strlen(url) < len) {
        fprintf(stderr, "DATABASE ERROR: SOURCE is not set\n");
        return 0;
    }

    bool success = db.run("INSERT INTO music(author, title, path, url, tags)"
                          "VALUES('%s', '%s', '%s', '%s', '%s');",
                          author, title, path, url, tags);

    if (!success) {
        fprintf(stderr, "DATABASE ERROR: Failed to insert music\n");
        return 0;
    }

    if (!db.run("SELECT last_insert_rowid()"))
        fprintf(stderr, "DATABASE WARNING: Failed to get insert ID\n");

    if (db.data.size() > 0)
        return std::stoi(db.data[0]);
    return 0;
}

int main(int argc, char const *argv[]) {
    DataBase db("toast.db");
    db.run("CREATE TABLE IF NOT EXISTS userdata ("
           "username TEXT PRIMARY KEY,"
           "credit NUMBER"
           ");");

    auto onMessage = [&](const dpp::message_create_t &event) {
        char cmd[64], data[256];
        sscanf(event.msg.content.c_str(), "%s %[^\n]", cmd, data);

        dpp::user author = event.msg.author;

        // Get Credit
        if (!strcmp(cmd, ".get")) {
            db.run("SELECT credit FROM userdata WHERE username = '%s'",
                   author.username.c_str());
            event.reply("You have " + db.data[0] + " Credits");
        } else {
            db.run("INSERT OR IGNORE INTO userdata (username, credit) VALUES "
                   "('%s', 0) ",
                   author.username.c_str());
            db.run("UPDATE userdata SET credit = credit + %i WHERE username = "
                   "'%s'",
                   strlen(event.msg.content.c_str()), author.username.c_str());
        }
    };

    // Discord Bot
    dpp::cluster bot(getenv("TOKEN_TOASTER"),
                     dpp::i_default_intents | dpp::i_message_content);
    bot.on_log(dpp::utility::cout_logger());

    bot.on_message_create(onMessage);

    // Slash Command
    bot.on_slashcommand([&bot](const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "vote") {
            // Check if user can create votes
            const dpp::snowflake role_id(1190955086449492082);
            const auto roles = event.command.member.get_roles();
            bool has_role = false;
            for (auto role : roles) {
                if (role == role_id) {
                    has_role = true;
                    break;
                }
            }

            if (!has_role) {
                dpp::message m;
                m.set_content("You can not start votes!")
                    .set_flags(dpp::m_ephemeral);

                event.reply(m);
                return;
            }

            // Read Vote File

            // Start Vote
            // TODO

            dpp::message msg(event.command.channel_id, "Click to vote!");

            /* Add an action row, and then a button within the action row. */
            msg.add_component(dpp::component().add_component(
                dpp::component()
                    .set_label("Vote!!")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_primary)
                    .set_id("vote_minecraft_version")));

            /* Reply to the user with our message. */
            event.reply(msg);
        }
        //  else if (event.command.get_command_name() == "start") {
        //
        // }
    });

    bot.on_button_click([&bot](const dpp::button_click_t &event) {
        if (event.custom_id == "vote_minecraft_version") {
            // Check if user has already voted
            // TODO
            if (!true) {
                dpp::message m;
                m.set_content("You have already voted!")
                    .set_flags(dpp::m_ephemeral);

                event.reply(m);
                return;
            }

            dpp::interaction_modal_response modal("my_modal",
                                                  "Vote Minecraft Version");

            modal.add_component(
                dpp::component()
                    // .set_label("What version should the server be?")
                    .set_label("version")
                    .set_id("field_id2")
                    .set_type(dpp::cot_text)
                    .set_placeholder("1.16.5")
                    .set_min_length(3)
                    .set_max_length(6)
                    .set_text_style(dpp::text_short)
                    .set_required(true));

            modal.add_row();

            modal.add_component(
                dpp::component()
                    // .set_label("Do you want the server to be modded? If so, "
                    //            "list a few mods!")
                    .set_label("Modded?")
                    .set_id("field_id3")
                    .set_type(dpp::cot_text)
                    .set_placeholder("1.16.5")
                    .set_max_length(2000)
                    .set_text_style(dpp::text_paragraph));

            /* Trigger the dialog box. All dialog boxes are ephemeral */
            event.dialog(modal);
        }
    });

    bot.on_form_submit([](const dpp::form_submit_t &event) {
        /* For this simple example, we know the first element of the first row
         * ([0][0]) is value type string. In the real world, it may not be safe
         * to make such assumptions!
         */
        std::string v =
            std::get<std::string>(event.components[0].components[0].value);

        dpp::message m;
        m.set_content("You entered: " + v).set_flags(dpp::m_ephemeral);

        /* Emit a reply. Form submission is still an interaction and must
         * generate some form of reply! */
        event.reply(m);
    });

    bot.on_ready([&bot](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            // Clear Commands
            // TODO remove for production
            // bot.global_bulk_command_delete();

            bot.global_command_create(dpp::slashcommand(
                "vote", "Send a message with a button!", bot.me.id));

            bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_custom,
                                           "Preparing ToasterMC"));
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}