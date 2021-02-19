#ifndef _SOLVE_MULTIPLE
#define _SOLVE_MULTIPLE

#include "coord_cube.h"

move_t *solve_continuous(coord_cube_t *cube);
move_t *solve_phase1_continuous(coord_cube_t *cube);
move_t *solve_phase2_continuous(coord_cube_t *cube, int starting_depth, float timeout);

#endif /* end of include guard */
