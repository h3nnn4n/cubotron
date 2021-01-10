#ifndef _PRINING
#define _PRINING

#include "coord_cube.h"

void build_pruning_tables();
void build_phase1_corner_table();
void build_phase1_edge_table();
int  get_phase1_pruning(coord_cube_t *cube);

void build_phase2_corner_table();

#endif /* end of include guard */
