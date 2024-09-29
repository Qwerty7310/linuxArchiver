#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdio.h>

FILE *archive(char *path_src, char *path_res);
int archiveDir(char *root_path, char *path, FILE *f_dir_info, FILE *f_info, FILE *f_content, int *cnt_dir,
               int *cnt_file);
int addFileInfo(char *root_path, char *path, char *name, FILE *f_info);
int addFileContent(char *full_path, FILE *f_content);
int concatInfoAndContent(FILE *f_dir_info, FILE *f_info, FILE *f_content, FILE *f_res, int cnt_dir,
                         int cnt_file);

#endif