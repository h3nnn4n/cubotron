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

#ifndef _STATS
#define _STATS

#include <stdint.h>

typedef struct {
    int phase1_depth;
    int phase2_depth;
    int solution_length;

    int phase1_move_count;
    int phase2_move_count;
    int phase2_attempts;
    int phase2_successes;
    int solutions_found;
    int die_aborted;

    int   total_moves;
    float total_phase2_time;
    float phase1_solve_time;
    float wall_time;
} solve_stats_t;

typedef struct {
    float min, max, avg, std, p90, p95, p99;
} float_aggregate_t;

typedef struct {
    int   min, max;
    float avg, std, p90, p95, p99;
} int_aggregate_t;

typedef struct {
    int thread_count;

    float_aggregate_t phase1_time;
    float_aggregate_t phase2_time;
    float_aggregate_t wall_time;
    int_aggregate_t   phase1_moves;
    int_aggregate_t   phase2_moves;
    int_aggregate_t   total_moves;
    int_aggregate_t   solutions_found;

    int64_t total_moves_all_threads;
    float   overall_wall_time;
    float   overall_moves_per_second;

    int   total_phase2_attempts;
    int   total_phase2_successes;
    float phase2_success_rate;

    int threads_die_aborted;
    int threads_completed;

    int   solution_lengths_min;
    int   solution_lengths_max;
    float solution_lengths_avg;
    int   solution_lengths_count;
} aggregate_stats_t;

void finalize_solve_stats(solve_stats_t *stats, uint64_t start_us, uint64_t end_us, uint64_t total_phase2_time_us,
                          int die_aborted);

aggregate_stats_t *compute_aggregate_stats(solve_stats_t **thread_stats, int thread_count, const int *solution_lengths,
                                           int n_solution_lengths);

void print_aggregate_stats(const aggregate_stats_t *agg, const solve_stats_t *first_solution);

void           print_solve_stats(const solve_stats_t *stats);
solve_stats_t *get_solve_stats();

#endif
