#include <stdio.h>

#include "coord_move_tables.h"
#include "utils.h"

#define max_moves 7

int main() {
    build_move_tables();

    coord_cube_t *cube = random_coord_cube();

    move_t        move_stack[max_moves];
    coord_cube_t *cube_stack[max_moves];
    int           pivot      = 0;
    int           move_count = 0;

    char buffer[512];

    for (int i = 0; i < max_moves; i++) {
        move_stack[i] = -1;
        cube_stack[i] = get_coord_cube();
    }

    copy_coord_cube(cube_stack[0], cube);
    printf("%d %d\n", cube->edge_orientations, cube->corner_orientations);

    do {
        move_stack[pivot]++;

        if (move_stack[pivot] >= N_MOVES - 1) {
            move_stack[pivot] = -1;
            /*printf("\n");*/
            pivot--;
            continue;
        }

        coord_apply_move(cube_stack[pivot], move_stack[pivot]);
        move_count++;

        if (move_count % 10000000 == 0) {
            sprintf(buffer, " %10d %2d | %3d | ", move_count, pivot, move_stack[pivot]);
            sprintf(buffer, "%s : %7d %7d -> ", buffer, cube_stack[pivot]->edge_orientations,
                    cube_stack[pivot]->corner_orientations);
            for (int i = 0; i <= pivot; i++)
                sprintf(buffer, "%s %s", buffer, move_to_str(move_stack[i]));
            printf("%s\n", buffer);
        }

        if (pivot < max_moves - 1) {
            copy_coord_cube(cube_stack[pivot], cube_stack[pivot + 1]);
            pivot++;
        }
    } while (1);

    return 0;
}
