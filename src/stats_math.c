#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include "stats_math.h"

static int float_compare(const void *a, const void *b) {
    float fa = *(const float *)a;
    float fb = *(const float *)b;
    if (fa < fb)
        return -1;
    if (fa > fb)
        return 1;
    return 0;
}

void sort_float(float *values, int n) { qsort(values, (size_t)n, sizeof(float), float_compare); }

float min_f(const float *values, int n) {
    float v = values[0];
    for (int i = 1; i < n; i++)
        if (values[i] < v)
            v = values[i];
    return v;
}

float max_f(const float *values, int n) {
    float v = values[0];
    for (int i = 1; i < n; i++)
        if (values[i] > v)
            v = values[i];
    return v;
}

float avg_f(const float *values, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += values[i];
    return (float)(sum / n);
}

float std_f(const float *values, int n) {
    float  mean   = avg_f(values, n);
    double sum_sq = 0.0;
    for (int i = 0; i < n; i++) {
        float diff = values[i] - mean;
        sum_sq += (double)diff * diff;
    }
    return (float)sqrt(sum_sq / n);
}

float percentile_f(const float *sorted, int n, float p) {
    if (n <= 0)
        return 0.0f;
    if (p >= 100.0f)
        return sorted[n - 1];
    if (p <= 0.0f)
        return sorted[0];

    double idx   = (double)p / 100.0 * (double)(n - 1);
    int    lower = (int)idx;
    int    upper = lower + 1 < n ? lower + 1 : n - 1;
    float  frac  = (float)(idx - lower);

    return sorted[lower] + frac * (sorted[upper] - sorted[lower]);
}

void compute_float_aggregate(float *values, int n, float *out_min, float *out_max, float *out_avg, float *out_std,
                             float *out_p90, float *out_p95, float *out_p99) {
    if (n <= 0) {
        if (out_min) {
            *out_min = 0;
        }
        if (out_max) {
            *out_max = 0;
        }
        if (out_avg) {
            *out_avg = 0;
        }
        if (out_std) {
            *out_std = 0;
        }
        if (out_p90) {
            *out_p90 = 0;
        }
        if (out_p95) {
            *out_p95 = 0;
        }
        if (out_p99) {
            *out_p99 = 0;
        }
        return;
    }

    float sorted[n];
    for (int i = 0; i < n; i++)
        sorted[i] = values[i];
    sort_float(sorted, n);

    if (out_min)
        *out_min = sorted[0];
    if (out_max)
        *out_max = sorted[n - 1];
    if (out_avg)
        *out_avg = avg_f(values, n);
    if (out_std)
        *out_std = std_f(values, n);
    if (out_p90)
        *out_p90 = percentile_f(sorted, n, 90.0f);
    if (out_p95)
        *out_p95 = percentile_f(sorted, n, 95.0f);
    if (out_p99)
        *out_p99 = percentile_f(sorted, n, 99.0f);
}

void compute_int_aggregate(const int *values, int n, int *out_min, int *out_max, float *out_avg, float *out_std,
                           float *out_p90, float *out_p95, float *out_p99) {
    if (n <= 0) {
        if (out_min) {
            *out_min = 0;
        }
        if (out_max) {
            *out_max = 0;
        }
        if (out_avg) {
            *out_avg = 0;
        }
        if (out_std) {
            *out_std = 0;
        }
        if (out_p90) {
            *out_p90 = 0;
        }
        if (out_p95) {
            *out_p95 = 0;
        }
        if (out_p99) {
            *out_p99 = 0;
        }
        return;
    }

    float f[n];
    for (int i = 0; i < n; i++)
        f[i] = (float)values[i];
    float f_min = 0, f_max = 0;
    compute_float_aggregate(f, n, &f_min, &f_max, out_avg, out_std, out_p90, out_p95, out_p99);
    if (out_min)
        *out_min = (int)f_min;
    if (out_max)
        *out_max = (int)f_max;
}
