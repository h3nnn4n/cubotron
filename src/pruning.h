#ifndef _PRINING
#define _PRINING

#include "coord_cube.h"

void build_pruning_tables();
void build_phase1_corner_table();
void build_phase1_edge_table();
void build_phase2_UD6_edge_table();
void build_phase2_corner_table();
int  get_phase1_pruning(coord_cube_t *cube);
int  get_phase2_pruning(coord_cube_t *cube);

#endif /* end of include guard */
