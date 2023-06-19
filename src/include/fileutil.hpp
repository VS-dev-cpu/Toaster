#pragma once

#include <dirent.h>
#include <string.h>

#include <fstream>
#include <string>
#include <vector>

// Get Token from File
std::string getToken(std::string path) {
    std::ifstream f(path);

    if (!f.is_open()) {
        fprintf(stderr, "ERROR: Failed to read Token File\n");
        exit(1);
    }

    std::string token((std::istreambuf_iterator<char>(f)),
                      (std::istreambuf_iterator<char>()));

    return token;
}

void scanForFiles(std::vector<std::string> &files) {
    files.clear();

    // List all Music Files
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("audio/")) != NULL) {
        /* print all the files and directories within directory */
        printf("Audio Files:\n");
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                const char *ext = strrchr(ent->d_name, '.');
                if (!strcmp(ext, ".mp3")) {
                    files.push_back(ent->d_name);
                    printf("%s\n", ent->d_name);
                }
            }
        }
        printf("----\n");
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
        exit(EXIT_FAILURE);
    }
}