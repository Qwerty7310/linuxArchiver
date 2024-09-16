#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <libgen.h>

// #include <unistd.h>

#include <string.h>
#include <sys/stat.h>
// #include <sys/syscall.h>

#include "my_dirent.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024
#define FILE_INFO_NAME "file_info.temp"
#define FILE_CONTENT_NAME "file_content_name.temp"

struct linux_dirent {
    uint64_t d_ino;
    int64_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

struct file_info {
    unsigned int d_size;
    char d_path[BUF_SIZE];
    char d_name[BUF_SIZE];
};

// int printDir(char *path);

int archiveDir(char *path, FILE *f_info, FILE *f_content, int *count);
FILE *archive(char *path);
int addFileInfo(char *path, char *name, FILE *f_info);

int readFileInfo(FILE *f_info);

int getFileSizeInBytes(FILE *file);

int main() {
    char *path1 = "/home/qwerty/OS/test1/";
    char *path2 = "../../test1/";
    char *path3 = "../../dferfe/";

    FILE *file = fopen("ARCH", "w");
    int cnt = 0;
    char buf[BUF_SIZE];

    /*
    puts(realpath(path1, buf));
    if (realpath(path1, buf) != NULL) {
        char *dir_name = basename(buf);  // Получаем имя каталога
        printf("Имя каталога: %s\n", dir_name);
    } else {
        perror("Ошибка получения реального пути");
    }
    */

    archiveDir(path1, file, NULL, &cnt);
    

    fclose(file);

    file = fopen("ARCH", "r");
    for (int i = 0; i < cnt; i++) {
        readFileInfo(file);
    }
    fclose(file);

    // DIR *directory = openDir(path1);
    // for (int i = 0; i < 10; i++) {
    //     readDir(directory);
    // }

    // printDir(path2);

    return 0;
}


// int addFileToArchive(FILE *dest, char *name, char *path) {
//     char *full_name = (char *)malloc(strlen(path) + strlen(name) + 1);
//     strcpy(full_name, path);
//     strcat(full_name, name);

//     FILE *src = fopen(full_name, "r");

//     addFileInfo(dest, src, name, path);
//     // addFileContent();

//     fclose(src);
//     free(full_name);
//     return 0;
// }

FILE *archive(char *path) {
    FILE *f_info = fopen(FILE_INFO_NAME, "w");
    FILE *f_content = fopen(FILE_CONTENT_NAME, "w");

    int count = 0;
    archiveDir(path, f_info, f_content, &count);

    FILE *f_res = fopen("./archive.arch", "w");

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
                // printf("%s\n", dir->d_name);
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
    while()
}

int readFileInfo(FILE *f_info) {
    struct file_info *buffer = (struct file_info *)malloc(sizeof(struct file_info));
    fread(buffer, sizeof(struct file_info), 1, f_info);

    printf("path: %s\n", buffer->d_path);
    printf("name: %s\n", buffer->d_name);

    return 0;
}

int getFileSizeInBytes(FILE *file) {
    int size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    return size;
}