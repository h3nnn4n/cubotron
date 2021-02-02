#ifndef _SOLVE
#define _SOLVE

#include "coord_cube.h"

move_t *solve(coord_cube_t *cube);
move_t *solve_phase1(coord_cube_t *cube);
move_t *solve_phase2(coord_cube_t *cube);

#endif /* end of include guard */
