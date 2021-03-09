#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "benchmark.h"
#include "coord_cube.h"
#include "cubie_cube.h"
#include "facelets.h"
#include "move_tables.h"
#include "pruning.h"
#include "solve.h"
#include "stats.h"
#include "utils.h"

static int do_benchmark   = 0;
static int do_solve       = 0;
static int rebuild_tables = 0;
static int max_depth      = 25;
static int n_solutions    = 1;

static struct option long_options[] = {
    {"benchmarks", no_argument, &do_benchmark, 1}, {"rebuild-tables", no_argument, &rebuild_tables, 1},
    {"solve", required_argument, 0, 's'},          {"max-depth", required_argument, 0, 'm'},
    {"n-solutions", required_argument, 0, 'n'},    {0, 0, 0, 0}};

int main(int argc, char **argv) {
    char *facelets_to_solve = NULL;

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
                do_solve          = 1;
                facelets_to_solve = malloc(sizeof(char) * (strlen(optarg) + 2));
                memcpy(facelets_to_solve, optarg, sizeof(char) * (strlen(optarg) + 1));
            } break;

            case 'm': {
                max_depth = atoi(optarg);
            } break;

            case 'n': {
                n_solutions = atoi(optarg);
            } break;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default: abort();
        }
    }

    if (rebuild_tables) {
        rmrf("move_tables");
        rmrf("pruning_tables");
    }

    build_move_tables();
    build_pruning_tables();
    init_stats();

    if (do_benchmark) {
        solve_cube_sample_library();
        solve_random_cubes();
        coord_benchmark();
    }

    if (do_solve) {
        solve_list_t *solution = solve_facelets(facelets_to_solve, max_depth, 0, n_solutions);

        do {
            int length = 0;

            printf("solution:\n");
            for (int i = 0; solution->solution[i] != MOVE_NULL; i++, length++) {
                printf(" %s", move_to_str(solution->solution[i]));
            }
            printf("\n");
            printf("length: %d\n", length);

            solve_list_t *old_solution = solution;

            solution = solution->next;

            free(old_solution->phase1_solution);
            free(old_solution->phase2_solution);
            free(old_solution->solution);
            free(old_solution);
        } while (solution != NULL && solution->solution != NULL);

        free(solution);
        free(facelets_to_solve);
    }

    purge_cubie_move_table();

    dump_stats();

    return 0;
}
