#pragma once

#include <sqlite3.h>
#include <stdio.h>

class Sqeel {
  public:
    Sqeel(const char *file) {
        int rc = sqlite3_open(file, &db);

        if (rc)
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        else
            fprintf(stdout, "Opened database successfully\n");
    }

    ~Sqeel() { sqlite3_close(db); }

    void exec(const char *op) { sqlite3_exec(db, op, nullptr, 0, nullptr); }

    int getCredit(int userid) {}

  private:
    sqlite3 *db;
};