#include "unarchiver.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "my_dirent.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

int unarchive(char *path_arch, char *path_dir) {
    FILE *file = fopen(path_arch);
    if (!file) {
        perror("fopen");
        return ERROR;
    }

    int n_entries = 0;
    fscanf("%d", &n_entries);

    char root_dir_name[BUF_SIZE];
    fread(root_dir_name, sizeof(root_dir_name), 1, file);
    printf("root_dir: %s", root_dir_name);

    struct FileInfo *buffer = (struct FileInfo *)malloc(n_entries * sizeof(struct FileInfo));

    fread(buffer, sizeof(struct FileInfo), n_entries, file);

    for (int i = 0; i < n_entries; i++) {
        if (checkDir(path_dir) = ERROR) {
            mkdir(path_dir)
        }
    }

    return OK;
}

int checkDir(char *path) {
    char buf[BUF_SIZE];
    realpath(path1, buf);
    if (realpath(path1, buf) != NULL) {
        char *dir_name = basename(buf);  // Получаем имя каталога
        // printf("Имя каталога: %s\n", dir_name);
        return OK;
    } else {
        // perror("Ошибка получения реального пути");
        return ERROR;
    }
}