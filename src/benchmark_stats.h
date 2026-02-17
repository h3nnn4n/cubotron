#ifndef _BENCHMARK_STATS
#define _BENCHMARK_STATS

#include <stddef.h>
#include <stdint.h>

typedef struct {
    double mean;
    double median;
    double std_dev;
    double min;
    double max;
    double p90;
    double p95;
    double p99;
    size_t n;
} stats_t;

typedef struct {
    char timestamp[32];
    char type[16];
    char git_commit[64];
    int  warmup_count;
    int  sample_count;
    int  warmup_duration_ms;
    int  benchmark_duration_ms;

    double *solve_times_ms;
    int    *solution_lengths;
    int    *phase1_lengths;
    int    *phase2_lengths;

    stats_t time_stats;
    stats_t length_stats;
} benchmark_result_t;

typedef struct {
    double mean_diff;
    double mean_diff_pct;
    double t_statistic;
    double p_value;
    double ci_lower;
    double ci_upper;
    double cohens_d;
    int    is_significant;
} comparison_t;

stats_t      calculate_stats(const double *data, size_t n);
comparison_t compare_samples(const double *data1, size_t n1, const double *data2, size_t n2);

benchmark_result_t *make_benchmark_result(int sample_count);
void                destroy_benchmark_result(benchmark_result_t *result);

int save_benchmark_json(const benchmark_result_t *result, const char *filename);
int save_benchmark_txt(const benchmark_result_t *result, const char *filename, const benchmark_result_t *previous);
benchmark_result_t *load_benchmark_json(const char *filename);
char               *find_latest_benchmark(const char *type_filter);

#endif
