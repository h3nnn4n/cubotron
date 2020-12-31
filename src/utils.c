#include <assert.h>

#include "cubie.h"

static char *_move_t_to_str[] = {
    "MOVE_U2", "MOVE_U2", "MOVE_U3", "MOVE_R1", "MOVE_R2", "MOVE_R3", "MOVE_F1", "MOVE_F2", "MOVE_F3",   "MOVE_D1",
    "MOVE_D2", "MOVE_D3", "MOVE_L1", "MOVE_L2", "MOVE_L3", "MOVE_B1", "MOVE_B2", "MOVE_B3", "MOVE_NULL",
};

int cubie_off_count(cube_cubie *cube) {
    int count = 0;

    for (int i = 0; i < 8; i++) {
        if (i != (int)cube->corner_permutations[i])
            count += 1;

        if (0 != cube->corner_orientations[i])
            count += 1;
    }

    for (int i = 0; i < 12; i++) {
        if (i != (int)cube->edge_permutations[i])
            count += 1;

        if (0 != cube->edge_orientations[i])
            count += 1;
    }

    return count;
}

int is_solved(cube_cubie *cube) { return cubie_off_count(cube) == 0; }

char *move_to_str(move_t move) {
    assert(move >= 0);
    assert(move <= 18);

    return _move_t_to_str[move];
}
