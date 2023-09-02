#include <dpp/dpp.h>
#include <dpp/presence.h>

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
    dpp::cluster bot(getToken("../../Toaster/TOKEN"),
                     dpp::i_default_intents | dpp::i_message_content);
    bot.on_log(dpp::utility::cout_logger());

    bot.on_message_create(onMessage);

    bot.start(dpp::st_wait);

    return 0;
}