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

#include "cubie_cube.h"
#include "definitions.h"
#include "facelets.h"
#include "utils.h"

facelet_t corner_facelets[N_CORNERS][3] = {{U9, R1, F3}, {U7, F1, L3}, {U1, L1, B3}, {U3, B1, R3},
                                           {D3, F9, R7}, {D1, L9, F7}, {D7, B9, L7}, {D9, R9, B7}};

facelet_t edge_facelets[N_EDGES][2] = {{U6, R2}, {U8, F2}, {U4, L2}, {U2, B2}, {D6, R8}, {D2, F8},
                                       {D4, L8}, {D8, B8}, {F6, R4}, {F4, L6}, {B6, L4}, {B4, R6}};

color_t corner_colors[N_CORNERS][3] = {{U, R, F}, {U, F, L}, {U, L, B}, {U, B, R},
                                       {D, F, R}, {D, L, F}, {D, B, L}, {D, R, B}};

color_t edge_colors[N_EDGES][2] = {{U, R}, {U, F}, {U, L}, {U, B}, {D, R}, {D, F},
                                   {D, L}, {D, B}, {F, R}, {F, L}, {B, L}, {B, R}};

int verify_valid_facelets(char facelets[N_FACELETS]) {
    int faces[N_COLORS] = {0};

    for (int i = 0; i < N_FACELETS; i++) {
        switch (facelets[i]) {
            case 'U': faces[U] += 1; break;
            case 'R': faces[R] += 1; break;
            case 'F': faces[F] += 1; break;
            case 'D': faces[D] += 1; break;
            case 'L': faces[L] += 1; break;
            case 'B': faces[B] += 1; break;
        }
    }

    for (int i = 0; i < N_COLORS; i++) {
        if (faces[i] != 9)
            return 0;
    }

    return 1;
}

color_t *build_facelet(char facelets[N_FACELETS]) {
    color_t *facelet_cube = (color_t *)malloc(sizeof(facelet_t) * N_FACELETS);

    for (int i = 0; i < N_FACELETS; i++) {
        switch (facelets[i]) {
            case 'U': facelet_cube[i] = U; break;
            case 'R': facelet_cube[i] = R; break;
            case 'F': facelet_cube[i] = F; break;
            case 'D': facelet_cube[i] = D; break;
            case 'L': facelet_cube[i] = L; break;
            case 'B': facelet_cube[i] = B; break;
        }
    }

    return facelet_cube;
}

cube_cubie_t *build_cubie_cube_from_str(char facelets[N_FACELETS]) {
    cube_cubie_t *cubie_cube = init_cubie_cube();
    color_t *     color_cube = build_facelet(facelets);

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
                cubie_cube->corner_permutations[i] = j;
                cubie_cube->corner_orientations[i] = orientation;
            }
        }
    }

    for (int i = 0; i < N_EDGES; i++) {
        for (int j = 0; j < N_EDGES; j++) {
            if (color_cube[edge_facelets[i][0]] == edge_colors[j][0] &&
                color_cube[edge_facelets[i][1]] == edge_colors[j][1]) {
                cubie_cube->edge_permutations[i] = j;
                cubie_cube->edge_orientations[i] = 0;
                break;
            }

            if (color_cube[edge_facelets[i][0]] == edge_colors[j][1] &&
                color_cube[edge_facelets[i][1]] == edge_colors[j][0]) {
                cubie_cube->edge_permutations[i] = j;
                cubie_cube->edge_orientations[i] = 1;
                break;
            }
        }
    }

    free(color_cube);

    assert(is_valid(cubie_cube));

    return cubie_cube;
}
