#include <stdlib.h>

#include "cubie.h"
#include "definitions.h"
#include "move_tables.h"

cube_cubie **build_move_table() {
    cube_cubie **move_table = malloc(sizeof(cube_cubie *) * 18);

    for (int i = 0; i < 18; i++) {
        move_table[i] = init_cubie_cube();
    }

    return move_table;
}

cube_cubie *build_basic_move(move base_move) {
    cube_cubie *cube = init_cubie_cube();

    switch (base_move) {
        case MOVE_U1:
        case MOVE_U2:
        case MOVE_U3:
            apply_basic_move_raw(cube, corner_permutation_U, edge_permutation_U, corner_orientation_U,
                                 edge_orientation_U);
            break;

        case MOVE_R1:
        case MOVE_R2:
        case MOVE_R3:
            apply_basic_move_raw(cube, corner_permutation_R, edge_permutation_R, corner_orientation_R,
                                 edge_orientation_R);
            break;

        case MOVE_F1:
        case MOVE_F2:
        case MOVE_F3:
            apply_basic_move_raw(cube, corner_permutation_F, edge_permutation_F, corner_orientation_F,
                                 edge_orientation_F);
            break;

        case MOVE_D1:
        case MOVE_D2:
        case MOVE_D3:
            apply_basic_move_raw(cube, corner_permutation_D, edge_permutation_D, corner_orientation_D,
                                 edge_orientation_D);
            break;

        case MOVE_L1:
        case MOVE_L2:
        case MOVE_L3:
            apply_basic_move_raw(cube, corner_permutation_L, edge_permutation_L, corner_orientation_L,
                                 edge_orientation_L);
            break;

        case MOVE_B1:
        case MOVE_B2:
        case MOVE_B3:
            apply_basic_move_raw(cube, corner_permutation_B, edge_permutation_B, corner_orientation_B,
                                 edge_orientation_B);
            break;

        case MOVE_NULL: break;
    }

    return cube;
}

void apply_basic_move_raw(cube_cubie *cube, corner cp[], edge ep[], int co[], int eo[]) {
    for (int i = 0; i < 8; i++) {
        cube->corner_permutations[i] = cp[i];
        cube->corner_orientations[i] = co[i];
    }

    for (int i = 0; i < 12; i++) {
        cube->edge_permutations[i] = ep[i];
        cube->edge_orientations[i] = eo[i];
    }
}

// Up
corner corner_permutation_U[] = {CORNER_UBR, CORNER_URF, CORNER_UFL, CORNER_ULB,
                                 CORNER_DFR, CORNER_DLF, CORNER_DBL, CORNER_DRB};
int    corner_orientation_U[] = {0, 0, 0, 0, 0, 0, 0, 0};
edge   edge_permutation_U[]   = {EDGE_UB, EDGE_UR, EDGE_UF, EDGE_UL, EDGE_DR, EDGE_DF,
                             EDGE_DL, EDGE_DB, EDGE_FR, EDGE_FL, EDGE_BL, EDGE_BR};
int    edge_orientation_U[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Right;
corner corner_permutation_R[] = {CORNER_DFR, CORNER_UFL, CORNER_ULB, CORNER_URF,
                                 CORNER_DRB, CORNER_DLF, CORNER_DBL, CORNER_UBR};
int    corner_orientation_R[] = {2, 0, 0, 1, 1, 0, 0, 2};
edge   edge_permutation_R[]   = {EDGE_FR, EDGE_UF, EDGE_UL, EDGE_UB, EDGE_BR, EDGE_DF,
                             EDGE_DL, EDGE_DB, EDGE_DR, EDGE_FL, EDGE_BL, EDGE_UR};
int    edge_orientation_R[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Front;
corner corner_permutation_F[] = {CORNER_UFL, CORNER_DLF, CORNER_ULB, CORNER_UBR,
                                 CORNER_URF, CORNER_DFR, CORNER_DBL, CORNER_DRB};
int    corner_orientation_F[] = {1, 2, 0, 0, 2, 1, 0, 0};
edge   edge_permutation_F[]   = {EDGE_UR, EDGE_FL, EDGE_UL, EDGE_UB, EDGE_DR, EDGE_FR,
                             EDGE_DL, EDGE_DB, EDGE_UF, EDGE_DF, EDGE_BL, EDGE_BR};
int    edge_orientation_F[]   = {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0};

// Down;
corner corner_permutation_D[] = {CORNER_URF, CORNER_UFL, CORNER_ULB, CORNER_UBR,
                                 CORNER_DLF, CORNER_DBL, CORNER_DRB, CORNER_DFR};
int    corner_orientation_D[] = {0, 0, 0, 0, 0, 0, 0, 0};
edge   edge_permutation_D[]   = {EDGE_UR, EDGE_UF, EDGE_UL, EDGE_UB, EDGE_DF, EDGE_DL,
                             EDGE_DB, EDGE_DR, EDGE_FR, EDGE_FL, EDGE_BL, EDGE_BR};
int    edge_orientation_D[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Left;
corner corner_permutation_L[] = {CORNER_URF, CORNER_ULB, CORNER_DBL, CORNER_UBR,
                                 CORNER_DFR, CORNER_UFL, CORNER_DLF, CORNER_DRB};
int    corner_orientation_L[] = {0, 1, 2, 0, 0, 2, 1, 0};
edge   edge_permutation_L[]   = {EDGE_UR, EDGE_UF, EDGE_BL, EDGE_UB, EDGE_DR, EDGE_DF,
                             EDGE_FL, EDGE_DB, EDGE_FR, EDGE_UL, EDGE_DL, EDGE_BR};
int    edge_orientation_L[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Back;
corner corner_permutation_B[] = {CORNER_URF, CORNER_UFL, CORNER_UBR, CORNER_DRB,
                                 CORNER_DFR, CORNER_DLF, CORNER_ULB, CORNER_DBL};
int    corner_orientation_B[] = {0, 0, 1, 2, 0, 0, 2, 1};
edge   edge_permutation_B[]   = {EDGE_UR, EDGE_UF, EDGE_UL, EDGE_BR, EDGE_DR, EDGE_DF,
                             EDGE_DL, EDGE_BL, EDGE_FR, EDGE_FL, EDGE_UB, EDGE_DB};
int    edge_orientation_B[]   = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1};
