#ifndef _UTILS
#define _UTILS

#include "coord_cube.h"
#include "cubie_cube.h"

int   is_solved(cube_cubie_t *cube);
int   cubie_off_count(cube_cubie_t *cube);
char *move_to_str_enum(move_t move);
char *move_to_str(move_t move);
int   is_valid(cube_cubie_t *cube);

cube_cubie_t *random_cubie_cube();
coord_cube_t *random_coord_cube();

move_t get_reverse_move(move_t move);

#endif /* end of include guard */
