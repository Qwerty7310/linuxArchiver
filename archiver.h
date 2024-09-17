#ifndef ARCHIVER
#define ARCHIVER

#include <stdio.h>

#define BUF_SIZE 1024

struct FileInfo {
    unsigned int d_size;
    char d_path[BUF_SIZE];
    char d_name[BUF_SIZE];
};

FILE *archive(char *path_src, char *path_res);
int archiveDir(char *root_path, char *path, FILE *f_info, FILE *f_content, int *count);
int addFileInfo(char *root_path, char *path, char *name, FILE *f_info);
int addFileContent(char *full_path, FILE *f_content);
int concatInfoAndContent(FILE *f_info, FILE *f_content, FILE *f_res);
int getFileSizeInBytes(FILE *file);

#endif