#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cubie_cube.h"
#include "definitions.h"
#include "facelets.h"

facelet_t corner_facelets[N_CORNERS][3] = {{U9, R1, F3}, {U7, F1, L3}, {U1, L1, B3}, {U3, B1, R3},
                                           {D3, F9, R7}, {D1, L9, F7}, {D7, B9, L7}, {D9, R9, B7}};

color_t corner_colors[N_CORNERS][3] = {{U, R, F}, {U, F, L}, {U, L, B}, {U, B, R},
                                       {D, F, R}, {D, L, F}, {D, B, L}, {D, R, B}};

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

    free(color_cube);

    return cubie_cube;
}
