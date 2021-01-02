#include <stdio.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "solve.h"
#include "utils.h"

int main() {
    build_move_tables();

    coord_cube_t *cube = get_coord_cube();

    move_t moves[4] = {
        MOVE_R1,
        MOVE_B1,
        MOVE_L1,
        MOVE_F1,
    };

    for (int i = 0; i < 4; i++) {
        coord_apply_move(cube, moves[i]);
        printf(" %s", move_to_str(moves[i]));
    }
    printf(" : %4d %4d %3d\n\n", cube->edge_orientations, cube->corner_orientations, cube->UD_slice);

    move_t *solution = solve(cube);

    if (solution == NULL) {
        printf("failed to solve\n");
    } else {
        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            printf(" %s", move_to_str(solution[i]));
            coord_apply_move(cube, solution[i]);
        }

        printf(" : %4d %4d %3d\n\n", cube->edge_orientations, cube->corner_orientations, cube->UD_slice);
    }

    return 0;
}
