#include "cubie.h"

#include "definitions.h"
#include "move_tables.h"

cube_cubie* init_cubie_cube() {
  cube_cubie* cube = (cube_cubie*)malloc(sizeof(cube_cubie));

  for (int i = 0; i < 8; i++) {
    cube->corner_permutations[i] = i;
    cube->corner_orientations[i] = 0;
  }

  for (int i = 0; i < 12; i++) {
    cube->edge_permutations[i] = i;
    cube->edge_orientations[i] = 0;
  }

  return cube;
}

cube_cubie* build_cubie_cube(move base_move) {
  cube_cubie* cube = init_cubie_cube();

  switch (base_move) {
    case MOVE_U1:
    case MOVE_U2:
    case MOVE_U3:
      apply_basic_move(cube, corner_permutation_U, edge_permutation_U,
                       corner_orientation_U, edge_orientation_U);
      break;

    case MOVE_R1:
    case MOVE_R2:
    case MOVE_R3:
      apply_basic_move(cube, corner_permutation_R, edge_permutation_R,
                       corner_orientation_R, edge_orientation_R);
      break;

    case MOVE_F1:
    case MOVE_F2:
    case MOVE_F3:
      apply_basic_move(cube, corner_permutation_F, edge_permutation_F,
                       corner_orientation_F, edge_orientation_F);
      break;

    case MOVE_D1:
    case MOVE_D2:
    case MOVE_D3:
      apply_basic_move(cube, corner_permutation_D, edge_permutation_D,
                       corner_orientation_D, edge_orientation_D);
      break;

    case MOVE_L1:
    case MOVE_L2:
    case MOVE_L3:
      apply_basic_move(cube, corner_permutation_L, edge_permutation_L,
                       corner_orientation_L, edge_orientation_L);
      break;

    case MOVE_B1:
    case MOVE_B2:
    case MOVE_B3:
      apply_basic_move(cube, corner_permutation_B, edge_permutation_B,
                       corner_orientation_B, edge_orientation_B);
      break;

    case MOVE_NULL:
      break;
  }

  return cube;
}

void apply_basic_move(cube_cubie* cube, corner cp[], edge ep[], int co[],
                      int eo[]) {
  for (int i = 0; i < 8; i++) {
    cube->corner_permutations[i] = cp[i];
    cube->corner_orientations[i] = co[i];
  }

  for (int i = 0; i < 12; i++) {
    cube->edge_permutations[i] = ep[i];
    cube->edge_orientations[i] = eo[i];
  }
}
