#include <stdio.h>
#include <stdlib.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "utils.h"

#define max_moves 4

move_t *solve(coord_cube_t *cube) {
    move_t *solution = NULL;

    move_t        move_stack[max_moves];
    coord_cube_t *cube_stack[max_moves];
    int           pivot      = 0;
    int           move_count = 0;

    char buffer[512];

    for (int i = 0; i < max_moves; i++) {
        move_stack[i] = -1;
        cube_stack[i] = random_coord_cube();
    }

    copy_coord_cube(cube_stack[0], cube);
    printf("scrambled cube: %d %d\n\n", cube->edge_orientations, cube->corner_orientations);

    do {
        move_stack[pivot]++;

        if (move_stack[pivot] >= N_MOVES) {
            move_stack[pivot] = -1;
            /*printf("\n");*/
            pivot--;

            if (pivot < 0) {
                break;
            } else if (pivot == 0) {
                copy_coord_cube(cube_stack[0], cube);
            } else if (pivot > 0) {
                copy_coord_cube(cube_stack[pivot], cube_stack[pivot - 1]);
            }

            continue;
        }

        coord_apply_move(cube_stack[pivot], move_stack[pivot]);
        move_count++;

        if (move_count % 1000000 == 0) {
            sprintf(buffer, " moves: %4d pivot: %2d", move_count, pivot);
            sprintf(buffer, "%s : %7d %7d -> ", buffer, cube_stack[pivot]->edge_orientations,
                    cube_stack[pivot]->corner_orientations);
            for (int i = 0; i <= pivot; i++)
                sprintf(buffer, "%s %s", buffer, move_to_str(move_stack[i]));
            printf("%s\n", buffer);
        }

        if (cube_stack[pivot]->edge_orientations + cube_stack[pivot]->corner_orientations == 0) {
            solution = malloc(sizeof(move_t) * (pivot + 1));

            for (int i = 0; i <= pivot; i++)
                solution[i] = get_reverse_move(move_stack[i]);
            solution[pivot + 1] = MOVE_NULL;

            break;
        }

        if (pivot < max_moves - 1) {
            copy_coord_cube(cube_stack[pivot + 1], cube_stack[pivot]);
            pivot++;
        } else {
            copy_coord_cube(cube_stack[pivot], cube_stack[pivot - 1]);
        }
    } while (1);

    return solution;
}
