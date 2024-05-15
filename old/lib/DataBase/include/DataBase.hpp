#pragma once

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

class DataBase {
  public:
    // Open / Create DataBase
    DataBase(const char *file);

    ~DataBase();

    bool run(const char *op, ...);

  public:
    std::vector<std::string> data;

  private:
    sqlite3 *db;

    int rc = SQLITE_OK;

  private:
    static int callback(void *data, int argc, char **argv, char **azColName);
};