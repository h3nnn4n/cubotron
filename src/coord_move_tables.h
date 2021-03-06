#ifndef _COORD_MOVE_TABLES
#define _COORD_MOVE_TABLES

#include "coord_cube.h"
#include "cubie_cube.h"
#include "definitions.h"

void coord_build_move_tables();
void coord_apply_move(coord_cube_t *cube, move_t move);

int *get_move_table_edge_orientations();
int *get_move_table_corner_orientations();
int *get_move_table_E_slice();
int *get_move_table_E_sorted_slice();
int *get_move_table_UD6_edge_permutations();
int *get_move_table_UD7_edge_permutations();
int *get_move_table_corner_permutations();
int *get_move_table_parity();

void build_UD6_edge_permutations_move_table();
void build_UD7_edge_permutations_move_table();

#endif /* end of include guard */
