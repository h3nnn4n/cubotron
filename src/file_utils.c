#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "file_utils.h"

void ensure_directory_exists(char *directory) {
    struct stat st = {0};

    if (stat("/some/directory", &st) == -1)
        mkdir(directory, 0777);
}

int file_exists(char *filepath) {
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}
