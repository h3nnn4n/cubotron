#include "coord_move_tables.h"
#include "cubie_move_table.h"

void build_move_tables() {
    cubie_build_move_table();
    coord_build_move_tables();
}
