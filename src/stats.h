#ifndef _STATS
#define _STATS

typedef struct {
    int used;

    int phase1_depth;
    int phase2_depth;
    int solution_length;

    int phase1_move_count;
    int phase2_move_count;
    int move_count;

    float phase1_solve_time;
    float phase2_solve_time;
    float solve_time;
} solve_stats_t;

solve_stats_t *get_current_stat();
void           finish_stats();
void           init_stats();
void           dump_stats();

#endif
