#ifndef __CONFIG_H
#define __CONFIG_H

#include "definitions.h"

typedef struct {
    int do_benchmark;
    int do_solve;
    int rebuild_tables;
    int max_depth;
    int n_solutions;

    float timeout;

    // we only have 18 moves, so the black list cant evet be greater than 18 in length
    // (Assuming there are no repeats)
    move_t move_black_list[18];
} config_t;

void      init_config();
config_t *get_config();

#endif /* end of include guard */
