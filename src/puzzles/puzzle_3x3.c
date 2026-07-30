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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cubie_cube.h"
#include "cubie_move_table.h"
#include "definitions.h"
#include "facelets.h"
#include "puzzle_3x3.h"
#include "utils.h"

static void puzzle_3x3_reset(void *state) {
    cube_cubie_t *cube = (cube_cubie_t *)state;

    for (int i = 0; i < N_CORNERS; i++) {
        cube->corner_permutations[i] = i;
        cube->corner_orientations[i] = 0;
    }

    for (int i = 0; i < N_EDGES; i++) {
        cube->edge_permutations[i] = i;
        cube->edge_orientations[i] = 0;
    }
}

static int puzzle_3x3_is_solved(const void *state) { return is_cubie_solved((const cube_cubie_t *)state); }

static void puzzle_3x3_apply_move(void *state, move_t move) { cubie_apply_move((cube_cubie_t *)state, move); }

static void puzzle_3x3_copy(void *dst, const void *src) { memcpy(dst, src, sizeof(cube_cubie_t)); }

static int puzzle_3x3_from_string(void *state, const char *str) {
    cube_cubie_t *tmp = build_cubie_cube_from_str((char *)str);
    memcpy(state, tmp, sizeof(cube_cubie_t));
    free(tmp);

    return 1;
}

static void puzzle_3x3_to_string(const void *state, char *buf, size_t bufsz) {
    const cube_cubie_t *cube = (const cube_cubie_t *)state;
    color_t             facelet_cube[N_FACELETS];

    extern facelet_t corner_facelets[N_CORNERS][3];
    extern facelet_t edge_facelets[N_EDGES][2];
    extern color_t   corner_colors[N_CORNERS][3];
    extern color_t   edge_colors[N_EDGES][2];

    for (int i = 0; i < N_CORNERS; i++) {
        int corner = cube->corner_permutations[i];
        int orient = cube->corner_orientations[i];

        facelet_cube[corner_facelets[i][orient]]           = corner_colors[corner][0];
        facelet_cube[corner_facelets[i][(orient + 1) % 3]] = corner_colors[corner][1];
        facelet_cube[corner_facelets[i][(orient + 2) % 3]] = corner_colors[corner][2];
    }

    for (int i = 0; i < N_EDGES; i++) {
        int edge   = cube->edge_permutations[i];
        int orient = cube->edge_orientations[i];

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

    size_t term = (N_FACELETS < bufsz) ? N_FACELETS : bufsz - 1;
    buf[term]   = '\0';
}

const puzzle_ops_t puzzle_3x3_ops = {
    .name       = "3x3",
    .n_moves    = N_MOVES,
    .state_size = sizeof(cube_cubie_t),

    .reset       = puzzle_3x3_reset,
    .is_solved   = puzzle_3x3_is_solved,
    .apply_move  = puzzle_3x3_apply_move,
    .copy        = puzzle_3x3_copy,
    .from_string = puzzle_3x3_from_string,
    .to_string   = puzzle_3x3_to_string,
};
