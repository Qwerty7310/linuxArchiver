#include <fcntl.h>
#include <libgen.h>  // for basename()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "my_dirent.h"
#include "archiver.h"
#include "file_info.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

#define FILE_INFO_NAME "file_info.temp"
#define FILE_CONTENT_NAME "file_content_name.temp"
#define FILE_DIR_INFO_NAME "file_dir_info.temp"

int getDirName(char *path, char *name);
int compare(const void *a, const void *b);

int addDirInfo(char *root_path, char *path, FILE *f_dir_info);

FILE *archive(char *path_src, char *path_res) {
    FILE *f_dir_info = fopen(FILE_DIR_INFO_NAME, "w");
    FILE *f_info = fopen(FILE_INFO_NAME, "w");
    FILE *f_content = fopen(FILE_CONTENT_NAME, "w");

    int cnt_dir = 0;
    int cnt_file = 0;
    archiveDir(path_src, path_src, f_dir_info, f_info, f_content, &cnt_dir, &cnt_file);
    fclose(f_dir_info);
    fclose(f_info);
    fclose(f_content);

    f_dir_info = fopen(FILE_DIR_INFO_NAME, "r");
    f_info = fopen(FILE_INFO_NAME, "r");
    f_content = fopen(FILE_CONTENT_NAME, "r");

    FILE *f_res = fopen(path_res, "w");
    // fprintf(f_res, "%d", count);

    char root_dir_name[BUF_SIZE];
    getDirName(path_src, root_dir_name);

    // printf("root: %s %ld\n", root_dir_name, sizeof(root_dir_name));
    fwrite(root_dir_name, sizeof(root_dir_name), 1, f_res);

    concatInfoAndContent(f_dir_info, f_info, f_content, f_res, cnt_dir, cnt_file);
    remove(FILE_DIR_INFO_NAME);
    remove(FILE_INFO_NAME);
    remove(FILE_CONTENT_NAME);

    return f_res;
}

int archiveDir(char *root_path, char *path, FILE *f_dir_info, FILE *f_info, FILE *f_content, int *cnt_dir, int *cnt_file) {
    DIR *directory = openDir(path);
    if (directory == NULL) return ERROR;

    struct dirent *dir = NULL;

    /*
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
        // if (dir) free(dir);
        
    }

    for (int i = 0; i < cnt; i++) {
        printf("%d: %s\n", i, p_dir[i]->d_name);
    }

    qsort(p_dir, cnt, sizeof(struct dirent *), compare);
    */
   (*cnt_dir) += 1;
    addDirInfo(root_path, path, f_dir_info);

    while ((dir = readDir(directory)) != NULL) {
        if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
            struct stat stat_buf;
            char *cur_path = (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1);  // for '\0' and '/'
            strcpy(cur_path, path); strcat(cur_path, dir->d_name); if (stat(cur_path, &stat_buf) != 0)
            {
                closeDir(directory);
                free(cur_path);
                perror("stat");
                if (dir) free(dir);
                return ERROR;
            }
            if (S_ISDIR(stat_buf.st_mode)) {
                strcat(cur_path, "/");
                if (archiveDir(root_path, cur_path, f_dir_info, f_info, f_content, cnt_dir, cnt_file) == ERROR) {
                    free(cur_path);
                    closeDir(directory);
                    if (dir) free(dir);
                    return ERROR;
                }
            } else {
                (*cnt_file) += 1;
                printf("%s\n", dir->d_name);
                addFileInfo(root_path, path, dir->d_name, f_info);
                addFileContent(cur_path, f_content);
            }
            free(cur_path);
        }
        if (dir) free(dir);
    }
    closeDir(directory);
    return OK;
}

int addDirInfo(char *root_path, char *path, FILE *f_dir_info) {
    char relative_path[BUF_SIZE];
    // strcpy(relative_path, "./");

    if (strlen(path) > strlen(root_path)) {
        strcpy(relative_path, path + strlen(root_path));

        fseek(f_dir_info, 0, SEEK_END);
        fwrite(relative_path, sizeof(relative_path), 1, f_dir_info);
    }
    

    return 0;
}

int addFileInfo(char *root_path, char *path, char *name, FILE *f_info) {
    char *full_path = (char *)malloc(strlen(path) + strlen(path) + 1);
    strcpy(full_path, path);
    strcat(full_path, name);

    char relative_path[BUF_SIZE];
    // strcpy(relative_path, "./");
    strcpy(relative_path, path + strlen(root_path));

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

// int concatInfoAndContent(FILE *f_dir_info, FILE *f_info, FILE *f_content, FILE *f_res) {
int concatInfoAndContent(FILE *f_dir_info, FILE *f_info, FILE *f_content, FILE *f_res, int cnt_dir, int cnt_file) {
    int f_dir_info_size = getFileSizeInBytes(f_dir_info);
    int f_info_size = getFileSizeInBytes(f_info);
    int f_content_size = getFileSizeInBytes(f_content);

    fprintf(f_res, "%d", cnt_dir - 1);

    char c;
    for (int i = 0; i < f_dir_info_size; i++) {
        c = fgetc(f_dir_info);
        fputc(c, f_res);
    }
    fprintf(f_res, "%d", cnt_file);
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

// int compare(const void *a, const void *b) {
//     struct dirent *entry_a = *(struct dirent **)a;
//     struct dirent *entry_b = *(struct dirent **)b;

//     struct stat stat_buf_a, stat_buf_b;

//     stat(entry_a->d_name, &stat_buf_a);
//     stat(entry_b->d_name, &stat_buf_b);

//     int is_dir_a = S_ISDIR(stat_buf_a.st_mode);
//     int is_dir_b = S_ISDIR(stat_buf_b.st_mode);
//     printf("a: %s, %d; b: %s, %d\n", entry_a->d_name, is_dir_a, entry_b->d_name, is_dir_b);

//     if (is_dir_a != is_dir_b) {
//         return is_dir_a - is_dir_b; // Возвращает -1, если entryA — файл, а entryB — директория
//     }

//     return 0;
// }
