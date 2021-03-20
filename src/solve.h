#ifndef _SOLVE
#define _SOLVE

#include "config.h"
#include "coord_cube.h"
#include "stats.h"

#define MAX_MOVES 30

typedef struct solve_list_s solve_list_t;

typedef struct solve_list_s {
    solve_list_t *next;

    move_t *phase1_solution;
    move_t *phase2_solution;
    move_t *solution;

    solve_stats_t *stats;
} solve_list_t;

typedef struct solve_context_s solve_context_t;

typedef struct solve_context_s {
    coord_cube_t *cube;
    move_t        move_stack[MAX_MOVES];
    coord_cube_t *cube_stack[MAX_MOVES];
    int           pruning_stack[MAX_MOVES];
    int           move_count;

    solve_context_t *phase2_context;
} solve_context_t;

solve_list_t *solve_facelets_single(char facelets[N_FACELETS]);
solve_list_t *solve_facelets(char facelets[N_FACELETS], config_t *config);
solve_list_t *solve(coord_cube_t *original_cube, config_t *config);
solve_list_t *solve_single(coord_cube_t *original_cube);
move_t *      solve_phase1(solve_context_t *solve_context, config_t *config, solve_list_t *solves);
move_t *      solve_phase2(solve_context_t *solve_context, config_t *config, int current_depth);

solve_list_t *new_solve_list_node();
void          destroy_solve_list_node(solve_list_t *node);

solve_context_t *make_solve_context(coord_cube_t *cube);
void             clear_solve_context(solve_context_t *solve_context);
void             destroy_solve_context(solve_context_t *context);

#endif /* end of include guard */
