#pragma once

#include <fstream>
#include <string>

// Get Token from File
std::string getToken(std::string path) {
    std::ifstream f(path);

    if (!f.is_open()) {
        perror("getToken()");
        fprintf(stderr, "Failed to open file\n");
        exit(1);
    }

    std::string token((std::istreambuf_iterator<char>(f)),
                      (std::istreambuf_iterator<char>()));

    return token;
}

char *loadPrompt(const char *path) {
    // Open File
    FILE *f = NULL;
    if (!(f = fopen(path, "r"))) {
        printf("Failed to open file: %s\n", path);
        return NULL;
    }

    // Get File Size
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Read File
    char *prompt = (char *)malloc(sizeof(char) * size);

    if (!fread(prompt, size, 1, f)) {
        printf("Failed to read file: %s\n", path);
        free(prompt);
        return NULL;
    }

    return prompt;
}