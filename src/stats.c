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

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stats.h"
#include "stats_math.h"

void finalize_solve_stats(solve_stats_t *stats, uint64_t start_us,
                          uint64_t end_us, uint64_t total_phase2_time_us,
                          int die_aborted) {
    stats->die_aborted     = die_aborted;
    stats->wall_time       = (float)(end_us - start_us) / 1000000.0f;
    stats->total_phase2_time = (float)total_phase2_time_us / 1000000.0f;
    stats->phase1_solve_time = stats->wall_time - stats->total_phase2_time;
    stats->total_moves       = stats->phase1_move_count + stats->phase2_move_count;
}

static void extract_float(solve_stats_t **thread_stats, int n,
                          float *out, size_t field_offset) {
    for (int i = 0; i < n; i++) {
        out[i] = *(float *)((char *)thread_stats[i] + field_offset);
    }
}

static void extract_int(solve_stats_t **thread_stats, int n,
                        int *out, size_t field_offset) {
    for (int i = 0; i < n; i++) {
        out[i] = *(int *)((char *)thread_stats[i] + field_offset);
    }
}

aggregate_stats_t *compute_aggregate_stats(solve_stats_t **thread_stats,
                                           int thread_count,
                                           const int *solution_lengths,
                                            int n_solution_lengths) {
    aggregate_stats_t *agg = calloc(1, sizeof(aggregate_stats_t));
    assert(agg != NULL);

    agg->thread_count = thread_count;

    if (thread_count == 0) {
        agg->solution_lengths_min   = 0;
        agg->solution_lengths_max   = 0;
        agg->solution_lengths_avg   = 0.0f;
        agg->solution_lengths_count = 0;
        return agg;
    }

    float buf_f[thread_count];
    int   buf_i[thread_count];

    extract_float(thread_stats, thread_count, buf_f, offsetof(solve_stats_t, phase1_solve_time));
    compute_float_aggregate(buf_f, thread_count, &agg->phase1_time.min, &agg->phase1_time.max,
                            &agg->phase1_time.avg, &agg->phase1_time.std,
                             &agg->phase1_time.p90, &agg->phase1_time.p95, &agg->phase1_time.p99);

    extract_float(thread_stats, thread_count, buf_f, offsetof(solve_stats_t, total_phase2_time));
    compute_float_aggregate(buf_f, thread_count, &agg->phase2_time.min, &agg->phase2_time.max,
                            &agg->phase2_time.avg, &agg->phase2_time.std,
                             &agg->phase2_time.p90, &agg->phase2_time.p95, &agg->phase2_time.p99);

    extract_float(thread_stats, thread_count, buf_f, offsetof(solve_stats_t, wall_time));
    compute_float_aggregate(buf_f, thread_count, &agg->wall_time.min, &agg->wall_time.max,
                            &agg->wall_time.avg, &agg->wall_time.std,
                             &agg->wall_time.p90, &agg->wall_time.p95, &agg->wall_time.p99);

    extract_int(thread_stats, thread_count, buf_i, offsetof(solve_stats_t, phase1_move_count));
    compute_int_aggregate(buf_i, thread_count, &agg->phase1_moves.min, &agg->phase1_moves.max,
                          &agg->phase1_moves.avg, &agg->phase1_moves.std,
                          &agg->phase1_moves.p90, &agg->phase1_moves.p95, &agg->phase1_moves.p99);

    extract_int(thread_stats, thread_count, buf_i, offsetof(solve_stats_t, phase2_move_count));
    compute_int_aggregate(buf_i, thread_count, &agg->phase2_moves.min, &agg->phase2_moves.max,
                          &agg->phase2_moves.avg, &agg->phase2_moves.std,
                          &agg->phase2_moves.p90, &agg->phase2_moves.p95, &agg->phase2_moves.p99);

    extract_int(thread_stats, thread_count, buf_i, offsetof(solve_stats_t, total_moves));
    compute_int_aggregate(buf_i, thread_count, &agg->total_moves.min, &agg->total_moves.max,
                          &agg->total_moves.avg, &agg->total_moves.std,
                          &agg->total_moves.p90, &agg->total_moves.p95, &agg->total_moves.p99);

    extract_int(thread_stats, thread_count, buf_i, offsetof(solve_stats_t, solutions_found));
    compute_int_aggregate(buf_i, thread_count, &agg->solutions_found.min, &agg->solutions_found.max,
                          &agg->solutions_found.avg, &agg->solutions_found.std,
                          &agg->solutions_found.p90, &agg->solutions_found.p95, &agg->solutions_found.p99);

    int64_t total_m = 0;
    int total_p2a = 0, total_p2s = 0;
    int died = 0;
    float max_wall = 0.0f;
    for (int i = 0; i < thread_count; i++) {
        solve_stats_t *s = thread_stats[i];
        total_m  += s->total_moves;
        total_p2a += s->phase2_attempts;
        total_p2s += s->phase2_successes;
        died     += s->die_aborted;
        if (s->wall_time > max_wall) max_wall = s->wall_time;
    }

    agg->total_moves_all_threads  = total_m;
    agg->overall_wall_time        = max_wall;
    agg->overall_moves_per_second = max_wall > 0.0f ? (float)total_m / max_wall : 0.0f;
    agg->total_phase2_attempts    = total_p2a;
    agg->total_phase2_successes   = total_p2s;
    agg->phase2_success_rate      = total_p2a > 0
                                        ? (float)total_p2s / (float)total_p2a * 100.0f
                                        : 0.0f;
    agg->threads_die_aborted      = died;
    agg->threads_completed        = thread_count - died;

    if (n_solution_lengths > 0) {
        float *len_f = malloc(sizeof(float) * n_solution_lengths);
        assert(len_f != NULL);
        int min_len = solution_lengths[0];
        int max_len = solution_lengths[0];
        double sum_len = 0;
        for (int i = 0; i < n_solution_lengths; i++) {
            int l = solution_lengths[i];
            len_f[i] = (float)l;
            if (l < min_len) min_len = l;
            if (l > max_len) max_len = l;
            sum_len += l;
        }
        agg->solution_lengths_min   = min_len;
        agg->solution_lengths_max   = max_len;
        agg->solution_lengths_avg   = (float)(sum_len / n_solution_lengths);
        agg->solution_lengths_count = n_solution_lengths;
        free(len_f);
    } else {
        agg->solution_lengths_min   = 0;
        agg->solution_lengths_max   = 0;
        agg->solution_lengths_avg   = 0.0f;
        agg->solution_lengths_count = 0;
    }

    return agg;
}

