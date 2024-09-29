#ifndef COMPRESS
#define COMPRESS

#include <stdio.h>

typedef struct {
    unsigned char offset;
    unsigned char length;  // or char, if offset = 0
} Token;

// int getFileSizeInBytes(FILE *file);
Token *findLongestMatch(char *str, int str_size, int buf_size);
FILE *deflate(FILE *file, char *path);
FILE *inflate(FILE *file, char *path);

#endif