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
    FILE *arch = fopen(path_arch, "r");
    if (!arch) {
        perror("open archive");
        return ERROR;
    }
    int cnt_dir;
    char root_dir_name[BUF_SIZE];
    fread(root_dir_name, sizeof(root_dir_name), 1, arch);
    fscanf(arch, "%d", &cnt_dir);

    puts(path_dir);
    puts(root_dir_name);

    char *new_path = (char *)malloc(strlen(path_dir) + strlen(root_dir_name) + 1);
    strcpy(new_path, path_dir);
    strcat(new_path, root_dir_name);
    puts(new_path);

    if (checkDir(path_dir) == OK) {
        // puts(new_path);
        // puts("\n");
        if (checkDir(new_path) == OK) {
            perror("such a directory already exists");
            free(new_path);
            return ERROR;
        } else {
            mkdir(new_path, 0700);
        }
    } else {
        perror("there is no such directory");
        free(new_path);
        return ERROR;
    }
    free(new_path);

    // printf("root_dir: %s, %d\n", root_dir_name, cnt_dir);

    for (int i = 0; i < cnt_dir; i++) {
        char buffer_dir[BUF_SIZE];
        fread(buffer_dir, BUF_SIZE, 1, arch);

        new_path = (char *)malloc(strlen(path_dir) + strlen(root_dir_name) + strlen(buffer_dir) + 1 + 1);
        strcpy(new_path, path_dir);
        strcat(new_path, root_dir_name);
        strcat(new_path, "/");
        strcat(new_path, buffer_dir);
        mkdir(new_path, 0700);
        // puts(new_path);
    }

    int cnt_file;
    fscanf(file, "%d", &cnt_file);

    // struct FileInfo *buffer_files = (struct FileInfo *)malloc(cnt_file * sizeof(struct FileInfo));
    // fread(buffer_files, sizeof(struct FileInfo), cnt_file, file);

    // // fread(buffer, sizeof(struct file_info), 1, f_info);

    // for (int i = 0; i < cnt_file; i++) {
    //     printf("path: %s\n", buffer_files[i].d_path);
    //     printf("name: %s\n", buffer_files[i].d_name);
    // }

    /*--------------------------------------------------------------------------------*/
    // FILE *file = fopen(path_arch);
    // if (!file) {
    //     perror("fopen");
    //     return ERROR;
    // }

    // int n_entries = 0;
    // fscanf("%d", &n_entries);

    // char root_dir_name[BUF_SIZE];
    // fread(root_dir_name, sizeof(root_dir_name), 1, file);
    // printf("root_dir: %s", root_dir_name);

    // struct FileInfo *buffer = (struct FileInfo *)malloc(n_entries * sizeof(struct FileInfo));

    // fread(buffer, sizeof(struct FileInfo), n_entries, file);

    // for (int i = 0; i < n_entries; i++) {
    //     if (checkDir(path_dir) = ERROR) {
    //         mkdir(path_dir)
    //     }
    // }

    return OK;
}

int checkDir(char *path) {
    char buf[BUF_SIZE];
    realpath(path, buf);
    if (realpath(path, buf) != NULL) {
        char *dir_name = basename(buf);  // Получаем имя каталога
        // printf("Имя каталога: %s\n", dir_name);
        return OK;
    } else {
        // perror("Ошибка получения реального пути");
        return ERROR;
    }
}