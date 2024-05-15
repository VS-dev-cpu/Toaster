#include <commands.hpp>

// bool updateMusic(DataBase &db, )

int main() {
    DataBase db("tracks.db");

    dbInit(db);

    int id =
        dbInsert(db, "eve", "tokyo ghetto", "", "https://youtu.be/PvzBWFGEz8M");

    printf("%s\n", db.data[0].c_str());

    // db.execute("INSERT OR REPLACE INTO social_credit (userid, "
    //            "balance) VALUES (%i, COALESCE((SELECT balance "
    //            "FROM social_credit WHERE userid = %i), 0) + %i)",
    //            1, 1, 2);

    // db.execute("SELECT balance FROM social_credit WHERE userid = %i", 1);

    return 0;
}