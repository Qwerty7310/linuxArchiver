#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "archiver.h"
#include "my_dirent.h"


#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

#define FILE_INFO_NAME "file_info.temp"
#define FILE_CONTENT_NAME "file_content_name.temp"

FILE *archive(char *path_src, char *path_res) {
    FILE *f_info = fopen(FILE_INFO_NAME, "w");
    FILE *f_content = fopen(FILE_CONTENT_NAME, "w");

    int count = 0;
    archiveDir(path_src, f_info, f_content, &count);
    fclose(f_info);
    fclose(f_content);

    f_info = fopen(FILE_INFO_NAME, "r");
    f_content = fopen(FILE_CONTENT_NAME, "r");
    FILE *f_res = fopen(path_res, "w");
    concatInfoAndContent(f_info, f_content, f_res, count);
    remove(FILE_INFO_NAME);
    remove(FILE_CONTENT_NAME);

    return f_res;
}

int archiveDir(char *path, FILE *f_info, FILE *f_content, int *count) {
    DIR *directory = openDir(path);
    if (directory == NULL)
        return ERROR;
    
    struct dirent *dir = NULL;
    while ((dir = readDir(directory)) != NULL) {
        if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
            struct stat stat_buf;
            char *cur_path = (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1);  // for "\0" and "/"
            strcpy(cur_path, path);
            strcat(cur_path, dir->d_name);
            if (stat(cur_path, &stat_buf) != 0) {
                closeDir(directory);
                free(cur_path);
                perror("stat");
                if (dir) free(dir);
                return ERROR;
            }
            if (S_ISDIR(stat_buf.st_mode)) {
                strcat(cur_path, "/");
                if (archiveDir(cur_path, f_info, f_content, count) == ERROR) {
                    free(cur_path);
                    closeDir(directory);
                    if (dir) free(dir);
                    return ERROR;
                }
            } else {
                (*count) += 1;
                printf("%s\n", dir->d_name);
                addFileInfo(path, dir->d_name, f_info);
                addFileContent(cur_path, f_content);
            }
            free(cur_path);
        }
        if (dir) free(dir);
    }
    closeDir(directory);
    return OK;
}

int addFileInfo(char *path, char *name, FILE *f_info) {
    char *full_path = (char *)malloc(strlen(path) + strlen(path) + 1);
    strcpy(full_path, path);
    strcat(full_path, name);

    FILE *f_src = fopen(full_path, "r");
    int f_size = getFileSizeInBytes(f_src);

    struct file_info info;
    info.d_size = f_size;
    strcpy(info.d_name, name);
    strcpy(info.d_path, path);

    fseek(f_info, 0, SEEK_END);
    fwrite(&info, sizeof(struct file_info), 1, f_info);

    free(full_path);
    return 0;
}

int addFileContent(char *full_path, FILE *f_content) {
    FILE *f_src = fopen(full_path, "r");
    int f_size = getFileSizeInBytes(f_src);
    for (int i = 0; i < f_size; i++) {
        char c = fgetc(f_src);
        fputc(c, f_content);
    }

    // char c;
    // while ((c = fgetc(f_src)) != EOF) {
    //     fputc(c, f_content);
    // }

    fclose(f_src);
}

int concatInfoAndContent(FILE *f_info, FILE *f_content, FILE *f_res, int n) {
    int f_info_size = getFileSizeInBytes(f_info);
    int f_content_size = getFileSizeInBytes(f_content);
    fprintf(f_res, "%d", n);

    char c;
    for (int i = 0; i < f_info_size; i++) {
        c = fgetc(f_info);
        fputc(c, f_res);
    }
    for (int i = 0; i < f_content_size; i++) {
        c = fgetc(f_content);
        fputc(c, f_res);
    }

    return 0;
}

int getFileSizeInBytes(FILE *file) {
    int size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    return size;
}
