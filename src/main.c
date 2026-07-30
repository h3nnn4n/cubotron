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
#include <stdlib.h>
#include <string.h>

#include "benchmark.h"
#include "config.h"
#include "definitions.h"
#include "mem_utils.h"
#include "move_tables.h"
#include "pruning.h"
#include "puzzle.h"
#include "solution.h"
#include "solve.h"
#include "solver.h"
#include "stats.h"
#include "utils.h"

static config_t *config;

int main(int argc, char **argv) {
    char *facelets_to_solve = NULL;

    init_config();
    config = get_config();

    struct option long_options[] = {{"benchmark-fast", no_argument, &config->do_benchmark_fast, 1},
                                    {"benchmark-slow", no_argument, &config->do_benchmark_slow, 1},
                                    {"rebuild-tables", no_argument, &config->rebuild_tables, 1},
                                    {"solve", required_argument, 0, 's'},
                                    {"solve-scramble", required_argument, 0, 'c'},
                                    {"puzzle", required_argument, 0, 'p'},
                                    {"max-depth", required_argument, 0, 'm'},
                                    {"n-solutions", required_argument, 0, 'n'},
                                    {"move-blacklist", required_argument, 0, 'b'},
                                    {"compare-against", required_argument, 0, 'A'},
                                    {"compare-benchmarks", required_argument, 0, 'B'},
                                    {"list-puzzles", no_argument, 0, 1},
                                    {"list-solvers", no_argument, 0, 2},
                                    {"help", no_argument, 0, 'h'},
                                    {0, 0, 0, 0}};

    while (1) {
        int c;
        int option_index = 0;

        c = getopt_long(argc, argv, "abc:d:f:h", long_options, &option_index);

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

                if (config->max_depth > MAX_MOVES - 1) {
                    fprintf(stderr, "Error: max_depth must be <= %d\n", MAX_MOVES - 1);
                    return 1;
                }
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

            case 'A': {
                config->compare_against = strdup(optarg);
            } break;

            case 'B': {
                config->compare_benchmarks = strdup(optarg);
            } break;

            case 'p': {
                if (optarg == NULL) {
                    fprintf(stderr, "optarg is missing for puzzle");
                    break;
                }

                config->puzzle_type = strdup(optarg);
            } break;

            case 1: {
                init_registry();
                printf("Available puzzles:\n");
                for (int i = 0; i < puzzle_count(); i++)
                    printf("  %s\n", puzzle_by_index(i)->name);
                return 0;
            }

            case 2: {
                init_registry();
                printf("Available solvers:\n");
                for (int i = 0; i < solver_count(); i++)
                    printf("  %s (for %s)\n", solver_by_index(i)->name, solver_by_index(i)->puzzle_name);
                return 0;
            }

            case 'h': print_help(); return 0;

            default: abort();
        }
    }

    if (config->compare_benchmarks != NULL) {
        char *comma = strchr(config->compare_benchmarks, ',');

        if (comma == NULL) {
            fprintf(stderr, "Error: --compare-benchmarks expects two filenames separated by comma\n");
            return 1;
        }

        *comma            = '\0';
        const char *file1 = config->compare_benchmarks;
        const char *file2 = comma + 1;

        build_move_tables();
        build_pruning_tables();

        compare_benchmark_files(file1, file2);
        return 0;
    }

    if (!config->do_benchmark_fast && !config->do_benchmark_slow && !config->do_solve &&
        config->compare_benchmarks == NULL) {
        print_help();
        return 0;
    }

    if (config->rebuild_tables) {
        rmrf("move_tables");
        rmrf("pruning_tables");
    }

    build_move_tables();
    build_pruning_tables();
    if (config->do_benchmark_fast) {
        run_benchmark_fast();
    } else if (config->do_benchmark_slow) {
        run_benchmark_slow();
    } else if (config->do_solve) {
        solve_list_t *solution = NULL;

        if (config->scramble_moves != NULL) {
            puzzle_t *p = puzzle_create(config->puzzle_type);
            printf("Scramble moves: ");
            for (int i = 0; config->scramble_moves[i] != MOVE_NULL; i++) {
                printf("%s ", move_to_str(config->scramble_moves[i]));
                p->ops->apply_move(p->state, config->scramble_moves[i]);
            }
            printf("\n");

            char buf[1024];
            p->ops->to_string(p->state, buf, sizeof(buf));
            puzzle_destroy(p);

            solution = solve_puzzle(config->puzzle_type, buf, config);
        } else {
            solution = solve_puzzle(config->puzzle_type, facelets_to_solve, config);
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
            printf("\n");
            print_aggregate_stats(solution->aggregate, solution->stats);
        }

        destroy_solve_list(solution);
        free(facelets_to_solve);
    }

    purge_cubie_move_table();

    return 0;
}
