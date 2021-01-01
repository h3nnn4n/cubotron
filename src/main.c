#include <stdio.h>

#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "utils.h"

#define max_moves 4

int main() {
    build_move_tables();

    /*coord_cube_t *cube = random_coord_cube();*/
    coord_cube_t *cube = get_coord_cube();
    coord_apply_move(cube, MOVE_R1);
    coord_apply_move(cube, MOVE_F1);
    /*coord_apply_move(cube, MOVE_U1);*/
    /*coord_apply_move(cube, MOVE_F1);*/

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

    sprintf(buffer, " moves: %4d pivot: %2d", move_count, pivot);
    sprintf(buffer, "%s : %7d %7d", buffer, cube->edge_orientations, cube->corner_orientations);
    printf("%s\n", buffer);

    /*coord_apply_move(cube, MOVE_F1);*/

    /*sprintf(buffer, " moves: %4d pivot: %2d", move_count, pivot);*/
    /*sprintf(buffer, "%s : %7d %7d", buffer, cube->edge_orientations, cube->corner_orientations);*/
    /*printf("%s\n", buffer);*/

    /*coord_apply_move(cube, MOVE_F3);*/

    /*sprintf(buffer, " moves: %4d pivot: %2d", move_count, pivot);*/
    /*sprintf(buffer, "%s : %7d %7d", buffer, cube->edge_orientations, cube->corner_orientations);*/
    /*printf("%s\n", buffer);*/

    /*return 0;*/

    do {
        move_stack[pivot]++;

        if (move_stack[pivot] >= N_MOVES) {
            /*if (move_stack[pivot] >= 6) {*/
            move_stack[pivot] = -1;
            printf("\n");
            pivot--;

            if (pivot == 0) {
                copy_coord_cube(cube_stack[0], cube);
            }

            if (pivot < 0) {
                printf("rip\n");
                break;
            }

            continue;
        }

        /*printf("- cube: %d %d\n", cube_stack[pivot]->edge_orientations, cube_stack[pivot]->corner_orientations);*/
        coord_apply_move(cube_stack[pivot], move_stack[pivot]);
        /*printf("+ cube: %d %d\n", cube_stack[pivot]->edge_orientations, cube_stack[pivot]->corner_orientations);*/
        move_count++;

        if (move_count % 10000000 == 0 || 1) {
            sprintf(buffer, " moves: %4d pivot: %2d", move_count, pivot);
            sprintf(buffer, "%s : %7d %7d -> ", buffer, cube_stack[pivot]->edge_orientations,
                    cube_stack[pivot]->corner_orientations);
            for (int i = 0; i <= pivot; i++)
                sprintf(buffer, "%s %s", buffer, move_to_str(move_stack[i]));
            printf("%s\n", buffer);
        }

        if (cube_stack[pivot]->edge_orientations + cube_stack[pivot]->corner_orientations == 0)
            break;

        if (pivot < max_moves - 1) {
            copy_coord_cube(cube_stack[pivot + 1], cube_stack[pivot]);
            pivot++;
        }
    } while (1);

    /*buffer[0] = '\0';*/
    /*for (int i = 0; i <= pivot; i++)*/
    /*sprintf(buffer, "%s %d", buffer,move_stack[pivot]);*/
    /*sprintf(buffer, "%s %s", buffer, move_to_str(move_stack[pivot]));*/
    /*printf("%s\n", buffer);*/

    return 0;
}
