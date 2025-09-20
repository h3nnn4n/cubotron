/*
 * Copyright <2021> <Renan S Silva, aka h3nnn4n>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stddef.h> // Has to come before entropy, because they forgot to add it

#include <assert.h>
#include <entropy.h>
#include <pcg_variants.h>
#include <stdint.h>
#include <stdio.h>

#include "benchmark.h"
#include "cubie_cube.h"
#include "facelets.h"
#include "move_tables.h"
#include "pruning.h"
#include "sample_facelets.h"
#include "solve.h"
#include "utils.h"

#define n_scramble_moves 50
#define n_moves_chunk    1000

void apply_random_scramble(coord_cube_t *cube, int n_moves) {
    assert(cube != NULL);
    assert(n_moves <= n_scramble_moves);

    move_t moves[n_scramble_moves] = {0};
    for (int i = 0; i < n_moves; i++) {
        moves[i] = pcg32_boundedrand(N_MOVES);

        do {
            moves[i] = pcg32_boundedrand(N_MOVES);
        } while (i > 0 && (moves[i] == moves[i - 1] || is_bad_move(moves[i], moves[i - 1])));
    }

    for (int i = 0; i < n_moves; i++) {
        coord_apply_move(cube, moves[i]);
        /*printf(" %s", move_to_str(moves[i]));*/
    }
    /*printf(" : %4d %4d %3d %4d %5d %4d\n", cube->edge_orientations, cube->corner_orientations, cube->E_slice,*/
    /*cube->E_sorted_slice, cube->UD6_edge_permutations, cube->corner_permutations);*/

    assert(!is_phase1_solved(cube));
    assert(!is_phase2_solved(cube));
}

void do_solve(const coord_cube_t *cube) {
    solve_list_t *solution = solve_single(cube);
    /*solve_list_t *solution = solve(cube, 22, 0, 1);*/
    /*static int    solve_count = 0;*/

    if (solution == NULL) {
        printf("\nWarning: failed to solve!\n");
    } else {
        /*solve_count++;*/
        /*printf("%3d solution: ", solve_count);*/
        /*int len = 0;*/
        /*for (int i = 0; solution->solution[i] != MOVE_NULL; i++, len++) {*/
        /*printf(" %s", move_to_str(solution->solution[i]));*/
        /*coord_apply_move(cube, solution->solution[i]);*/
        /*}*/
        /*printf("  length: %2d\n", len);*/

        free(solution->next);
        free(solution->phase1_solution);
        free(solution->phase2_solution);
        free(solution->solution);
        free(solution);
    }

    /*assert(is_phase1_solved(cube));*/
    /*assert(is_phase2_solved(cube));*/
}

void solve_random_cubes() {
    printf("BENCHMARK: Solve random cubes\n");

    uint64_t seeds[2];
    entropy_getbytes((void *)seeds, sizeof(seeds));
    pcg32_srandom(seeds[0], seeds[1]);

    coord_cube_t *cube = get_coord_cube();

    int      solve_count = 0;
    uint64_t start_time  = get_microseconds();

    while (1) {
        int n = pcg32_boundedrand(n_scramble_moves / 2) + n_scramble_moves / 2;
        apply_random_scramble(cube, n);
        do_solve(cube);
        /*printf("\n");*/
        solve_count += 1;

        if (get_microseconds() - start_time > 5000000.0)
            break;
    }

    uint64_t end_time = get_microseconds();
    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("solve_count: %d - ", solve_count);
    printf("solves per second : %.2f\n", ((float)solve_count / (end_time - start_time)) * 1000000.0);
    printf("\n");

    free(cube);
}

void solve_cube_sample_library() {
    printf("BENCHMARK: Solve 100 sample cubes\n");
    int      solve_count = 0;
    uint64_t start_time  = get_microseconds();

    for (int i = 0; i < N_FACELETS_SAMPLES; i++) {
        cube_cubie_t *cubie_cube = build_cubie_cube_from_str(sample_facelets[i]);
        coord_cube_t *cube       = make_coord_cube(cubie_cube);

        do_solve(cube);
        /*printf("\n");*/
        solve_count += 1;

        free(cubie_cube);
        free(cube);
    }

    uint64_t end_time = get_microseconds();
    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("solve_count: %d - ", solve_count);
    printf("solves per second : %.2f\n", ((float)solve_count / (end_time - start_time)) * 1000000.0);
    printf("\n");
}

void coord_benchmark() {
    printf("BENCHMARK: Do coord moves in 1 second\n");

    coord_cube_t *cube = get_coord_cube();

    move_t moves[n_moves_chunk];

    for (int i = 0; i < n_moves_chunk; i++)
        moves[i] = pcg32_boundedrand(N_MOVES);

    int      move_count = 0;
    uint64_t start_time = get_microseconds();
    uint64_t end_time;

    do {
        for (int j = 0; j < 10000; j++) {
            for (int i = 0; i < n_moves_chunk; i++) {
                coord_apply_move(cube, moves[i]);
            }

            move_count += n_moves_chunk;
        }

        end_time = get_microseconds();
    } while (end_time - start_time < 1000000);

    printf("elapsed time: %f seconds - ", (float)(end_time - start_time) / 1000000.0);
    printf("moves: %d\n", move_count);
    printf("moves per second : %.2f\n", ((float)move_count / (end_time - start_time)) * 1000000.0);
    printf("\n");

    free(cube);
}
