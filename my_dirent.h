#ifndef MY_DIRENT
#define MY_DERENT

#include <stdint.h>
#include <stdio.h>

#define BUF_SIZE 1024

typedef struct {
    int fd;
    char buffer[BUF_SIZE];
    int buf_pos;
    int buf_end;
} DIR;

struct dirent {
    uint64_t d_ino;
    int64_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};

DIR *openDir(char *name);
int closeDir(DIR *dir);
struct dirent *readDir(DIR *dir);
int getFileSizeInBytes(FILE *file);

#endif