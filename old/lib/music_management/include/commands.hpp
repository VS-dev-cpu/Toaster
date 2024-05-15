#pragma once

#include <DataBase.hpp>
#include <cstdio>
#include <string>

#define MUSIC_PROPERTY_ID 0
#define MUSIC_PROPERTY_AUTHOR 1
#define MUSIC_PROPERTY_TITLE 2
#define MUSIC_PROPERTY_PATH 3
#define MUSIC_PROPERTY_URL 4
#define MUSIC_PROPERTY_TAGS 5

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

bool dbUpdate(DataBase &db, int id, int property, const char *value) {
    switch (property) {
    case MUSIC_PROPERTY_AUTHOR:
        break;

    case MUSIC_PROPERTY_TITLE:
        break;

    case MUSIC_PROPERTY_PATH:
        break;

    case MUSIC_PROPERTY_URL:
        break;

    case MUSIC_PROPERTY_TAGS:
        break;

    default:
        fprintf(stderr, "DATABASE WARNING: Invalid Music Property ID\n");
        return false;
        break;
    }

    return true;
}