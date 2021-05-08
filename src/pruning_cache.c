/*
 * Copyright <2021> <Renan S Silva, aka h3nnn4n>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "file_utils.h"
#include "pruning_cache.h"
#include "utils.h"

int pruning_table_cache_load(char *cache_name, char *table_name, int **pruning_table, int table_size) {
    char filepath[512];
    snprintf(filepath, 511, "cache/%s/%s", cache_name, table_name);

    if (!file_exists(filepath))
        return 0;

    long start_time = get_microseconds();

    printf("loading: %35s   ", filepath);
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

    long end_time = get_microseconds();
    printf("%9lu bytes loaded in %.4f seconds\n", total_bytes_read, (float)(end_time - start_time) / 1000000.0);

    return 1;
}

void pruning_table_cache_store(char *cache_name, char *table_name, int *pruning_table, int table_size) {
    char filepath[512];
    char cachepath[512];

    snprintf(filepath, 511, "cache/%s/%s", cache_name, table_name);
    snprintf(cachepath, 511, "cache/%s", cache_name);

    long start_time = get_microseconds();
    ensure_directory_exists(cachepath);

    FILE *        f             = fopen(filepath, "wb");
    unsigned long bytes_written = fwrite(pruning_table, sizeof(int), table_size, f);
    fclose(f);

    long end_time = get_microseconds();
    printf("%9lu bytes stored in %s in %.4f seconds\n", bytes_written, filepath,
           (float)(end_time - start_time) / 1000000.0);
}
