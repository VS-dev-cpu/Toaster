#include <DataBase.hpp>
#include <cstdio>

int main() {
    DataBase db("test.db");
    db.execute("CREATE TABLE IF NOT EXISTS social_credit (userid INTEGER "
               "PRIMARY KEY,balance REAL NOT NULL);");

    db.execute("INSERT OR REPLACE INTO social_credit (userid, "
               "balance) VALUES (%i, COALESCE((SELECT balance "
               "FROM social_credit WHERE userid = %i), 0) + %i)",
               1, 1, 2);

    printf(
        "%s\n",
        db.execute("SELECT balance FROM social_credit WHERE userid = %i", 1));

    return 0;
}