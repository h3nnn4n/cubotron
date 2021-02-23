#ifndef _SOLVE
#define _SOLVE

#include "coord_cube.h"

move_t *solve_facelets_single(char facelets[N_FACELETS]);
move_t *solve_facelets(char facelets[N_FACELETS], int max_depth, float timeout, int max_solutions);
move_t *solve(coord_cube_t *original_cube, int max_depth, float timeout, int max_solutions);
move_t *solve_single(coord_cube_t *original_cube);
move_t *solve_phase1(coord_cube_t *cube, int max_depth, float timeout, int max_solutions);
move_t *solve_phase2(coord_cube_t *cube, int starting_depth, float timeout);

#endif /* end of include guard */
