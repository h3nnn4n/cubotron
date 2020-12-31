#ifndef _UTILS
#define _UTILS

#include "cubie.h"

int   is_solved(cube_cubie *cube);
int   cubie_off_count(cube_cubie *cube);
char *move_to_str(move_t move);
int   is_valid(cube_cubie *cube);

#endif /* end of include guard */
