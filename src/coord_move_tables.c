#include <assert.h>
#include <stdio.h>

#include "coord_cube.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "definitions.h"

static int *move_table_edge_orientations   = NULL;
static int *move_table_corner_orientations = NULL;
static int *move_table_UD_slice            = NULL;
static int *move_table_corner_permutations = NULL;

int *get_move_table_edge_orientations() { return move_table_edge_orientations; }
int *get_move_table_corner_orientations() { return move_table_corner_orientations; }
int *get_move_table_UD_slice() { return move_table_UD_slice; }
int *get_move_table_corner_permutations() { return move_table_corner_permutations; }

void coord_apply_move(coord_cube_t *cube, move_t move) {
    assert(cube != NULL);
    assert(move >= 0);
    assert(move < N_MOVES);
    assert(move_table_edge_orientations != NULL);
    assert(move_table_corner_orientations != NULL);
    assert(cube->edge_orientations * N_MOVES + move < N_EDGE_ORIENTATIONS * N_MOVES);
    assert(cube->corner_orientations * N_MOVES + move < N_CORNER_ORIENTATIONS * N_MOVES);
    assert(cube->corner_permutations * N_MOVES + move < N_CORNER_PERMUTATIONS * N_MOVES);

    // Phase 1
    cube->edge_orientations   = move_table_edge_orientations[cube->edge_orientations * N_MOVES + move];
    cube->corner_orientations = move_table_corner_orientations[cube->corner_orientations * N_MOVES + move];
    cube->UD_slice            = move_table_UD_slice[cube->UD_slice * N_MOVES + move];

    // Phase 2
    cube->corner_permutations = move_table_corner_permutations[cube->corner_permutations * N_MOVES + move];

    // Post conditions
    assert(cube->edge_orientations < N_EDGE_ORIENTATIONS);
    assert(cube->corner_orientations < N_CORNER_ORIENTATIONS);
    assert(cube->UD_slice < N_SLICES);
    assert(cube->corner_permutations < N_CORNER_PERMUTATIONS);
}

void build_move_tables() {
    cubie_build_move_table();

    cube_cubie_t *cube = NULL;

    // Edge orientation move tables
    if (move_table_edge_orientations == NULL) {
        move_table_edge_orientations = (int *)malloc(sizeof(int) * N_EDGE_ORIENTATIONS * N_MOVES);

        cube = init_cubie_cube();

        for (int edge_orientations = 0; edge_orientations < N_EDGE_ORIENTATIONS; edge_orientations++) {
            for (int move = 0; move < N_MOVES; move++) {
                set_edge_orientations(cube, edge_orientations);
                cubie_apply_move(cube, move);
                move_table_edge_orientations[edge_orientations * N_MOVES + move] = get_edge_orientations(cube);
            }
        }

        free(cube);
    }

    // Corner orientation move tables
    if (move_table_corner_orientations == NULL) {
        move_table_corner_orientations = (int *)malloc(sizeof(int) * N_CORNER_ORIENTATIONS * N_MOVES);

        cube = init_cubie_cube();

        for (int corner_orientations = 0; corner_orientations < N_CORNER_ORIENTATIONS; corner_orientations++) {
            for (int move = 0; move < N_MOVES; move++) {
                set_corner_orientations(cube, corner_orientations);
                cubie_apply_move(cube, move);
                move_table_corner_orientations[corner_orientations * N_MOVES + move] = get_corner_orientations(cube);
            }
        }

        free(cube);
    }

    // UD slice move table
    if (move_table_UD_slice == NULL) {
        move_table_UD_slice = (int *)malloc(sizeof(int) * N_SLICES * N_MOVES);

        cube = init_cubie_cube();

        for (int slice = 0; slice < N_SLICES; slice++) {
            for (int move = 0; move < N_MOVES; move++) {
                set_UD_slice(cube, slice);
                cubie_apply_move(cube, move);
                move_table_UD_slice[slice * N_MOVES + move] = get_UD_slice(cube);
            }
        }

        free(cube);
    }

    // Corner permutations move table
    if (move_table_corner_permutations == NULL) {
        move_table_corner_permutations = (int *)malloc(sizeof(int) * N_CORNER_PERMUTATIONS * N_MOVES);

        cube = init_cubie_cube();

        for (int permutations = 0; permutations < N_CORNER_PERMUTATIONS; permutations++) {
            for (int move = 0; move < N_MOVES; move++) {
                set_corner_permutations(cube, permutations);
                cubie_apply_move(cube, move);
                move_table_corner_permutations[permutations * N_MOVES + move] = get_corner_permutations(cube);
            }
        }

        free(cube);
    }
}
