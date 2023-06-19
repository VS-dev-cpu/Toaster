#pragma once

#include <cstring>
#include <sqlite3.h>
#include <stdio.h>
#include <string>

class DataBase {
  public:
    // Open / Create DataBase
    DataBase(const char *file);

    ~DataBase();

    const char *execute(const char *op, ...);

  private:
    sqlite3 *db;

    int rc = SQLITE_OK;

    std::string data;

  private:
    static int callback(void *data, int argc, char **argv, char **azColName);
};