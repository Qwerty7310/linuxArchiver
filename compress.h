#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdio.h>

typedef struct {
    unsigned char offset;
    unsigned char length;  // or char, if offset = 0
} Token;

Token *findLongestMatch(char *str, int str_size, int buf_size);
FILE *deflate(FILE *file, char *path);
FILE *inflate(FILE *file, char *path);

#endif