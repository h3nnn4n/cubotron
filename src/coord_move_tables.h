#ifndef _MOVE_TABLES
#define _MOVE_TABLES

#include "coord_cube.h"
#include "cubie_cube.h"
#include "definitions.h"

void build_move_tables();
void coord_apply_move(coord_cube_t *cube, move_t move);

int *get_move_table_edge_orientations();
int *get_move_table_corner_orientations();
int *get_move_table_UD_slice();
int *get_move_table_corner_permutations();

#endif /* end of include guard */
