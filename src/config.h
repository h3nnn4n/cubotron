#ifndef __CONFIG_H
#define __CONFIG_H

typedef struct {
    int do_benchmark;
    int do_solve;
    int rebuild_tables;
    int max_depth;
    int n_solutions;

    float timeout;
} config_t;

void       init_config();
config_t *get_config();

#endif /* end of include guard */
