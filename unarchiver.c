#include "unarchiver.h"

#include <fcntl.h>
#include <libgen.h>  // for basename()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "file_info.h"
#include "my_dirent.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

int unarchive(char *path_arch, char *path_dir) {
    FILE *arch = fopen(path_arch, "r");  // файл архива
    if (!arch) {
        perror("open archive");
        return ERROR;
    }
    int cnt_dir;                   // количество директорий
    char root_dir_name[BUF_SIZE];  // имя корневой директории архива
    fread(root_dir_name, sizeof(root_dir_name), 1, arch);
    fscanf(arch, "%d", &cnt_dir);

    char *new_path =
        (char *)malloc(strlen(path_dir) + strlen(root_dir_name) + 1);  // путь до директории вместе с именем
    strcpy(new_path, path_dir);
    strcat(new_path, root_dir_name);

    if (checkDir(path_dir) == OK) {  // проверка на наличие
        if (checkDir(new_path) == OK) {
            perror("such a directory already exists");
            free(new_path);
            return ERROR;
        } else
            mkdir(new_path, 0700);  // создание

    } else {
        perror("there is no such directory");
        free(new_path);
        return ERROR;
    }
    free(new_path);

    for (int i = 0; i < cnt_dir; i++) {
        char buffer_dir[BUF_SIZE];
        fread(buffer_dir, BUF_SIZE, 1, arch);
        new_path = (char *)malloc(strlen(path_dir) + strlen(root_dir_name) + strlen(buffer_dir) + 1 +
                                  1);  // for '/' and '\0'
        strcpy(new_path, path_dir);
        strcat(new_path, root_dir_name);
        strcat(new_path, "/");
        strcat(new_path, buffer_dir);
        mkdir(new_path, 0700);
        free(new_path);
    }

    int cnt_file;
    fscanf(arch, "%d", &cnt_file);

    struct FileInfo *file_info_buf = (struct FileInfo *)malloc(cnt_file * sizeof(struct FileInfo));
    fread(file_info_buf, sizeof(struct FileInfo), cnt_file, arch);

    for (int i = 0; i < cnt_file; i++) {
        char *full_path =
            (char *)malloc(strlen(path_dir) + strlen(root_dir_name) + 1 + strlen(file_info_buf[i].d_path) +
                           strlen(file_info_buf[i].d_name) + 1);  // for '/' and '\0'
        strcpy(full_path, path_dir);
        strcat(full_path, root_dir_name);
        strcat(full_path, "/");
        strcat(full_path, file_info_buf[i].d_path);
        strcat(full_path, file_info_buf[i].d_name);

        FILE *file = fopen(full_path, "w");
        for (int j = 0; j < file_info_buf[i].d_size; j++) {
            char c = fgetc(arch);
            fputc(c, file);
        }
        fclose(file);
        free(full_path);
    }
    free(file_info_buf);
    fclose(arch);
    return OK;
}

int checkDir(char *path) {
    char buf[BUF_SIZE];
    realpath(path, buf);
    if (realpath(path, buf) != NULL) {
        char *dir_name = basename(buf);  // Get directory name
        return OK;
    } else
        return ERROR;
}