#include <stdio.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "solve.h"
#include "utils.h"

int main() {
    build_move_tables();

    coord_cube_t *cube = get_coord_cube();
    coord_apply_move(cube, MOVE_F1);
    coord_apply_move(cube, MOVE_U1);
    coord_apply_move(cube, MOVE_R3);
    coord_apply_move(cube, MOVE_F2);

    move_t *solution = solve(cube);

    if (solution == NULL) {
        printf("failed to solve\n");
    } else {
        for (int i = 0; solution[i] != MOVE_NULL; i++)
            printf(" %s", move_to_str(solution[i]));

        printf("\n");
    }

    return 0;
}
