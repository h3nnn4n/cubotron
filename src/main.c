#include "benchmark.h"
#include "move_tables.h"
#include "pruning.h"

int main() {
    build_move_tables();
    build_pruning_tables();

    solve_random_cubes();
    solve_cube_sample_library();
    coord_benchmark();

    purge_cubie_move_table();

    return 0;
}
