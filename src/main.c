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

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "benchmark.h"
#include "config.h"
#include "coord_cube.h"
#include "cubie_cube.h"
#include "facelets.h"
#include "mem_utils.h"
#include "move_tables.h"
#include "pruning.h"
#include "solve.h"
#include "stats.h"
#include "utils.h"

static config_t *config;

int main(int argc, char **argv) {
    char *facelets_to_solve = NULL;

    init_config();
    config = get_config();

    struct option long_options[] = {{"benchmarks", no_argument, &config->do_benchmark, 1},
                                    {"rebuild-tables", no_argument, &config->rebuild_tables, 1},
                                    {"solve", required_argument, 0, 's'},
                                    {"solve-scramble", optional_argument, 0, 'c'},
                                    {"max-depth", required_argument, 0, 'm'},
                                    {"n-solutions", required_argument, 0, 'n'},
                                    {"move-blacklist", required_argument, 0, 'b'},
                                    {0, 0, 0, 0}};

    while (1) {
        int c;
        int option_index = 0;

        c = getopt_long(argc, argv, "abc:d:f:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 0:
                if (long_options[option_index].flag != 0)
                    break;

                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf(" with arg %s", optarg);

                printf("\n");
                break;

            case 's': {
                if (optarg == NULL) {
                    fprintf(stderr, "optarg is missing for solve");
                    break;
                }

                config->do_solve  = 1;
                facelets_to_solve = malloc(sizeof(char) * (strlen(optarg) + 2));
                memcpy_(facelets_to_solve, optarg, sizeof(char) * (strlen(optarg) + 1));
            } break;

            case 'b': {
                if (optarg == NULL) {
                    fprintf(stderr, "optarg is missing for benchmark");
                    break;
                }

                char *move_black_list_str = malloc(sizeof(char) * (strlen(optarg) + 2));
                memcpy_(move_black_list_str, optarg, sizeof(char) * (strlen(optarg) + 1));

                for (size_t i = 0; i < strlen(move_black_list_str); i++) {
                    move_t move = str_to_move(&move_black_list_str[i]);

                    if (move == MOVE_NULL)
                        continue;

                    config->move_black_list[move] = move;
                }

                free(move_black_list_str);
            } break;

            case 'm': {
                if (optarg == NULL) {
                    fprintf(stderr, "optarg is missing for max_depth");
                    break;
                }

                config->max_depth = atoi(optarg);
            } break;

            case 'n': {
                if (optarg == NULL) {
                    fprintf(stderr, "optarg is missing for number of sulutions");
                    break;
                }

                config->n_solutions = atoi(optarg);
            } break;

            case 'c': {
                if (optarg == NULL) {
                    fprintf(stderr, "optarg is missing for scramble");
                    break;
                }

                config->do_solve       = 1;
                config->scramble_moves = move_sequence_str_to_moves(optarg);
            } break;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default: abort();
        }
    }

    if (config->rebuild_tables) {
        rmrf("move_tables");
        rmrf("pruning_tables");
    }

    build_move_tables();
    build_pruning_tables();
    init_stats();

    if (config->do_benchmark) {
        solve_cube_sample_library();
        solve_random_cubes();
        coord_benchmark();
    } else if (config->do_solve) {
        solve_list_t *solution = NULL;

        if (config->scramble_moves != NULL) {
            coord_cube_t *cube = get_coord_cube();
            printf("Scramble moves: ");
            for (int i = 0; config->scramble_moves[i] != MOVE_NULL; i++) {
                printf("%s ", move_to_str(config->scramble_moves[i]));
                coord_apply_move(cube, config->scramble_moves[i]);
            }
            printf("\n");

            solution = solve_single(cube);
        } else {
            solution = solve_facelets(facelets_to_solve, config);
        }

        // Print all solutions
        solve_list_t *current      = solution;
        int           solution_num = 1;
        while (current != NULL && current->solution != NULL) {
            int length = 0;
            for (int i = 0; current->solution[i] != MOVE_NULL; i++, length++) {
            }
            printf("Solution %2d (length %d): ", solution_num, length);
            print_move_sequence(current->solution);

            current = current->next;
            solution_num++;
        }

        if (solution != NULL) {
            print_solve_stats(solution->stats);
        }

        destroy_solve_list(solution);
        free(facelets_to_solve);
    } else {
        printf("No solve mode specified\n");
    }

    purge_cubie_move_table();

    dump_stats();

    return 0;
}
