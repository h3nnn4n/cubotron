#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_utils.h"
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
    memset(solve_stats, 0, sizeof(solve_stats_t) * buffer_size);
}

int increase_stats_buffer() {
    int new_size = buffer_size * 2;

    solve_stats_t *new_buffer = (solve_stats_t *)realloc(solve_stats, new_size);

    if (new_buffer == NULL || (int)new_buffer < 0) {
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

    for (int i = 0; i < stats_pivot; i++) {
        solve_stats_t *stats = &solve_stats[i];

        fprintf(f, "%d,%d,%d,", stats->phase1_depth, stats->phase2_depth, stats->solution_length);
        fprintf(f, "%d,%d,%d,", stats->phase1_move_count, stats->phase2_move_count, stats->move_count);
        fprintf(f, "%f,%f,%f", stats->phase1_solve_time, stats->phase2_solve_time, stats->solve_time);
        fprintf(f, "\n");
    }

    fclose(f);
}
