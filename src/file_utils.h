#ifndef _FILE_UTILS
#define _FILE_UTILS

void ensure_directory_exists(char *directory);
int  file_exists(char *filepath);
int  mkdir_p(const char *path);

#endif
