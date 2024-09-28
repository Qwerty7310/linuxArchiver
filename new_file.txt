#ifndef UNARCHIVER
#define UNARCHIVER

#include <stdio.h>

// #define BUF_SIZE 1024

// struct FileInfo {
//     unsigned int d_size;
//     char d_path[BUF_SIZE];
//     char d_name[BUF_SIZE];
// };

int unarchive(char *path_arch, char *path_dir);
int checkDir(char *path);

#endif