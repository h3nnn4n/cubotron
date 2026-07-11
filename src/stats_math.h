#ifndef _STATS_MATH_H
#define _STATS_MATH_H

void  sort_float(float *values, int n);
float min_f(const float *values, int n);
float max_f(const float *values, int n);
float avg_f(const float *values, int n);
float std_f(const float *values, int n);
float percentile_f(const float *sorted, int n, float p);

void compute_float_aggregate(float *values, int n, float *out_min, float *out_max, float *out_avg, float *out_std,
                             float *out_p90, float *out_p95, float *out_p99);

void compute_int_aggregate(const int *values, int n, int *out_min, int *out_max, float *out_avg, float *out_std,
                           float *out_p90, float *out_p95, float *out_p99);

#endif
