#ifndef _MOVE_TABLES
#define _MOVE_TABLES

#include "coord_cube.h"
#include "cubie_cube.h"
#include "definitions.h"

void build_move_tables();
void coord_apply_move(coord_cube_t *cube, move_t move);

#endif /* end of include guard */
