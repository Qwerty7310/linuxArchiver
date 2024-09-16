#ifndef ARCHIVER
#define ARCHIVER

#include <stdio.h>

#define BUF_SIZE 1024

struct file_info {
    unsigned int d_size;
    char d_path[BUF_SIZE];
    char d_name[BUF_SIZE];
};

FILE *archive(char *path);
int archiveDir(char *path, FILE *f_info, FILE *f_content, int *count);
int addFileInfo(char *path, char *name, FILE *f_info);
int addFileContent(char *full_path, FILE *f_content);
int getFileSizeInBytes(FILE *file);

#endif