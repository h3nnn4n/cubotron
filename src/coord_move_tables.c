#include <assert.h>

#include "coord_cube.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "definitions.h"

static int *move_table_edge_orientations   = NULL;
static int *move_table_corner_orientations = NULL;

void coord_apply_move(coord_cube_t *cube, move_t move) {
    assert(cube != NULL);
    assert(move >= 0);
    assert(move < N_MOVES);
    assert(move_table_edge_orientations != NULL);
    assert(move_table_corner_orientations != NULL);
    assert(cube->edge_orientations * N_MOVES + move < N_EDGE_ORIENTATIONS * N_MOVES);
    assert(cube->corner_orientations * N_MOVES + move < N_CORNER_ORIENTATIONS * N_MOVES);

    cube->edge_orientations   = move_table_edge_orientations[cube->edge_orientations * N_MOVES + move];
    cube->corner_orientations = move_table_corner_orientations[cube->corner_orientations * N_MOVES + move];
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
}
