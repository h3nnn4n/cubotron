#include "config.h"

static config_t config = {0};

void init_config() {
    config.do_benchmark   = 0;
    config.do_solve       = 0;
    config.rebuild_tables = 0;
    config.max_depth      = 25;
    config.n_solutions    = 1;
    config.timeout        = 1;
}

config_t *get_config() { return &config; }
