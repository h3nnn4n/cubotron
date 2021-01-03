#include <pcg_variants.h>
#include <stdio.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "solve.h"
#include "utils.h"

#define n_scrable_moves 20

int main() {
    build_move_tables();

    coord_cube_t *cube = get_coord_cube();

    move_t moves[n_scrable_moves] = {0};
    for (int i = 0; i < n_scrable_moves; i++) {
        moves[i] = pcg32_boundedrand(N_MOVES);

        do {
            moves[i] = pcg32_boundedrand(N_MOVES);
        } while (i > 0 && moves[i] == moves[i - 1]);
    }

    for (int i = 0; i < n_scrable_moves; i++) {
        coord_apply_move(cube, moves[i]);
        printf(" %s", move_to_str(moves[i]));
    }
    printf(" : %4d %4d %3d\n\n", cube->edge_orientations, cube->corner_orientations, cube->UD_slice);

    move_t *solution = solve(cube);

    if (solution == NULL) {
        printf("failed to solve");
    } else {
        printf("solution:\n");
        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            printf(" %s", move_to_str(solution[i]));
            coord_apply_move(cube, solution[i]);
        }
        printf("\n");
    }

    return 0;
}
