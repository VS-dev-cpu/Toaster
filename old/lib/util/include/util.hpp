#pragma once

#include <mpg123.h>
#include <out123.h>

#include <dirent.h>
#include <string.h>

#include <fstream>
#include <string>
#include <vector>

#include <stdarg.h>

#include <config.h>

// Get Lower Number
inline int min(int a, int b) {
    if (a < b)
        return a;
    return b;
}

// Get Higher Number
inline int max(int a, int b) {
    if (a > b)
        return a;
    return b;
}

// system() with va_args
int run(const char *text, ...);

// Download Music
int ytdlp(std::string search, std::string &id);

// Load Music From File
bool loadMusic(std::vector<uint8_t> &data, const char *path);

// Scan Files in Folder
void scanForFiles(std::vector<std::string> &files, std::string folder,
                  std::vector<std::string> exts = {});
