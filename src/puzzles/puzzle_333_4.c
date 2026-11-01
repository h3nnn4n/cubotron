/*
 * Copyright <2026> <Renan S Silva, aka h3nnn4n>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "definitions.h"
#include "facelets.h"
#include "puzzle_333_4.h"
#include "utils.h"

edge_t   edge_permutation_U[]   = {UB, UR, UF, UL, DR, DF, DL, DB, FR, FL, BL, BR};

edge_t   edge_permutation_R[]   = {FR, UF, UL, UB, BR, DF, DL, DB, DR, FL, BL, UR};

edge_t   edge_permutation_F[]   = {UR, FL, UL, UB, DR, FR, DL, DB, UF, DF, BL, BR};

edge_t   edge_permutation_D[]   = {UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR};

edge_t   edge_permutation_L[]   = {UR, UF, BL, UB, DR, DF, FL, DB, FR, UL, DL, BR};

edge_t   edge_permutation_B[]   = {UR, UF, UL, BR, DR, DF, DL, BL, FR, FL, UB, DB};

static void build_3334_move_table() {

}

static void reset(void *state) {
    cube_333_4_t *cube = (cube_333_4_t *)state;

    for (int i = 0; i < N_CORNERS; i++) {
        cube->base.corner_permutations[i] = i;
        cube->base.corner_orientations[i] = 0;
    }

    for (int i = 0; i < N_EDGES; i++) {
        cube->base.edge_permutations[i] = i;
        cube->base.edge_orientations[i] = 0;
    }

    cube->centers = 0;
}

static int is_solved(const void *state) {
    const cube_333_4_t *cube = (const cube_333_4_t *)state;

    return is_cubie_solved(&cube->base) && cube->centers == 0;
}

static bool is_333_4_valid(cube_333_4_t *cube) {
    bool centers_valid = cube->centers < 24;

    return centers_valid && is_valid(&cube->base);
}

static void apply_move(void *state, move_t move) {
    assert(state != NULL);
    if (move == MOVE_NULL)
        return;

    cube_333_4_t *cube = (cube_333_4_t *)state;

    assert(cube != NULL);
    assert(move>= 0 && move< N_MOVES);
    assert(is_333_4_valid(cube));

    cubie_apply_move(&cube->base, move);

    /*assert(move_table_centers != NULL);*/

    /*multiply_cube_cubie(cube, move_table_centers[move]);*/

    assert(is_333_4_valid(cube));
}

static void copy(void *dst, const void *src) {
    memcpy(dst, src, sizeof(cube_333_4_t));
}

static int from_string(void *state, const char *str) {
    cube_333_4_t *tmp = (cube_333_4_t *)build_cubie_cube_from_str((char *)str);
    memcpy(&((cube_333_4_t *)state)->base, tmp, sizeof(cube_cubie_t));
    free(tmp);

    // TODO: parse 333_4 extras portion of the string
    // TODO: 333_4 extras reset

    return 1;
}

static void to_string(const void *state, char *buf, size_t bufsz) {
    const cube_333_4_t *cube = (const cube_333_4_t *)state;
    color_t             facelet_cube[N_FACELETS];

    extern facelet_t corner_facelets[N_CORNERS][3];
    extern facelet_t edge_facelets[N_EDGES][2];
    extern color_t   corner_colors[N_CORNERS][3];
    extern color_t   edge_colors[N_EDGES][2];

    for (int i = 0; i < N_CORNERS; i++) {
        int corner = cube->base.corner_permutations[i];
        int orient = cube->base.corner_orientations[i];

        facelet_cube[corner_facelets[i][orient]]           = corner_colors[corner][0];
        facelet_cube[corner_facelets[i][(orient + 1) % 3]] = corner_colors[corner][1];
        facelet_cube[corner_facelets[i][(orient + 2) % 3]] = corner_colors[corner][2];
    }

    for (int i = 0; i < N_EDGES; i++) {
        int edge   = cube->base.edge_permutations[i];
        int orient = cube->base.edge_orientations[i];

        if (orient == 0) {
            facelet_cube[edge_facelets[i][0]] = edge_colors[edge][0];
            facelet_cube[edge_facelets[i][1]] = edge_colors[edge][1];
        } else {
            facelet_cube[edge_facelets[i][0]] = edge_colors[edge][1];
            facelet_cube[edge_facelets[i][1]] = edge_colors[edge][0];
        }
    }

    for (int i = 0; i < N_FACELETS && i < (int)bufsz - 1; i++) {
        switch (facelet_cube[i]) {
            case U: buf[i] = 'U'; break;
            case R: buf[i] = 'R'; break;
            case F: buf[i] = 'F'; break;
            case D: buf[i] = 'D'; break;
            case L: buf[i] = 'L'; break;
            case B: buf[i] = 'B'; break;
        }
    }

    // TODO: append 333_4 extras portion of the string

    size_t term = (N_FACELETS < bufsz) ? N_FACELETS : bufsz - 1;
    buf[term]   = '\0';
}

const puzzle_ops_t puzzle_333_4_ops = {
    .name       = "333_4",
    .n_moves    = N_MOVES,
    .state_size = sizeof(cube_333_4_t),

    .reset       = reset,
    .is_solved   = is_solved,
    .apply_move  = apply_move,
    .copy        = copy,
    .from_string = from_string,
    .to_string   = to_string,
};
