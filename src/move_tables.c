#include <assert.h>
#include <stdlib.h>

#include "cubie.h"
#include "definitions.h"
#include "move_tables.h"

static cube_cubie **move_table = NULL;

void apply_move(cube_cubie *cube, move_t move_to_apply) {
    if (move_to_apply == MOVE_NULL)
        return;

    assert(move_table != NULL);
    multiply_cube_cubie(cube, move_table[move_to_apply]);
}

void build_move_table() {
    if (move_table != NULL)
        return;

    move_table = malloc(sizeof(cube_cubie *) * 18);

    cube_cubie *moves[6];
    moves[0] = build_basic_move(MOVE_U1);
    moves[1] = build_basic_move(MOVE_R1);
    moves[2] = build_basic_move(MOVE_F1);
    moves[3] = build_basic_move(MOVE_D1);
    moves[4] = build_basic_move(MOVE_L1);
    moves[5] = build_basic_move(MOVE_B1);

    for (int i = 0; i < 18; i++) {
        move_table[i] = init_cubie_cube();
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            for (int jj = 0; jj <= j; jj++) {
                multiply_cube_cubie(move_table[3 * i + j], moves[i]);
            }
        }
    }
}

cube_cubie *build_basic_move(move_t base_move) {
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

void apply_basic_move_raw(cube_cubie *cube, corner_t cp[], edge_t ep[], int co[], int eo[]) {
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
corner_t corner_permutation_U[] = {UBR, URF, UFL, ULB, DFR, DLF, DBL, DRB};
int      corner_orientation_U[] = {0, 0, 0, 0, 0, 0, 0, 0};
edge_t   edge_permutation_U[]   = {UB, UR, UF, UL, DR, DF, DL, DB, FR, FL, BL, BR};
int      edge_orientation_U[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Right;
corner_t corner_permutation_R[] = {DFR, UFL, ULB, URF, DRB, DLF, DBL, UBR};
int      corner_orientation_R[] = {2, 0, 0, 1, 1, 0, 0, 2};
edge_t   edge_permutation_R[]   = {FR, UF, UL, UB, BR, DF, DL, DB, DR, FL, BL, UR};
int      edge_orientation_R[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Front;
corner_t corner_permutation_F[] = {UFL, DLF, ULB, UBR, URF, DFR, DBL, DRB};
int      corner_orientation_F[] = {1, 2, 0, 0, 2, 1, 0, 0};
edge_t   edge_permutation_F[]   = {UR, FL, UL, UB, DR, FR, DL, DB, UF, DF, BL, BR};
int      edge_orientation_F[]   = {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0};

// Down;
corner_t corner_permutation_D[] = {URF, UFL, ULB, UBR, DLF, DBL, DRB, DFR};
int      corner_orientation_D[] = {0, 0, 0, 0, 0, 0, 0, 0};
edge_t   edge_permutation_D[]   = {UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR};
int      edge_orientation_D[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Left;
corner_t corner_permutation_L[] = {URF, ULB, DBL, UBR, DFR, UFL, DLF, DRB};
int      corner_orientation_L[] = {0, 1, 2, 0, 0, 2, 1, 0};
edge_t   edge_permutation_L[]   = {UR, UF, BL, UB, DR, DF, FL, DB, FR, UL, DL, BR};
int      edge_orientation_L[]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Back;
corner_t corner_permutation_B[] = {URF, UFL, UBR, DRB, DFR, DLF, ULB, DBL};
int      corner_orientation_B[] = {0, 0, 1, 2, 0, 0, 2, 1};
edge_t   edge_permutation_B[]   = {UR, UF, UL, BR, DR, DF, DL, BL, FR, FL, UB, DB};
int      edge_orientation_B[]   = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1};
