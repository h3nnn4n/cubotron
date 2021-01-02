#include <stdio.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "solve.h"
#include "utils.h"

int main() {
    build_move_tables();

    coord_cube_t *cube = get_coord_cube();
    coord_apply_move(cube, MOVE_R1);
    coord_apply_move(cube, MOVE_B1);
    coord_apply_move(cube, MOVE_L1);
    coord_apply_move(cube, MOVE_F1);

    printf("scrambled cube: %d %d\n\n", cube->edge_orientations, cube->corner_orientations);
    move_t *solution = solve(cube);

    if (solution == NULL) {
        printf("failed to solve\n");
    } else {
        for (int i = 0; solution[i] != MOVE_NULL; i++) {
            printf(" %s", move_to_str(solution[i]));
            coord_apply_move(cube, solution[i]);
        }

        printf("\n");

        printf("\n   solved cube: %d %d\n\n", cube->edge_orientations, cube->corner_orientations);
    }

    return 0;
}
