#ifndef _COORD_MOVE_TABLES
#define _COORD_MOVE_TABLES

#include "coord_cube.h"
#include "cubie_cube.h"
#include "definitions.h"

void coord_build_move_tables();
void coord_apply_move(coord_cube_t *cube, move_t move);

int *get_move_table_edge_orientations();
int *get_move_table_corner_orientations();
int *get_move_table_UD_slice();
int *get_move_table_UD_sorted_slice();
int *get_move_table_corner_permutations();
int *get_move_table_parity();

#endif /* end of include guard */
