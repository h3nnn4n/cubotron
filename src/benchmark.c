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

#include <stddef.h> // Has to come before entropy, because they forgot to add it

#include <assert.h>
#include <entropy.h>
#include <pcg_variants.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "benchmark.h"
#include "benchmark_stats.h"
#include "config.h"
#include "cubie_cube.h"
#include "facelets.h"
#include "move_tables.h"
#include "pruning.h"
#include "sample_facelets.h"
#include "solve.h"
#include "utils.h"

#define n_scramble_moves 50

void apply_random_scramble(coord_cube_t *cube, int n_moves) {
    assert(cube != NULL);
    assert(n_moves <= n_scramble_moves);

    move_t moves[n_scramble_moves] = {0};
    for (int i = 0; i < n_moves; i++) {
        moves[i] = pcg32_boundedrand(N_MOVES);

        do {
            moves[i] = pcg32_boundedrand(N_MOVES);
        } while (i > 0 && (moves[i] == moves[i - 1] || is_duplicated_or_undoes_move(moves[i], moves[i - 1])));
    }

    for (int i = 0; i < n_moves; i++) {
        coord_apply_move(cube, moves[i]);
        /*printf(" %s", move_to_str(moves[i]));*/
    }
    /*printf(" : %4d %4d %3d %4d %5d %4d\n", cube->edge_orientations, cube->corner_orientations, cube->E_slice,*/
    /*cube->E_sorted_slice, cube->UD6_edge_permutations, cube->corner_permutations);*/

    assert(!is_phase1_solved(cube));
    assert(!is_phase2_solved(cube));
}