static void print_float_aggregate(const char *label, const float_aggregate_t *a) {
    printf("  %s:  min=%.6f  avg=%.6f  std=%.6f  p90=%.6f  p95=%.6f  p99=%.6f  max=%.6f\n",
           label, a->min, a->avg, a->std, a->p90, a->p95, a->p99, a->max);
}

static void print_int_aggregate(const char *label, const int_aggregate_t *a) {
    printf("  %s:  min=%d  avg=%.1f  std=%.1f  p90=%.0f  p95=%.0f  p99=%.0f  max=%d\n",
           label, a->min, a->avg, a->std, a->p90, a->p95, a->p99, a->max);
}

void print_aggregate_stats(const aggregate_stats_t *agg,
                           const solve_stats_t *first_solution) {
    printf("Solve stats:\n");

    if (first_solution != NULL) {
        printf("  Solution length: %d\n", first_solution->solution_length);
        printf("  Phase 1 depth:   %d\n", first_solution->phase1_depth);
        printf("  Phase 2 depth:   %d\n", first_solution->phase2_depth);
        printf("\n");
    }

    print_float_aggregate("Phase 1 time (s)", &agg->phase1_time);
    print_float_aggregate("Phase 2 time (s)", &agg->phase2_time);
    print_float_aggregate("Wall time (s)   ",    &agg->wall_time);
    printf("\n");

    print_int_aggregate("Phase 1 moves",  &agg->phase1_moves);
    print_int_aggregate("Phase 2 moves",  &agg->phase2_moves);
    print_int_aggregate("Total moves  ",    &agg->total_moves);
    printf("\n");

    print_int_aggregate("Solutions found (per thread)", &agg->solutions_found);
    printf("\n");

    printf("  Total moves (all threads): %lld\n", (long long)agg->total_moves_all_threads);
    printf("  Moves per second:          %.2f\n", agg->overall_moves_per_second);
    printf("  Wall time:                 %.6f s\n", agg->overall_wall_time);
    printf("\n");

    printf("  Phase 2 attempts:     %d\n", agg->total_phase2_attempts);
    printf("  Phase 2 successes:    %d\n", agg->total_phase2_successes);
    printf("  Phase 2 success rate: %.1f%%\n", agg->phase2_success_rate);
    printf("\n");

    printf("  Threads die-aborted: %d\n", agg->threads_die_aborted);
    printf("  Threads completed:   %d\n", agg->threads_completed);
    printf("\n");

    printf("  Solution lengths: min=%d  max=%d  avg=%.1f  (count=%d)\n",
           agg->solution_lengths_min, agg->solution_lengths_max,
           agg->solution_lengths_avg, agg->solution_lengths_count);
}

void print_solve_stats(const solve_stats_t *stats) {
    printf("Solve stats:\n");
    printf("  Phase 1 time: %f seconds\n", stats->phase1_solve_time);
    printf("  Phase 2 time: %f seconds\n", stats->total_phase2_time);
    printf("  Total time: %f seconds\n", stats->wall_time);
    printf("  Phase 1 depth: %d\n", stats->phase1_depth);
    printf("  Phase 2 depth: %d\n", stats->phase2_depth);
    printf("  Solution length: %d\n", stats->solution_length);
    printf("  Phase 1 move count: %d\n", stats->phase1_move_count);
    printf("  Phase 2 move count: %d\n", stats->phase2_move_count);
    printf("  Move count: %d\n", stats->total_moves);
    float mps = stats->wall_time > 0.0f ? (float)stats->total_moves / stats->wall_time : 0.0f;
    printf("  Moves per second: %.2f\n", mps);
}

solve_stats_t *get_solve_stats() {
    solve_stats_t *stats = (solve_stats_t *)malloc(sizeof(solve_stats_t));
    memset(stats, 0, sizeof(solve_stats_t));
    return stats;
}

void init_stats() {}
void dump_stats() {}
