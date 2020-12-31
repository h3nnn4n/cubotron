#ifndef _CUBIE_MOVE_TABLE
#define _CUBIE_MOVE_TABLE

#include "cubie_cube.h"
#include "definitions.h"

void          cubie_build_move_table();
cube_cubie_t *cubie_build_basic_move(move_t base_move);
void          cubie_apply_basic_move_raw(cube_cubie_t *cube, corner_t cp[], edge_t ep[], int co[], int eo[]);
void          cubie_apply_move(cube_cubie_t *cube, move_t move_to_apply);

extern corner_t corner_permutation_U[];
extern int      corner_orientation_U[];
extern edge_t   edge_permutation_U[];
extern int      edge_orientation_U[];

extern corner_t corner_permutation_R[];
extern int      corner_orientation_R[];
extern edge_t   edge_permutation_R[];
extern int      edge_orientation_R[];

extern corner_t corner_permutation_F[];
extern int      corner_orientation_F[];
extern edge_t   edge_permutation_F[];
extern int      edge_orientation_F[];

extern corner_t corner_permutation_D[];
extern int      corner_orientation_D[];
extern edge_t   edge_permutation_D[];
extern int      edge_orientation_D[];

extern corner_t corner_permutation_L[];
extern int      corner_orientation_L[];
extern edge_t   edge_permutation_L[];
extern int      edge_orientation_L[];

extern corner_t corner_permutation_B[];
extern int      corner_orientation_B[];
extern edge_t   edge_permutation_B[];
extern int      edge_orientation_B[];

#endif /* end of include guard */
