#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_utils.h"
#include "stats.h"

static int            stats_pivot = 0;
static solve_stats_t *solve_stats = NULL;

#define MAX_STATS 1000

solve_stats_t *get_current_stat() {
    if (solve_stats == NULL)
        return NULL;

    return &solve_stats[stats_pivot];
}

void init_stats() {
    solve_stats = (solve_stats_t *)malloc(sizeof(solve_stats_t) * MAX_STATS);
    memset(solve_stats, 0, sizeof(solve_stats_t) * MAX_STATS);
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

    if (stats_pivot < MAX_STATS) {
        stats_pivot += 1;
    }
}

void dump_stats() {
    if (solve_stats == NULL)
        return;

    ensure_directory_exists("stats");

    FILE *f = fopen("stats/log.csv", "wt");

    for (int i = 0; i < MAX_STATS; i++) {
        solve_stats_t *stats = &solve_stats[i];

        if (stats->used == 0)
            continue;

        fprintf(f, "%d,%d,%d,", stats->phase1_depth, stats->phase2_depth, stats->solution_length);
        fprintf(f, "%d,%d,%d,", stats->phase1_move_count, stats->phase2_move_count, stats->move_count);
        fprintf(f, "%f,%f,%f", stats->phase1_solve_time, stats->phase2_solve_time, stats->solve_time);
        fprintf(f, "\n");
    }

    fclose(f);
}
