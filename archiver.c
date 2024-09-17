#include "archiver.h"

#include <fcntl.h>
#include <libgen.h>  // for basename()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "my_dirent.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

#define FILE_INFO_NAME "file_info.temp"
#define FILE_CONTENT_NAME "file_content_name.temp"

int getDirName(char *path, char *name);
int compare(const void *a, const void *b);

FILE *archive(char *path_src, char *path_res) {
    FILE *f_info = fopen(FILE_INFO_NAME, "w");
    FILE *f_content = fopen(FILE_CONTENT_NAME, "w");

    int count = 0;
    archiveDir(path_src, path_src, f_info, f_content, &count);
    fclose(f_info);
    fclose(f_content);

    f_info = fopen(FILE_INFO_NAME, "r");
    f_content = fopen(FILE_CONTENT_NAME, "r");
    FILE *f_res = fopen(path_res, "w");
    fprintf(f_res, "%d", count);

    char root_dir_name[BUF_SIZE];
    getDirName(path_src, root_dir_name);

    printf("root: %s %ld\n", root_dir_name, sizeof(root_dir_name));
    fwrite(root_dir_name, sizeof(root_dir_name), 1, f_res);

    concatInfoAndContent(f_info, f_content, f_res);
    remove(FILE_INFO_NAME);
    remove(FILE_CONTENT_NAME);

    return f_res;
}

int archiveDir(char *root_path, char *path, FILE *f_info, FILE *f_content, int *count) {
    DIR *directory = openDir(path);
    if (directory == NULL) return ERROR;

    struct dirent *dir = NULL;

    int cnt = 0;
    struct dirent **p_dir = (struct dirent **)malloc(10 * sizeof(struct dirent *));

    while ((dir = readDir(directory)) != NULL) {
        if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
            struct stat stat_buf;
            char *cur_path = (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1);  // for "\0" and "/"
            strcpy(cur_path, path);
            strcat(cur_path, dir->d_name);
            if (stat(cur_path, &stat_buf) != 0) {
                // closeDir(directory);
                free(cur_path);
                perror("stat");
                if (dir) free(dir);
                return ERROR;
            }
            p_dir[cnt] = dir;
            cnt += 1;
            // p_dir = realloc(p_dir, 10 * sizeof(struct dirent *));
            free(cur_path);
        }
        if (dir) free(dir);
    }

    // qsort(p_dir, cnt - 1, sizeof(struct dirent), compare);

    for (int i = 0; i < cnt - 1; i++) {
        struct stat stat_buf;
        char *cur_path = (char *)malloc(strlen(path) + strlen(p_dir[i]->d_name) + 1 + 1);  // for "\0" and "/"
        strcpy(cur_path, path);
        strcat(cur_path, dir->d_name);
        if (stat(cur_path, &stat_buf) != 0) {
            // closeDir(directory);
            free(cur_path);
            perror("stat");
            // if (dir) free(dir);
            return ERROR;
        }
        if (S_ISDIR(stat_buf.st_mode)) {
            strcat(cur_path, "/");
            if (archiveDir(root_path, cur_path, f_info, f_content, count) == ERROR) {
                free(cur_path);
                // closeDir(directory);
                // if (dir) free(dir);
                return ERROR;
            }
        } else {
            (*count) += 1;
            printf("%s\n", p_dir[i]->d_name);
            addFileInfo(root_path, path, p_dir[i]->d_name, f_info);
            addFileContent(cur_path, f_content);
        }
        free(cur_path);
    }

    // while ((dir = readDir(directory)) != NULL) {
    //     if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
    //         struct stat stat_buf;
    //         char *cur_path = (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1);  // for "\0" and
    //         "/" strcpy(cur_path, path); strcat(cur_path, dir->d_name); if (stat(cur_path, &stat_buf) != 0)
    //         {
    //             closeDir(directory);
    //             free(cur_path);
    //             perror("stat");
    //             if (dir) free(dir);
    //             return ERROR;
    //         }
    //         if (S_ISDIR(stat_buf.st_mode)) {
    //             strcat(cur_path, "/");
    //             if (archiveDir(root_path, cur_path, f_info, f_content, count) == ERROR) {
    //                 free(cur_path);
    //                 closeDir(directory);
    //                 if (dir) free(dir);
    //                 return ERROR;
    //             }
    //         } else {
    //             (*count) += 1;
    //             printf("%s\n", dir->d_name);
    //             addFileInfo(root_path, path, dir->d_name, f_info);
    //             addFileContent(cur_path, f_content);
    //         }
    //         free(cur_path);
    //     }
    //     if (dir) free(dir);
    // }
    closeDir(directory);
    return OK;
}

int addFileInfo(char *root_path, char *path, char *name, FILE *f_info) {
    char *full_path = (char *)malloc(strlen(path) + strlen(path) + 1);
    strcpy(full_path, path);
    strcat(full_path, name);

    char relative_path[BUF_SIZE];
    strcpy(relative_path, "./");
    strcat(relative_path, path + strlen(root_path));

    FILE *f_src = fopen(full_path, "r");
    int f_size = getFileSizeInBytes(f_src);

    struct FileInfo info;
    info.d_size = f_size;
    strcpy(info.d_name, name);
    strcpy(info.d_path, relative_path);

    fseek(f_info, 0, SEEK_END);
    fwrite(&info, sizeof(struct FileInfo), 1, f_info);

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

int concatInfoAndContent(FILE *f_info, FILE *f_content, FILE *f_res) {
    int f_info_size = getFileSizeInBytes(f_info);
    int f_content_size = getFileSizeInBytes(f_content);

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

int getDirName(char *path, char *name) {
    char buffer[BUF_SIZE];
    realpath(path, buffer);
    if (realpath(path, buffer) != NULL) {
        char *dir_name = basename(buffer);  // Получаем имя каталога
        strcpy(name, dir_name);
        return OK;
    } else
        return ERROR;
}

int getFileSizeInBytes(FILE *file) {
    int size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    return size;
}

int compare(const void *a, const void *b) {
    struct dirent *entry_a = (struct dirent *)a;
    struct dirent *entry_b = (struct dirent *)b;

    struct stat stat_buf_a, stat_buf_b;

    stat(entry_a->d_name, &stat_buf_a);
    stat(entry_b->d_name, &stat_buf_b);

    int is_dir_a = S_ISDIR(stat_buf_a.st_mode);
    int is_dir_b = S_ISDIR(stat_buf_b.st_mode);

    if (is_dir_a != is_dir_b) {
        return is_dir_a - is_dir_b; // Возвращает -1, если entryA — файл, а entryB — директория
    }

    return 0;
}
