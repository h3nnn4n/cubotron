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
#include <stdlib.h>
#include <string.h>

#include <pcg_variants.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "mem_utils.h"
#include "utils.h"

coord_cube_t *get_coord_cube() {
    coord_cube_t *coord_cube = (coord_cube_t *)malloc(sizeof(coord_cube_t));
    reset_coord_cube(coord_cube);
    return coord_cube;
}

void reset_coord_cube(coord_cube_t *cube) {
    assert(cube != NULL);
    memset_(cube, 0, sizeof(coord_cube_t));
}

coord_cube_t *make_coord_cube(cube_cubie_t *cubie) {
    coord_cube_t *coord_cube = get_coord_cube();

    coord_cube->edge_orientations     = get_edge_orientations(cubie);
    coord_cube->corner_orientations   = get_corner_orientations(cubie);
    coord_cube->E_slice               = get_E_slice(cubie);
    coord_cube->E_sorted_slice        = get_E_sorted_slice(cubie);
    coord_cube->parity                = get_edge_parity(cubie);
    coord_cube->UD6_edge_permutations = get_UD6_edges(cubie);
    coord_cube->UD7_edge_permutations = get_UD7_edges(cubie);
    coord_cube->corner_permutations   = get_corner_permutations(cubie);

    return coord_cube;
}

void copy_coord_cube(coord_cube_t *dest, const coord_cube_t *source) {
    assert(dest != NULL);
    assert(source != NULL);
    memcpy_(dest, source, sizeof(coord_cube_t));
}

int are_all_coord_equal(const coord_cube_t *cube1, const coord_cube_t *cube2) {
    if (cube1->edge_orientations != cube2->edge_orientations)
        return 0;

    if (cube1->corner_orientations != cube2->corner_orientations)
        return 0;

    if (cube1->E_slice != cube2->E_slice)
        return 0;

    if (cube1->E_sorted_slice != cube2->E_sorted_slice)
        return 0;

    if (cube1->parity != cube2->parity)
        return 0;

    if (cube1->UD6_edge_permutations != cube2->UD6_edge_permutations)
        return 0;

    if (cube1->UD7_edge_permutations != cube2->UD7_edge_permutations)
        return 0;

    if (cube1->corner_permutations != cube2->corner_permutations)
        return 0;

    return 1;
}

int are_phase1_coord_equal(const coord_cube_t *cube1, const coord_cube_t *cube2) {
    if (cube1->edge_orientations != cube2->edge_orientations)
        return 0;

    if (cube1->corner_orientations != cube2->corner_orientations)
        return 0;

    if (cube1->E_slice != cube2->E_slice)
        return 0;

    return 1;
}

int is_phase1_solved(const coord_cube_t *cube) {
    return (cube->edge_orientations + cube->corner_orientations + cube->E_slice) == 0;
}

int is_phase2_solved(const coord_cube_t *cube) {
    return (cube->UD6_edge_permutations + cube->corner_permutations + cube->E_sorted_slice) == 0;
}

int is_coord_solved(const coord_cube_t *cube) { return is_phase1_solved(cube) && is_phase2_solved(cube); }

int is_move_sequence_a_solution_for_cube(const coord_cube_t *cube, const move_t *moves) {
    coord_cube_t *temp_cube = get_coord_cube();
    copy_coord_cube(temp_cube, cube);

    for (int i = 0; moves[i] != MOVE_NULL; i++) {
        coord_apply_move(temp_cube, moves[i]);
    }

    int result = is_coord_solved(temp_cube);
    free(temp_cube);
    return result;
}

void scramble_cube(coord_cube_t *cube, int n_moves) {
    move_t prev_move = MOVE_NULL;
    for (int i = 0; i < n_moves;) {
        move_t move = pcg32_boundedrand(N_MOVES);
        if (i > 0 && is_duplicated_or_undoes_move(move, prev_move)) {
            continue;
        }
        coord_apply_move(cube, move);
        prev_move = move;
        i++;
    }

    // printf("Scramble: ");
    // for (int i = 0; i < n_moves; i++) {
    //     printf("%s ", move_to_str(scramble_moves[i]));
    // }
    // printf("\n");
}
