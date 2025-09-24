/*
 * Copyright <2021> <Renan S Silva, aka h3nnn4n>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <assert.h>
#include <stdio.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "definitions.h"
#include "pruning_cache.h"

static int *move_table_edge_orientations          = NULL;
static int *move_table_corner_orientations        = NULL;
static int *move_table_E_slice                    = NULL;
static int *move_table_E_sorted_slice             = NULL;
static int *move_table_UD6_edge_permutations      = NULL;
static int *move_table_UD7_edge_permutations      = NULL;
static int *move_table_corner_permutations        = NULL;
static int  move_table_parity[N_PARITY * N_MOVES] = {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1,
                                                     0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};

int *get_move_table_edge_orientations() { return move_table_edge_orientations; }
int *get_move_table_corner_orientations() { return move_table_corner_orientations; }
int *get_move_table_E_slice() { return move_table_E_slice; }
int *get_move_table_E_sorted_slice() { return move_table_E_sorted_slice; }
int *get_move_table_UD6_edge_permutations() { return move_table_UD6_edge_permutations; }
int *get_move_table_UD7_edge_permutations() { return move_table_UD7_edge_permutations; }
int *get_move_table_corner_permutations() { return move_table_corner_permutations; }
int *get_move_table_parity() { return move_table_parity; }

void coord_apply_move(coord_cube_t *cube, move_t move) {
    assert(cube != NULL);
    assert(move >= 0);
    assert(move < N_MOVES);
    assert(move_table_edge_orientations != NULL);
    assert(move_table_corner_orientations != NULL);
    assert(move_table_E_slice != NULL);
    assert(move_table_E_sorted_slice != NULL);
    assert(move_table_UD6_edge_permutations != NULL);
    assert(move_table_UD7_edge_permutations != NULL);
    assert(move_table_corner_permutations != NULL);

    assert(cube->edge_orientations * N_MOVES + move < N_EDGE_ORIENTATIONS * N_MOVES);
    assert(cube->corner_orientations * N_MOVES + move < N_CORNER_ORIENTATIONS * N_MOVES);
    assert(cube->E_slice * N_MOVES + move < N_SLICES * N_MOVES);
    assert(cube->E_sorted_slice * N_MOVES + move < N_SORTED_SLICES * N_MOVES);
    assert(cube->UD6_edge_permutations * N_MOVES + move < N_UD6_PHASE1_PERMUTATIONS * N_MOVES);
    assert(cube->UD7_edge_permutations * N_MOVES + move < N_UD7_PHASE1_PERMUTATIONS * N_MOVES);
    assert(cube->corner_permutations * N_MOVES + move < N_CORNER_PERMUTATIONS * N_MOVES);
    assert(cube->parity * N_MOVES + move < N_PARITY * N_MOVES);

    // Phase 1
    cube->edge_orientations   = move_table_edge_orientations[cube->edge_orientations * N_MOVES + move];
    cube->corner_orientations = move_table_corner_orientations[cube->corner_orientations * N_MOVES + move];
    cube->E_slice             = move_table_E_slice[cube->E_slice * N_MOVES + move];

    // Phase 2
    cube->E_sorted_slice        = move_table_E_sorted_slice[cube->E_sorted_slice * N_MOVES + move];
    cube->parity                = move_table_parity[cube->parity * N_MOVES + move];
    cube->UD6_edge_permutations = move_table_UD6_edge_permutations[cube->UD6_edge_permutations * N_MOVES + move];
    cube->UD7_edge_permutations = move_table_UD7_edge_permutations[cube->UD7_edge_permutations * N_MOVES + move];
    cube->corner_permutations   = move_table_corner_permutations[cube->corner_permutations * N_MOVES + move];

    // Post conditions
    assert(cube->edge_orientations >= 0);
    assert(cube->corner_orientations >= 0);
    assert(cube->E_slice >= 0);
    assert(cube->E_sorted_slice >= 0);
    assert(cube->parity >= 0);
    assert(cube->UD6_edge_permutations >= 0);
    assert(cube->UD7_edge_permutations >= 0);
    assert(cube->corner_permutations >= 0);

    assert(cube->edge_orientations < N_EDGE_ORIENTATIONS);
    assert(cube->corner_orientations < N_CORNER_ORIENTATIONS);
    assert(cube->E_slice < N_SLICES);
    assert(cube->E_sorted_slice < N_SORTED_SLICES);
    assert(cube->parity < N_PARITY);
    assert(cube->UD6_edge_permutations < N_UD6_PHASE1_PERMUTATIONS);
    assert(cube->UD7_edge_permutations < N_UD7_PHASE1_PERMUTATIONS);
    assert(cube->corner_permutations < N_CORNER_PERMUTATIONS);
}

void coord_apply_moves(coord_cube_t *cube, move_t *moves, int n_moves) {
    for (int i = 0; i < n_moves; i++) {
        coord_apply_move(cube, moves[i]);
    }
}

void coord_build_move_tables() {
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

    // E slice move table
    if (move_table_E_slice == NULL) {
        move_table_E_slice = (int *)malloc(sizeof(int) * N_SLICES * N_MOVES);

        cube = init_cubie_cube();

        for (int slice = 0; slice < N_SLICES; slice++) {
            for (int move = 0; move < N_MOVES; move++) {
                set_E_slice(cube, slice);
                cubie_apply_move(cube, move);
                move_table_E_slice[slice * N_MOVES + move] = get_E_slice(cube);
            }
        }

        free(cube);
    }

    // E sorted slice move table
    if (move_table_E_sorted_slice == NULL) {
        move_table_E_sorted_slice = (int *)malloc(sizeof(int) * N_SORTED_SLICES * N_MOVES);

        cube = init_cubie_cube();

        for (int slice = 0; slice < N_SORTED_SLICES; slice++) {
            for (int move = 0; move < N_MOVES; move++) {
                set_E_sorted_slice(cube, slice);
                cubie_apply_move(cube, move);
                assert(slice * N_MOVES + move < N_SORTED_SLICES * N_MOVES);
                move_table_E_sorted_slice[slice * N_MOVES + move] = get_E_sorted_slice(cube);
            }
        }

        free(cube);
    }

    // UD6 edge permutations move table
    build_UD6_edge_permutations_move_table();

    // UD7 edge permutations move table
    build_UD7_edge_permutations_move_table();

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

void build_UD6_edge_permutations_move_table() {
    cube_cubie_t *cube = NULL;

    if (move_table_UD6_edge_permutations != NULL)
        return;

    if (pruning_table_cache_load("move_tables", "UD6_edge_permutations", &move_table_UD6_edge_permutations,
                                 N_UD6_PHASE1_PERMUTATIONS * N_MOVES))
        return;

    move_table_UD6_edge_permutations = (int *)malloc(sizeof(int) * N_UD6_PHASE1_PERMUTATIONS * N_MOVES);

    cube = init_cubie_cube();

    for (int permutations = 0; permutations < N_UD6_PHASE1_PERMUTATIONS; permutations++) {
        for (int move = 0; move < N_MOVES; move++) {
            set_UD6_edges(cube, permutations);
            cubie_apply_move(cube, move);
            move_table_UD6_edge_permutations[permutations * N_MOVES + move] = get_UD6_edges(cube);
        }
    }

    pruning_table_cache_store("move_tables", "UD6_edge_permutations", move_table_UD6_edge_permutations,
                              N_UD6_PHASE1_PERMUTATIONS * N_MOVES);

    free(cube);
}

void build_UD7_edge_permutations_move_table() {
    cube_cubie_t *cube = NULL;

    if (move_table_UD7_edge_permutations != NULL)
        return;

    if (pruning_table_cache_load("move_tables", "UD7_edge_permutations", &move_table_UD7_edge_permutations,
                                 N_UD7_PHASE1_PERMUTATIONS * N_MOVES))
        return;

    move_table_UD7_edge_permutations = (int *)malloc(sizeof(int) * N_UD7_PHASE1_PERMUTATIONS * N_MOVES);

    cube = init_cubie_cube();

    for (int permutations = 0; permutations < N_UD7_PHASE1_PERMUTATIONS; permutations++) {
        for (int move = 0; move < N_MOVES; move++) {
            set_UD7_edges(cube, permutations);
            cubie_apply_move(cube, move);
            int value = get_UD7_edges(cube);

            assert(value >= 0);
            assert(value < N_UD7_PHASE1_PERMUTATIONS);

            move_table_UD7_edge_permutations[permutations * N_MOVES + move] = value;
        }
    }

    pruning_table_cache_store("move_tables", "UD7_edge_permutations", move_table_UD7_edge_permutations,
                              N_UD7_PHASE1_PERMUTATIONS * N_MOVES);

    free(cube);
}
