/*
 * Copyright <2026> <Renan S Silva, aka h3nnn4n>
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

#include "benchmark_stats.h"

#include <assert.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int compare_double(const void *a, const void *b) {
    double da = *(const double *)a;
    double db = *(const double *)b;
    return (da > db) - (da < db);
}

static double percentile(double *sorted_data, size_t n, double p) {
    if (n == 0)
        return 0.0;
    double index = p * (n - 1);
    size_t lower = (size_t)index;
    size_t upper = lower + 1;
    if (upper >= n)
        return sorted_data[n - 1];
    double weight = index - lower;
    return sorted_data[lower] * (1.0 - weight) + sorted_data[upper] * weight;
}

stats_t calculate_stats(const double *data, size_t n) {
    stats_t stats = {0};
    if (n == 0)
        return stats;

    stats.n = n;

    double *sorted = malloc(n * sizeof(double));
    memcpy(sorted, data, n * sizeof(double));
    qsort(sorted, n, sizeof(double), compare_double);

    double sum = 0.0;
    for (size_t i = 0; i < n; i++)
        sum += sorted[i];
    stats.mean = sum / n;

    stats.median = (n % 2 == 0) ? (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0 : sorted[n / 2];

    double var_sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        double diff = sorted[i] - stats.mean;
        var_sum += diff * diff;
    }
    stats.std_dev = sqrt(var_sum / n);

    stats.min = sorted[0];
    stats.max = sorted[n - 1];
    stats.p90 = percentile(sorted, n, 0.90);
    stats.p95 = percentile(sorted, n, 0.95);
    stats.p99 = percentile(sorted, n, 0.99);

    free(sorted);
    return stats;
}

comparison_t compare_samples(const double *data1, size_t n1, const double *data2, size_t n2) {
    comparison_t comp = {0};

    if (n1 == 0 || n2 == 0)
        return comp;

    // Calculate basic statistics for both samples
    stats_t stats1 = calculate_stats(data1, n1);
    stats_t stats2 = calculate_stats(data2, n2);

    // Calculate absolute and percentage difference in means
    comp.mean_diff     = stats1.mean - stats2.mean;
    comp.mean_diff_pct = (stats2.mean != 0.0) ? (comp.mean_diff / stats2.mean) * 100.0 : 0.0;

    // Welch's t-test: compares means without assuming equal variance
    double var1 = stats1.std_dev * stats1.std_dev;
    double var2 = stats2.std_dev * stats2.std_dev;

    // Standard error of the difference between means
    double se = sqrt(var1 / n1 + var2 / n2);

    if (se > 0.0) {
        // Welch's t-statistic: how many standard errors apart are the means?
        comp.t_statistic = comp.mean_diff / se;

        // Welch-Satterthwaite degrees of freedom (accounts for unequal variances)
        double df_num = (var1 / n1 + var2 / n2) * (var1 / n1 + var2 / n2);
        double df_den = (var1 * var1) / (n1 * n1 * (n1 - 1)) + (var2 * var2) / (n2 * n2 * (n2 - 1));
        double df     = (df_den > 0.0) ? df_num / df_den : 1.0;

        // Critical value for 95% confidence (α=0.05)
        // Use 1.96 for large samples (df >= 30), 2.042 for small samples
        double t_crit = 1.96;

        if (df < 30) {
            t_crit = 2.042;
        }

        // 95% confidence interval for the difference
        comp.ci_lower = comp.mean_diff - t_crit * se;
        comp.ci_upper = comp.mean_diff + t_crit * se;

        // Statistically significant if |t| > critical value
        comp.is_significant = (fabs(comp.t_statistic) > t_crit) ? 1 : 0;
        comp.p_value        = comp.is_significant ? 0.02 : 0.5; // Approximate p-value
    }

    // Cohen's d: standardized effect size (small: 0.2, medium: 0.5, large: 0.8)
    double pooled_std = sqrt((var1 + var2) / 2.0);
    comp.cohens_d     = (pooled_std > 0.0) ? comp.mean_diff / pooled_std : 0.0;

    return comp;
}

benchmark_result_t *make_benchmark_result(int sample_count) {
    benchmark_result_t *result = calloc(1, sizeof(benchmark_result_t));
    result->sample_count       = sample_count;
    result->solve_times_ms     = calloc(sample_count, sizeof(double));
    result->solution_lengths   = calloc(sample_count, sizeof(int));
    result->phase1_lengths     = calloc(sample_count, sizeof(int));
    result->phase2_lengths     = calloc(sample_count, sizeof(int));
    return result;
}

void destroy_benchmark_result(benchmark_result_t *result) {
    if (result) {
        free(result->solve_times_ms);
        free(result->solution_lengths);
        free(result->phase1_lengths);
        free(result->phase2_lengths);
        free(result);
    }
}

int save_benchmark_json(const benchmark_result_t *result, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f)
        return -1;

    fprintf(f, "{\n");
    fprintf(f, "  \"timestamp\": \"%s\",\n", result->timestamp);
    fprintf(f, "  \"type\": \"%s\",\n", result->type);
    fprintf(f, "  \"git_commit\": \"%s\",\n", result->git_commit);
    fprintf(f, "  \"warmup_duration_ms\": %d,\n", result->warmup_duration_ms);
    fprintf(f, "  \"benchmark_duration_ms\": %d,\n", result->benchmark_duration_ms);
    fprintf(f, "  \"warmup_count\": %d,\n", result->warmup_count);
    fprintf(f, "  \"sample_count\": %d,\n", result->sample_count);

    fprintf(f, "  \"solve_times_ms\": [");
    for (int i = 0; i < result->sample_count; i++) {
        if (i > 0)
            fprintf(f, ", ");
        fprintf(f, "%.3f", result->solve_times_ms[i]);
    }
    fprintf(f, "],\n");

    fprintf(f, "  \"solution_lengths\": [");
    for (int i = 0; i < result->sample_count; i++) {
        if (i > 0)
            fprintf(f, ", ");
        fprintf(f, "%d", result->solution_lengths[i]);
    }
    fprintf(f, "],\n");

    fprintf(f, "  \"stats\": {\n");
    fprintf(f, "    \"time\": {\n");
    fprintf(f, "      \"mean\": %.3f,\n", result->time_stats.mean);
    fprintf(f, "      \"median\": %.3f,\n", result->time_stats.median);
    fprintf(f, "      \"std\": %.3f,\n", result->time_stats.std_dev);
    fprintf(f, "      \"min\": %.3f,\n", result->time_stats.min);
    fprintf(f, "      \"max\": %.3f,\n", result->time_stats.max);
    fprintf(f, "      \"p90\": %.3f,\n", result->time_stats.p90);
    fprintf(f, "      \"p95\": %.3f,\n", result->time_stats.p95);
    fprintf(f, "      \"p99\": %.3f\n", result->time_stats.p99);
    fprintf(f, "    },\n");
    fprintf(f, "    \"length\": {\n");
    fprintf(f, "      \"mean\": %.2f,\n", result->length_stats.mean);
    fprintf(f, "      \"median\": %.1f,\n", result->length_stats.median);
    fprintf(f, "      \"std\": %.2f,\n", result->length_stats.std_dev);
    fprintf(f, "      \"min\": %.0f,\n", result->length_stats.min);
    fprintf(f, "      \"max\": %.0f\n", result->length_stats.max);
    fprintf(f, "    }\n");
    fprintf(f, "  }\n");
    fprintf(f, "}\n");

    fclose(f);
    return 0;
}

int save_benchmark_txt(const benchmark_result_t *result, const char *filename, const benchmark_result_t *previous) {
    FILE *f = fopen(filename, "w");
    if (!f)
        return -1;

    fprintf(f, "=== Cubotron Benchmark Results ===\n");
    fprintf(f, "Type: %s\n", result->type);
    fprintf(f, "Date: %s\n", result->timestamp);
    fprintf(f, "Git:  %s\n", result->git_commit);
    fprintf(f, "Warmup: %d solves in %d ms\n", result->warmup_count, result->warmup_duration_ms);
    fprintf(f, "Benchmark: %d solves in %d ms\n\n", result->sample_count, result->benchmark_duration_ms);

    fprintf(f, "--- Solve Time (ms) ---\n");
    fprintf(f, "  Mean:     %7.3f ms\n", result->time_stats.mean);
    fprintf(f, "  Std Dev:  %7.3f ms\n", result->time_stats.std_dev);
    fprintf(f, "  Min:      %7.3f ms\n", result->time_stats.min);
    fprintf(f, "  P50:      %7.3f ms\n", result->time_stats.median);
    fprintf(f, "  P90:      %7.3f ms\n", result->time_stats.p90);
    fprintf(f, "  P95:      %7.3f ms\n", result->time_stats.p95);
    fprintf(f, "  P99:      %7.3f ms\n", result->time_stats.p99);
    fprintf(f, "  Max:      %7.3f ms\n\n", result->time_stats.max);

    fprintf(f, "--- Solution Length (moves) ---\n");
    fprintf(f, "  Mean:     %7.2f moves\n", result->length_stats.mean);
    fprintf(f, "  Std Dev:  %7.2f moves\n", result->length_stats.std_dev);
    fprintf(f, "  Min:      %7.0f moves\n", result->length_stats.min);
    fprintf(f, "  Median:   %7.0f moves\n", result->length_stats.median);
    fprintf(f, "  Max:      %7.0f moves\n\n", result->length_stats.max);

    if (previous) {
        fprintf(f, "=== Comparison vs Previous Benchmark ===\n");
        fprintf(f, "Previous: %s (%d samples)\n\n", previous->timestamp, previous->sample_count);

        double *times1 = malloc(result->sample_count * sizeof(double));
        double *times2 = malloc(previous->sample_count * sizeof(double));
        for (int i = 0; i < result->sample_count; i++)
            times1[i] = result->solve_times_ms[i];
        for (int i = 0; i < previous->sample_count; i++)
            times2[i] = previous->solve_times_ms[i];

        comparison_t time_comp = compare_samples(times1, result->sample_count, times2, previous->sample_count);

        fprintf(f, "Solve Time:       %.3f ms → %.3f ms   (%+.1f%%)   %s\n", result->time_stats.mean,
                previous->time_stats.mean, time_comp.mean_diff_pct,
                time_comp.is_significant ? (time_comp.mean_diff < 0 ? "✓ FASTER" : "✗ SLOWER") : "~ NO CHANGE");
        fprintf(f, "  Welch's t-test: t=%.2f, p=%.3f (%s at α=0.05)\n", time_comp.t_statistic, time_comp.p_value,
                time_comp.is_significant ? "significant" : "not significant");
        fprintf(f, "  95%% CI:         [%.3f, %.3f] ms\n", time_comp.ci_lower, time_comp.ci_upper);
        fprintf(f, "  Effect size:    Cohen's d = %.2f\n\n", time_comp.cohens_d);

        double *lengths1 = malloc(result->sample_count * sizeof(double));
        double *lengths2 = malloc(previous->sample_count * sizeof(double));
        for (int i = 0; i < result->sample_count; i++)
            lengths1[i] = (double)result->solution_lengths[i];
        for (int i = 0; i < previous->sample_count; i++)
            lengths2[i] = (double)previous->solution_lengths[i];

        comparison_t length_comp = compare_samples(lengths1, result->sample_count, lengths2, previous->sample_count);

        fprintf(f, "Solution Length:  %.2f → %.2f moves       (%+.1f%%)   %s\n", result->length_stats.mean,
                previous->length_stats.mean, length_comp.mean_diff_pct,
                length_comp.is_significant ? (length_comp.mean_diff < 0 ? "✓ BETTER" : "✗ WORSE") : "~ NO CHANGE");
        fprintf(f, "  Welch's t-test: t=%.2f, p=%.3f (%s)\n", length_comp.t_statistic, length_comp.p_value,
                length_comp.is_significant ? "significant" : "not significant");
        fprintf(f, "  95%% CI:         [%.2f, %.2f] moves\n\n", length_comp.ci_lower, length_comp.ci_upper);

        int verdict = 0;
        if (time_comp.is_significant && time_comp.mean_diff < 0)
            verdict = 1;
        else if (time_comp.is_significant && time_comp.mean_diff > 0)
            verdict = -1;

        if (verdict > 0)
            fprintf(f, "Verdict: ✓ Performance IMPROVED (time significantly faster)\n");
        else if (verdict < 0)
            fprintf(f, "Verdict: ✗ Performance REGRESSED (time significantly slower)\n");
        else
            fprintf(f, "Verdict: ~ No significant change\n");

        free(times1);
        free(times2);
        free(lengths1);
        free(lengths2);
    }

    fclose(f);
    return 0;
}

benchmark_result_t *load_benchmark_json(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;

    benchmark_result_t *result = calloc(1, sizeof(benchmark_result_t));

    char line[4096];
    int  sample_count = 0;

    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "  \"timestamp\": \"%31[^\"]\"", result->timestamp);
        sscanf(line, "  \"type\": \"%15[^\"]\"", result->type);
        sscanf(line, "  \"git_commit\": \"%63[^\"]\"", result->git_commit);
        sscanf(line, "  \"warmup_duration_ms\": %d", &result->warmup_duration_ms);
        sscanf(line, "  \"benchmark_duration_ms\": %d", &result->benchmark_duration_ms);
        sscanf(line, "  \"warmup_count\": %d", &result->warmup_count);
        sscanf(line, "  \"sample_count\": %d", &sample_count);
    }

    result->sample_count     = sample_count;
    result->solve_times_ms   = calloc(sample_count, sizeof(double));
    result->solution_lengths = calloc(sample_count, sizeof(int));
    result->phase1_lengths   = calloc(sample_count, sizeof(int));
    result->phase2_lengths   = calloc(sample_count, sizeof(int));

    fseek(f, 0, SEEK_SET);
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, "\"solve_times_ms\":")) {
            char *p = strchr(line, '[');
            if (p) {
                p++;
                for (int i = 0; i < sample_count && *p; i++) {
                    result->solve_times_ms[i] = strtod(p, &p);
                    while (*p == ',' || *p == ' ')
                        p++;
                }
            }
        }
        if (strstr(line, "\"solution_lengths\":")) {
            char *p = strchr(line, '[');
            if (p) {
                p++;
                for (int i = 0; i < sample_count && *p; i++) {
                    result->solution_lengths[i] = strtol(p, &p, 10);
                    while (*p == ',' || *p == ' ')
                        p++;
                }
            }
        }
    }

    double *times   = malloc(sample_count * sizeof(double));
    double *lengths = malloc(sample_count * sizeof(double));
    for (int i = 0; i < sample_count; i++) {
        times[i]   = result->solve_times_ms[i];
        lengths[i] = (double)result->solution_lengths[i];
    }
    result->time_stats   = calculate_stats(times, sample_count);
    result->length_stats = calculate_stats(lengths, sample_count);
    free(times);
    free(lengths);

    fclose(f);
    return result;
}

char *find_latest_benchmark(const char *type_filter) {
    DIR *dir = opendir("benchmarks/results");
    if (!dir)
        return NULL;

    char *latest        = NULL;
    char  latest_ts[32] = {0};

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".json") == NULL)
            continue;

        if (type_filter && strstr(entry->d_name, type_filter) == NULL)
            continue;

        char ts[32];
        if (sscanf(entry->d_name, "%*[^_]_%31[^.]", ts) == 1) {
            if (strcmp(ts, latest_ts) > 0) {
                strncpy(latest_ts, ts, sizeof(latest_ts) - 1);
                free(latest);
                size_t path_size = strlen("benchmarks/results/") + strlen(entry->d_name) + 1;
                latest           = malloc(path_size);
                snprintf(latest, path_size, "benchmarks/results/%s", entry->d_name);
            }
        }
    }

    closedir(dir);
    return latest;
}