static void run_benchmark_internal(const char *base_type, int warmup_duration_ms, int benchmark_duration_ms) {
    const config_t *config = get_config();

    char type[16];
    if (config->max_depth != 25) {
        snprintf(type, sizeof(type), "%s_d%d", base_type, config->max_depth);
    } else {
        strncpy(type, base_type, sizeof(type) - 1);
        type[sizeof(type) - 1] = '\0';
    }

    printf("=== Cubotron Benchmark ===\n");
    printf("Type: %s\n", type);
    printf("Warmup duration: %d ms\n", warmup_duration_ms);
    printf("Benchmark duration: %d ms\n\n", benchmark_duration_ms);

    uint64_t seeds[2];
    entropy_getbytes((void *)seeds, sizeof(seeds));
    pcg32_srandom(seeds[0], seeds[1]);

    coord_cube_t *cube = get_coord_cube();

    printf("Warmup phase...\n");
    int      warmup_count  = 0;
    uint64_t warmup_start  = get_microseconds();
    uint64_t warmup_target = (uint64_t)warmup_duration_ms * 1000;

    while (get_microseconds() - warmup_start < warmup_target) {
        int n = pcg32_boundedrand(n_scramble_moves / 2) + n_scramble_moves / 2;
        apply_random_scramble(cube, n);
        solve_list_t *solution = solve_single(cube);

        if (solution) {
            destroy_solve_list(solution);
        }

        warmup_count++;
    }

    printf("  Completed %d warmup solves\n\n", warmup_count);

    printf("Benchmark phase...\n");
    int     max_samples  = 1000000;
    double *temp_times   = malloc(max_samples * sizeof(double));
    int    *temp_lengths = malloc(max_samples * sizeof(int));
    int     sample_count = 0;

    uint64_t bench_start  = get_microseconds();
    uint64_t bench_target = (uint64_t)benchmark_duration_ms * 1000;

    while (get_microseconds() - bench_start < bench_target && sample_count < max_samples) {
        int n = pcg32_boundedrand(n_scramble_moves / 2) + n_scramble_moves / 2;
        apply_random_scramble(cube, n);

        uint64_t      start_time = get_microseconds();
        solve_list_t *solution   = solve_single(cube);
        uint64_t      end_time   = get_microseconds();

        temp_times[sample_count] = (end_time - start_time) / 1000.0;

        if (solution && solution->solution) {
            int len = 0;

            for (int j = 0; solution->solution[j] != MOVE_NULL; j++) {
                len++;
            }

            temp_lengths[sample_count] = len;
            destroy_solve_list(solution);
        } else {
            temp_lengths[sample_count] = -1;
        }

        sample_count++;
    }

    uint64_t actual_bench_duration = get_microseconds() - bench_start;

    printf("  Completed %d solves\n\n", sample_count);

    free(cube);

    benchmark_result_t *result = make_benchmark_result(sample_count);
    snprintf(result->type, sizeof(result->type), "%s", type);
    result->warmup_count          = warmup_count;
    result->warmup_duration_ms    = (int)(get_microseconds() - warmup_start) / 1000;
    result->benchmark_duration_ms = (int)actual_bench_duration / 1000;

    for (int i = 0; i < sample_count; i++) {
        result->solve_times_ms[i]   = temp_times[i];
        result->solution_lengths[i] = temp_lengths[i];
    }

    free(temp_times);
    free(temp_lengths);

    time_t           now = time(NULL);
    struct tm        tm_buf;
    const struct tm *tm = localtime_r(&now, &tm_buf);
    strftime(result->timestamp, sizeof(result->timestamp), "%Y%m%d_%H%M%S", tm);

    FILE *git_pipe = popen("git rev-parse --short HEAD 2>/dev/null", "r");

    if (git_pipe) {
        if (fgets(result->git_commit, sizeof(result->git_commit), git_pipe)) {
            result->git_commit[strcspn(result->git_commit, "\n")] = 0;
        }

        pclose(git_pipe);
    }

    if (strlen(result->git_commit) == 0) {
        strncpy(result->git_commit, "unknown", sizeof(result->git_commit) - 1);
    }

    double *lengths = malloc(sample_count * sizeof(double));

    for (int i = 0; i < sample_count; i++) {
        lengths[i] = (double)result->solution_lengths[i];
    }

    result->time_stats   = calculate_stats(result->solve_times_ms, sample_count);
    result->length_stats = calculate_stats(lengths, sample_count);
    free(lengths);

    char *prev_file = NULL;

    prev_file = find_latest_benchmark(type);

    benchmark_result_t *previous = NULL;

    if (prev_file) {
        previous = load_benchmark_json(prev_file);
        free(prev_file);
    }

    char json_filename[256];
    char txt_filename[256];
    snprintf(json_filename, sizeof(json_filename), "benchmarks/results/%s_%s.json", type, result->timestamp);
    snprintf(txt_filename, sizeof(txt_filename), "benchmarks/results/%s_%s.txt", type, result->timestamp);

    save_benchmark_json(result, json_filename);
    save_benchmark_txt(result, txt_filename, previous);

    printf("=== Cubotron Benchmark Results ===\n");
    printf("Type: %s\n", result->type);
    printf("Date: %s\n", result->timestamp);
    printf("Git:  %s\n", result->git_commit);
    printf("Warmup: %d solves in %d ms\n", result->warmup_count, result->warmup_duration_ms);
    printf("Benchmark: %d solves in %d ms\n\n", result->sample_count, result->benchmark_duration_ms);

    printf("--- Solve Time (ms) ---\n");
    printf("  Mean:     %7.3f ms\n", result->time_stats.mean);
    printf("  Std Dev:  %7.3f ms\n", result->time_stats.std_dev);
    printf("  Min:      %7.3f ms\n", result->time_stats.min);
    printf("  P50:      %7.3f ms\n", result->time_stats.median);
    printf("  P90:      %7.3f ms\n", result->time_stats.p90);
    printf("  P95:      %7.3f ms\n", result->time_stats.p95);
    printf("  P99:      %7.3f ms\n", result->time_stats.p99);
    printf("  Max:      %7.3f ms\n\n", result->time_stats.max);

    printf("--- Solution Length (moves) ---\n");
    printf("  Mean:     %7.2f moves\n", result->length_stats.mean);
    printf("  Std Dev:  %7.2f moves\n", result->length_stats.std_dev);
    printf("  Min:      %7.0f moves\n", result->length_stats.min);
    printf("  Median:   %7.0f moves\n", result->length_stats.median);
    printf("  Max:      %7.0f moves\n\n", result->length_stats.max);

    if (previous) {
        printf("=== Comparison vs Previous Benchmark ===\n");
        printf("Previous: %s (%d samples)\n\n", previous->timestamp, previous->sample_count);

        comparison_t time_comp = compare_samples(result->solve_times_ms, result->sample_count, previous->solve_times_ms,
                                                 previous->sample_count);

        printf("Solve Time:       %.3f ms → %.3f ms   (%+.1f%%)   %s\n", previous->time_stats.mean,
               result->time_stats.mean, time_comp.mean_diff_pct,
               time_comp.is_significant ? (time_comp.mean_diff < 0 ? "✓ FASTER" : "✗ SLOWER") : "~ NO CHANGE");
        printf("  Welch's t-test: t=%.2f, p=%.3f (%s at α=0.05)\n", time_comp.t_statistic, time_comp.p_value,
               time_comp.is_significant ? "significant" : "not significant");
        printf("  95%% CI:         [%.3f, %.3f] ms\n", time_comp.ci_lower, time_comp.ci_upper);
        printf("  Effect size:    Cohen's d = %.2f\n\n", time_comp.cohens_d);

        double *lengths1 = malloc(result->sample_count * sizeof(double));
        double *lengths2 = malloc(previous->sample_count * sizeof(double));

        for (int i = 0; i < result->sample_count; i++) {
            lengths1[i] = (double)result->solution_lengths[i];
        }

        for (int i = 0; i < previous->sample_count; i++) {
            lengths2[i] = (double)previous->solution_lengths[i];
        }

        comparison_t length_comp = compare_samples(lengths1, result->sample_count, lengths2, previous->sample_count);

        printf("Solution Length:  %.2f → %.2f moves       (%+.1f%%)   %s\n", previous->length_stats.mean,
               result->length_stats.mean, length_comp.mean_diff_pct,
               length_comp.is_significant ? (length_comp.mean_diff < 0 ? "✓ BETTER" : "✗ WORSE") : "~ NO CHANGE");
        printf("  Welch's t-test: t=%.2f, p=%.3f (%s)\n", length_comp.t_statistic, length_comp.p_value,
               length_comp.is_significant ? "significant" : "not significant");
        printf("  95%% CI:         [%.2f, %.2f] moves\n\n", length_comp.ci_lower, length_comp.ci_upper);

        int verdict = 0;

        if (time_comp.is_significant && time_comp.mean_diff < 0) {
            verdict = 1;
        } else if (time_comp.is_significant && time_comp.mean_diff > 0) {
            verdict = -1;
        }

        if (verdict > 0) {
            printf("Verdict: ✓ Performance IMPROVED (time significantly faster)\n");
        } else if (verdict < 0) {
            printf("Verdict: ✗ Performance REGRESSED (time significantly slower)\n");
        } else {
            printf("Verdict: ~ No significant change\n");
        }

        free(lengths1);
        free(lengths2);
        destroy_benchmark_result(previous);
    }

    printf("\nResults saved to:\n");
    printf("  %s\n", json_filename);
    printf("  %s\n", txt_filename);

    destroy_benchmark_result(result);
}

void run_benchmark_fast() { run_benchmark_internal("fast", 500, 5000); }

void run_benchmark_slow() { run_benchmark_internal("slow", 1000, 30000); }
