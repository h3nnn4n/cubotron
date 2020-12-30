#ifndef _MOVE_TABLES
#define _MOVE_TABLES

#include "cubie.h"
#include "definitions.h"

void        build_move_table();
cube_cubie *build_basic_move(move base_move);
void        apply_basic_move_raw(cube_cubie *cube, corner cp[], edge ep[], int co[], int eo[]);
void        apply_move(cube_cubie *cube, move move_to_apply);

extern corner corner_permutation_U[];
extern int    corner_orientation_U[];
extern edge   edge_permutation_U[];
extern int    edge_orientation_U[];

extern corner corner_permutation_R[];
extern int    corner_orientation_R[];
extern edge   edge_permutation_R[];
extern int    edge_orientation_R[];

extern corner corner_permutation_F[];
extern int    corner_orientation_F[];
extern edge   edge_permutation_F[];
extern int    edge_orientation_F[];

extern corner corner_permutation_D[];
extern int    corner_orientation_D[];
extern edge   edge_permutation_D[];
extern int    edge_orientation_D[];

extern corner corner_permutation_L[];
extern int    corner_orientation_L[];
extern edge   edge_permutation_L[];
extern int    edge_orientation_L[];

extern corner corner_permutation_B[];
extern int    corner_orientation_B[];
extern edge   edge_permutation_B[];
extern int    edge_orientation_B[];

#endif /* end of include guard */
