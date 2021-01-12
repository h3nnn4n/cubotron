#ifndef _PRUNING_CACHE
#define _PRUNING_CACHE

int  pruning_table_cache_load(char *cache_name, char *table_name, int **pruning_table, int table_size);
void pruning_table_cache_store(char *cache_name, char *table_name, int *pruning_table, int table_size);
int  file_exists(char *filepath);

#endif /* end of include guard */
