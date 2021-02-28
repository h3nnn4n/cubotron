#ifndef _SOLVE
#define _SOLVE

#include "coord_cube.h"
#include "stats.h"

typedef struct solve_list_s solve_list_t;

typedef struct solve_list_s {
    solve_list_t *next;

    move_t *phase1_solution;
    move_t *phase2_solution;
    move_t *solution;

    solve_stats_t *stats;
} solve_list_t;

solve_list_t *solve_facelets_single(char facelets[N_FACELETS]);
solve_list_t *solve_facelets(char facelets[N_FACELETS], int max_depth, float timeout, int max_solutions);
solve_list_t *solve(coord_cube_t *original_cube, int max_depth, float timeout, int max_solutions);
solve_list_t *solve_single(coord_cube_t *original_cube);
move_t *      solve_phase1(coord_cube_t *cube, int max_depth, float timeout, int max_solutions, solve_list_t *solves);
move_t *      solve_phase2(coord_cube_t *cube, int starting_depth, float timeout);

solve_list_t *new_solve_list_node();
void          destroy_solve_list_node(solve_list_t *node);

#endif /* end of include guard */
