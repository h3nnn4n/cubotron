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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "file_utils.h"
#include "pruning_cache.h"
#include "utils.h"

int pruning_table_cache_load(const char *cache_name, const char *table_name, int **pruning_table, int table_size) {
    char filepath[512];
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    snprintf(filepath, sizeof(filepath), "cache/%s/%s", cache_name, table_name);

    if (!file_exists(filepath))
        return 0;

    uint32_t start_time = get_microseconds();

    // printf("loading: %-45s ", filepath);
    fflush(stdout);
    FILE *f = fopen(filepath, "rb");

    *pruning_table = (int *)malloc(sizeof(int) * table_size);

    size_t elements_read = fread(*pruning_table, sizeof(int), table_size, f);
    fclose(f);

    uint32_t bytes_read = (uint32_t)(elements_read * sizeof(int));
    uint32_t end_time   = get_microseconds();
    // printf("%10u bytes loaded in %6.4f seconds\n", bytes_read, (float)(end_time - start_time) / 1000000.0);

    return 1;
}

void pruning_table_cache_store(const char *cache_name, const char *table_name, const int *pruning_table,
                               int table_size) {
    char filepath[512];
    char cachepath[512];

    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    snprintf(filepath, sizeof(filepath), "cache/%s/%s", cache_name, table_name);
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    snprintf(cachepath, sizeof(cachepath), "cache/%s", cache_name);

    uint32_t start_time = get_microseconds();
    ensure_directory_exists(cachepath);

    FILE  *f                = fopen(filepath, "wb");
    size_t elements_written = fwrite(pruning_table, sizeof(int), table_size, f);
    fclose(f);

    uint32_t bytes_written = (uint32_t)(elements_written * sizeof(int));
    uint32_t end_time      = get_microseconds();
    printf("storing: %-45s %10u bytes stored in %6.4f seconds\n", filepath, bytes_written,
           (float)(end_time - start_time) / 1000000.0);
}
