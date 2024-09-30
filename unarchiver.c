#include "unarchiver.h"

#include <fcntl.h>
#include <libgen.h>  // for basename()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "compress.h"
#include "file_info.h"
#include "my_dirent.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

#define UNZIP_FILE_NAME "./unzip_arch.tmp"

int unarchive(char *path_arch, char *path_dir) {
    FILE *zip_arch = fopen(path_arch, "r");  // файл архива
    if (!zip_arch) {
        perror("open archive");
        return ERROR;
    }

    FILE *arch = inflate(zip_arch, UNZIP_FILE_NAME);
    if (!arch) return ERROR;

    fclose(arch);
    arch = fopen(UNZIP_FILE_NAME, "r");

    int cnt_dir;                   // количество директорий
    char root_dir_name[BUF_SIZE];  // имя корневой директории архива
    if (fread(root_dir_name, sizeof(root_dir_name), 1, arch) < 1) {
        perror("fread");
        return ERROR;
    }
    if (fscanf(arch, "%d", &cnt_dir) < 1) {
        perror("fscanf");
        return ERROR;
    }

    char *new_path =
        (char *)malloc(strlen(path_dir) + strlen(root_dir_name) + 1);  // путь до директории вместе с именем
    if (!new_path) {
        perror("malloc");
        return ERROR;
    }
    strcpy(new_path, path_dir);
    strcat(new_path, root_dir_name);

    if (checkDir(path_dir) == OK) {  // проверка на наличие
        if (checkDir(new_path) == OK) {
            perror("such a directory already exists");
            free(new_path);
            return ERROR;
        } else if (mkdir(new_path, 0700) == -1) {  // создание
            perror("mkdir");
            return ERROR;
        }

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
        if (mkdir(new_path, 0700) == -1) {
            perror("mkdir");
            return ERROR;
        }
        free(new_path);
    }

    int cnt_file;
    if (fscanf(arch, "%d", &cnt_file) < 1) {
        perror("fscanf");
        return ERROR;
    }

    struct FileInfo *file_info_buf = (struct FileInfo *)malloc(cnt_file * sizeof(struct FileInfo));
    if ((int)fread(file_info_buf, sizeof(struct FileInfo), cnt_file, arch) < cnt_file) {
        perror("fread");
        return ERROR;
    }

    for (int i = 0; i < cnt_file; i++) {
        char *full_path =
            (char *)malloc(strlen(path_dir) + strlen(root_dir_name) + 1 + strlen(file_info_buf[i].d_path) +
                           strlen(file_info_buf[i].d_name) + 1);  // for '/' and '\0'
        if (!full_path) {
            perror("malloc");
            return ERROR;
        }
        strcpy(full_path, path_dir);
        strcat(full_path, root_dir_name);
        strcat(full_path, "/");
        strcat(full_path, file_info_buf[i].d_path);
        strcat(full_path, file_info_buf[i].d_name);

        FILE *file = fopen(full_path, "w");
        if (!file) {
            perror(full_path);
            return ERROR;
        }
        for (int j = 0; j < (int)file_info_buf[i].d_size; j++) {
            char c = fgetc(arch);
            fputc(c, file);
        }
        fclose(file);
        free(full_path);
    }
    free(file_info_buf);
    fclose(arch);
    remove(UNZIP_FILE_NAME);

    return OK;
}

int checkDir(char *path) {
    char buf[BUF_SIZE];
    realpath(path, buf);
    // char *dir_name = basename(buf);  для получения имени директории
    if (realpath(path, buf) != NULL)
        return OK;
    else
        return ERROR;
}