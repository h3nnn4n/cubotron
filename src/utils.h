#ifndef _UTILS
#define _UTILS

#include "cubie_cube.h"

int   is_solved(cube_cubie_t *cube);
int   cubie_off_count(cube_cubie_t *cube);
char *move_to_str(move_t move);
int   is_valid(cube_cubie_t *cube);

#endif /* end of include guard */
