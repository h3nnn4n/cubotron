#include <assert.h>
#include <stdio.h>
#include <time.h>

#include <pcg_variants.h>

#include "coord_cube.h"
#include "coord_move_tables.h"
#include "cubie_cube.h"
#include "cubie_move_table.h"

static int Cnk_cache[N_CORNERS * 4] = {0};

static char *_move_t_to_str_enum[] = {
    "MOVE_U1", "MOVE_U2", "MOVE_U3", "MOVE_R1", "MOVE_R2", "MOVE_R3", "MOVE_F1", "MOVE_F2", "MOVE_F3",   "MOVE_D1",
    "MOVE_D2", "MOVE_D3", "MOVE_L1", "MOVE_L2", "MOVE_L3", "MOVE_B1", "MOVE_B2", "MOVE_B3", "MOVE_NULL",
};

static char *_move_t_to_str[] = {
    "U ", "U2", "U'", "R ", "R2", "R'", "F ", "F2", "F'", "D ", "D2", "D'", "L ", "L2", "L'", "B ", "B2", "B'", "NULL",
};

static move_t reverse_move[] = {
    /* MOVE_U1 */ MOVE_U3,
    /* MOVE_U2 */ MOVE_U2,
    /* MOVE_U3 */ MOVE_U1,
    /* MOVE_R1 */ MOVE_R3,
    /* MOVE_R2 */ MOVE_R2,
    /* MOVE_R3 */ MOVE_R1,
    /* MOVE_F1 */ MOVE_F3,
    /* MOVE_F2 */ MOVE_F2,
    /* MOVE_F3 */ MOVE_F1,
    /* MOVE_D1 */ MOVE_D3,
    /* MOVE_D2 */ MOVE_D2,
    /* MOVE_D3 */ MOVE_D1,
    /* MOVE_L1 */ MOVE_L3,
    /* MOVE_L2 */ MOVE_L2,
    /* MOVE_L3 */ MOVE_L1,
    /* MOVE_B1 */ MOVE_B2,
    /* MOVE_B2 */ MOVE_B2,
    /* MOVE_B3 */ MOVE_B1,
};

int cubie_off_count(cube_cubie_t *cube) {
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

int is_solved(cube_cubie_t *cube) { return cubie_off_count(cube) == 0; }

char *move_to_str_enum(move_t move) {
    assert(move >= 0);
    assert(move <= N_MOVES);

    return _move_t_to_str_enum[move];
}

char *move_to_str(move_t move) {
    assert(move >= 0);
    assert(move <= 18);

    return _move_t_to_str[move];
}

int is_valid(cube_cubie_t *cube) {
    int edge_count[12]     = {0};
    int corner_count[8]    = {0};
    int edge_orientation   = 0;
    int corner_orientation = 0;

    for (int i = 0; i < 12; i++) {
        edge_count[cube->edge_permutations[i]]++;
        edge_orientation += cube->edge_orientations[i];
    }

    for (int i = 0; i < 12; i++)
        if (edge_count[i] != 1)
            return 0;

    if (edge_orientation % 2 != 0)
        return 0;

    for (int i = 0; i < 8; i++) {
        corner_count[cube->corner_permutations[i]]++;
        corner_orientation += cube->corner_orientations[i];
    }

    for (int i = 0; i < 8; i++)
        if (corner_count[i] != 1)
            return 0;

    if (corner_orientation % 3 != 0)
        return 0;

    return 1;
}

cube_cubie_t *random_cubie_cube() {
    cube_cubie_t *cube = init_cubie_cube();

    for (int i = 0; i < 40; i++) {
        move_t move = pcg32_boundedrand(N_MOVES);
        cubie_apply_move(cube, move);
    }

    return cube;
}

coord_cube_t *random_coord_cube() {
    coord_cube_t *cube = get_coord_cube();

    for (int i = 0; i < 40; i++) {
        move_t move = pcg32_boundedrand(N_MOVES);
        coord_apply_move(cube, move);
    }

    return cube;
}

move_t get_reverse_move(move_t move) {
    assert(move >= 0);
    assert(move < N_MOVES);

    return reverse_move[move];
}

long get_microseconds(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (long)(ts.tv_sec * 1000000000L + ts.tv_nsec) / 1000;
}

void coord_benchmark() {
    coord_cube_t *cube = get_coord_cube();

    move_t moves[1000];

    for (int i = 0; i < 1000; i++)
        moves[i] = pcg32_boundedrand(N_MOVES);

    int  move_count = 0;
    long start      = get_microseconds();
    long end;

    do {
        for (int j = 0; j < 1000; j++) {
            for (int i = 0; i < 1000; i++) {
                coord_apply_move(cube, moves[i]);
                move_count++;
            }
        }
        end = get_microseconds();
    } while (end - start < 1000000);

    printf("elapsed time: %ld microseconds\n", end - start);
    printf("moves: %d\n", move_count);
    printf("moves / second : %.2f\n", ((float)move_count / (end - start)) * 1000000.0);
}

int Cnk(int n, int k) {
    int i, j, s;

    if (n < k)
        return 0;

    int index = n * 4 + k;

    if (Cnk_cache[index] > 0)
        return Cnk_cache[index];

    if (k > n / 2)
        k = n - k;

    for (s = 1, i = n, j = 1; i != n - k; i--, j++) {
        s *= i;
        s /= j;
    }

    Cnk_cache[index] = s;

    return s;
}
