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

#include "cubie_move_table.h"
#include "definitions.h"
#include "puzzle_2x2.h"

static int corner_facelets[8][3] = {
    {3, 4, 9},    // URF: U4(3), R1(4), F2(9)
    {2, 8, 17},   // UFL: U3(2), F1(8), L2(17)
    {0, 16, 21},  // ULB: U1(0), L1(16), B2(21)
    {1, 20, 5},   // UBR: U2(1), B1(20), R2(5)
    {13, 11, 6},  // DFR: D2(13), F4(11), R3(6)
    {12, 19, 10}, // DLF: D1(12), L4(19), F3(10)
    {14, 23, 18}, // DBL: D3(14), B4(23), L3(18)
    {15, 7, 22},  // DRB: D4(15), R4(7), B3(22)
};

static color_t corner_colors[8][3] = {
    {U, R, F}, {U, F, L}, {U, L, B}, {U, B, R}, {D, F, R}, {D, L, F}, {D, B, L}, {D, R, B},
};

static void reset(void *state) {
    cube_2x2_t *cube = (cube_2x2_t *)state;

    for (int i = 0; i < N_CORNERS; i++) {
        cube->corner_permutations[i] = i;
        cube->corner_orientations[i] = 0;
    }
}

static int is_solved(const void *state) {
    const cube_2x2_t *cube = (const cube_2x2_t *)state;

    for (int i = 0; i < N_CORNERS; i++) {
        if (cube->corner_permutations[i] != (corner_t)i || cube->corner_orientations[i] != 0)
            return 0;
    }

    return 1;
}

static void apply_move_raw(cube_2x2_t *cube, const corner_t cp[8], const int co[8]) {
    cube_2x2_t result;

    for (int i = 0; i < N_CORNERS; i++) {
        result.corner_permutations[i] = cube->corner_permutations[cp[i]];
        result.corner_orientations[i] = (cube->corner_orientations[cp[i]] + co[i]) % 3;
    }

    memcpy(cube, &result, sizeof(cube_2x2_t));
}

static void apply_face(cube_2x2_t *cube, const corner_t cp[8], const int co[8], int n_turns) {
    for (int t = 0; t < n_turns; t++) {
        apply_move_raw(cube, cp, co);
    }
}

static void apply_move(void *state, move_t move) {
    cube_2x2_t *cube = (cube_2x2_t *)state;

    switch (move) {
        case MOVE_U1: apply_face(cube, corner_permutation_U, corner_orientation_U, 1); break;
        case MOVE_U2: apply_face(cube, corner_permutation_U, corner_orientation_U, 2); break;
        case MOVE_U3: apply_face(cube, corner_permutation_U, corner_orientation_U, 3); break;
        case MOVE_R1: apply_face(cube, corner_permutation_R, corner_orientation_R, 1); break;
        case MOVE_R2: apply_face(cube, corner_permutation_R, corner_orientation_R, 2); break;
        case MOVE_R3: apply_face(cube, corner_permutation_R, corner_orientation_R, 3); break;
        case MOVE_F1: apply_face(cube, corner_permutation_F, corner_orientation_F, 1); break;
        case MOVE_F2: apply_face(cube, corner_permutation_F, corner_orientation_F, 2); break;
        case MOVE_F3: apply_face(cube, corner_permutation_F, corner_orientation_F, 3); break;
        default: break;
    }
}

static void copy(void *dst, const void *src) { memcpy(dst, src, sizeof(cube_2x2_t)); }

static int from_string(void *state, const char *str) {
    cube_2x2_t *cube      = (cube_2x2_t *)state;
    color_t     colors[6] = {U, R, F, D, L, B};

    reset(state);

    color_t color_cube[N_FACELETS_2X2];

    for (int i = 0; i < N_FACELETS_2X2; i++) {
        switch (str[i]) {
            case 'U': color_cube[i] = colors[U]; break;
            case 'R': color_cube[i] = colors[R]; break;
            case 'F': color_cube[i] = colors[F]; break;
            case 'D': color_cube[i] = colors[D]; break;
            case 'L': color_cube[i] = colors[L]; break;
            case 'B': color_cube[i] = colors[B]; break;
            default: return 0;
        }
    }

    for (int i = 0; i < N_CORNERS; i++) {
        int orientation = 0;

        for (orientation = 0; orientation < 3; orientation++) {
            if (color_cube[corner_facelets[i][orientation]] == U || color_cube[corner_facelets[i][orientation]] == D)
                break;
        }

        color_t color_a = color_cube[corner_facelets[i][(orientation + 1) % 3]];
        color_t color_b = color_cube[corner_facelets[i][(orientation + 2) % 3]];

        for (int j = 0; j < N_CORNERS; j++) {
            if (color_a == corner_colors[j][1] && color_b == corner_colors[j][2]) {
                cube->corner_permutations[i] = (corner_t)j;
                cube->corner_orientations[i] = orientation;
            }
        }
    }

    return 1;
}

static void to_string(const void *state, char *buf, size_t bufsz) {
    const cube_2x2_t *cube = (const cube_2x2_t *)state;
    color_t           facelet_cube[N_FACELETS_2X2];

    for (int i = 0; i < N_CORNERS; i++) {
        int corner = cube->corner_permutations[i];
        int orient = cube->corner_orientations[i];

        facelet_cube[corner_facelets[i][orient]]           = corner_colors[corner][0];
        facelet_cube[corner_facelets[i][(orient + 1) % 3]] = corner_colors[corner][1];
        facelet_cube[corner_facelets[i][(orient + 2) % 3]] = corner_colors[corner][2];
    }

    for (int i = 0; i < N_FACELETS_2X2 && i < (int)bufsz - 1; i++) {
        switch (facelet_cube[i]) {
            case U: buf[i] = 'U'; break;
            case R: buf[i] = 'R'; break;
            case F: buf[i] = 'F'; break;
            case D: buf[i] = 'D'; break;
            case L: buf[i] = 'L'; break;
            case B: buf[i] = 'B'; break;
        }
    }

    size_t term = (N_FACELETS_2X2 < bufsz) ? N_FACELETS_2X2 : bufsz - 1;
    buf[term]   = '\0';
}

const puzzle_ops_t puzzle_2x2_ops = {
    .name       = "2x2",
    .n_moves    = 9,
    .state_size = sizeof(cube_2x2_t),

    .reset       = reset,
    .is_solved   = is_solved,
    .apply_move  = apply_move,
    .copy        = copy,
    .from_string = from_string,
    .to_string   = to_string,
};
