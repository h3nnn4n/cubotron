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

#ifndef _PRUNING_CACHE
#define _PRUNING_CACHE

#include <stdint.h>

int  pruning_table_cache_load(const char *cache_name, const char *table_name, int **pruning_table, int table_size);
void pruning_table_cache_store(const char *cache_name, const char *table_name, const int *pruning_table,
                               int table_size);

int  pruning_table_cache_load_u32(const char *cache_name, const char *table_name, uint32_t **pruning_table,
                                  int table_size);
void pruning_table_cache_store_u32(const char *cache_name, const char *table_name, const uint32_t *pruning_table,
                                   int table_size);

int  pruning_table_cache_load_u8(const char *cache_name, const char *table_name, uint8_t **pruning_table,
                                 int table_size);
void pruning_table_cache_store_u8(const char *cache_name, const char *table_name, const uint8_t *pruning_table,
                                  int table_size);

#endif /* end of include guard */
