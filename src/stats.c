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
#include <string.h>

#include "file_utils.h"
#include "mem_utils.h"
#include "stats.h"

#define STARTING_BUFFER_SIZE 500

static int            buffer_size = 0;
static int            stats_pivot = 0;
static solve_stats_t *solve_stats = NULL;

solve_stats_t *get_current_stat() {
    if (solve_stats == NULL)
        return NULL;

    return &solve_stats[stats_pivot];
}

void init_stats() {
    buffer_size = STARTING_BUFFER_SIZE;
    solve_stats = (solve_stats_t *)malloc(sizeof(solve_stats_t) * buffer_size);
    memset_(solve_stats, 0, sizeof(solve_stats_t) * buffer_size);
}

int increase_stats_buffer() {
    int new_size = buffer_size * 2;

    solve_stats_t *new_buffer = (solve_stats_t *)realloc(solve_stats, sizeof(solve_stats_t) * new_size);

    if (new_buffer == NULL) {
        // Should we Panic?
        printf("failed to increase stats buffer size!\n");
        return 0;
    }

    for (int i = buffer_size; i < new_size; i++) {
        solve_stats[i].used = 0;
    }

    buffer_size = new_size;
    solve_stats = new_buffer;

    return 1;
}

void finish_stats() {
    if (solve_stats == NULL)
        return;

    solve_stats[stats_pivot].used = 1;

    solve_stats[stats_pivot].solution_length =
        solve_stats[stats_pivot].phase1_depth + solve_stats[stats_pivot].phase2_depth;

    solve_stats[stats_pivot].move_count =
        solve_stats[stats_pivot].phase1_move_count + solve_stats[stats_pivot].phase2_move_count;

    solve_stats[stats_pivot].solve_time =
        solve_stats[stats_pivot].phase1_solve_time + solve_stats[stats_pivot].phase2_solve_time;

    if (stats_pivot < buffer_size) {
        stats_pivot += 1;
    } else {
        if (increase_stats_buffer()) {
            stats_pivot += 1;
        }
    }
}

void dump_stats() {
    if (solve_stats == NULL)
        return;

    ensure_directory_exists("stats");

    FILE *f = fopen("stats/log.csv", "wt");

    fprintf(f, "phase1_solution_length,phase2_solution_length,solution_length,");
    fprintf(f, "phase1_move_count,phase2_move_count,move_count,");
    fprintf(f, "phase1_solve_time,phase2_solve_time,solve_time");
    fprintf(f, "\n");

    for (int i = 0; i < buffer_size; i++) {
        solve_stats_t *stats = &solve_stats[i];

        // FIXME: Somehow we are storing empty entries in the buffer. For now
        // lets just skip over it
        if (stats->used == 0)
            continue;

        fprintf(f, "%d,%d,%d,", stats->phase1_depth, stats->phase2_depth, stats->solution_length);
        fprintf(f, "%d,%d,%d,", stats->phase1_move_count, stats->phase2_move_count, stats->move_count);
        fprintf(f, "%f,%f,%f", stats->phase1_solve_time, stats->phase2_solve_time, stats->solve_time);
        fprintf(f, "\n");
    }

    fclose(f);
}
