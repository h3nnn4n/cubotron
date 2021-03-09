#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "file_utils.h"

void ensure_directory_exists(char *directory) {
    if (file_exists(directory)) {
        return;
    }

    mkdir_p(directory);
}

int file_exists(char *filepath) {
    struct stat buffer = {0};

    return (stat(filepath, &buffer) == 0);
}

// https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
int mkdir_p(const char *path) {
    assert(path != NULL);

    const size_t len = strlen(path);
    char         _path[PATH_MAX];
    char *       p;

    errno = 0;

    if (len > sizeof(_path) - 1) {
        errno = ENAMETOOLONG;
        return -1;
    }

    strcpy(_path, path);

    for (p = _path + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';

            if (mkdir(_path, S_IRWXU) != 0) {
                if (errno != EEXIST)
                    return -1;
            }

            *p = '/';
        }
    }

    if (mkdir(_path, S_IRWXU) != 0) {
        if (errno != EEXIST)
            return -1;
    }

    return 0;
}
