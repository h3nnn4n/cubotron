#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "pruning_cache.h"

int pruning_table_cache_load(char *cache_name, char *table_name, int **pruning_table, int table_size) {
    char filepath[512];
    snprintf(filepath, 511, "%s/%s", cache_name, table_name);

    if (!file_exists(filepath))
        return 0;

    printf("loading: %s   ", filepath);
    fflush(stdout);
    FILE *f = fopen(filepath, "rb");

    *pruning_table = (int *)malloc(sizeof(int) * table_size);

    unsigned long bytes_read       = -1;
    unsigned long total_bytes_read = 0;

    do {
        bytes_read = fread(*pruning_table, sizeof(int), table_size, f);
        total_bytes_read += bytes_read;
    } while (bytes_read > 0);

    fclose(f);

    printf("%lu bytes loaded\n", total_bytes_read);

    return 1;
}

void pruning_table_cache_store(char *cache_name, char *table_name, int *pruning_table, int table_size) {
    char filepath[512];
    snprintf(filepath, 511, "%s/%s", cache_name, table_name);

    FILE *        f             = fopen(filepath, "wb");
    unsigned long bytes_written = fwrite(pruning_table, sizeof(int), table_size, f);
    fclose(f);

    printf("%lu bytes stored in %s\n", bytes_written, filepath);
}

int file_exists(char *filepath) {
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}
