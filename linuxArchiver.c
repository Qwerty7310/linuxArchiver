#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>

#include <string.h>
#include <sys/stat.h>
// #include <sys/syscall.h>

#include "my_dirent.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

struct linux_dirent {
    uint64_t d_ino;
    int64_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

struct file_info {
    uint64_t d_off;
    unsigned int d_reclen;
    unsigned int d_size;
    char *d_path;
    char *d_name;
};

// int printDir(char *path);
// int openDir(char *path);
int printDir(char *path);

int printFileName(char *name, char *path);
// int readDir(char *path);
int addFileToArchive(FILE *dest, char *name, char *path);
int addFileInfo(FILE *dest, FILE *src, char *name, char *path);

int main() {
    char *path1 = "/home/qwerty/OS/test1/";
    char *path2 = "../../test1/";
    char *path3 = "../../dferfe/";

    // DIR *directory = openDir(path1);
    // for (int i = 0; i < 10; i++) {
    //     readDir(directory);
    // }

    printDir(path2);

    return 0;
}

// int printDir(char *path) {
//     int directory = openDir(path);
//     if (directory == -1) {
//         return ERROR;
//     }
//     readDir(directory, path, &printFileName);
//     close(directory);
//     return OK;
// }

// int printFileName(char *name, char *path) {
//     printf("%s %s\n", path, name);
//     return 0;
// }

// int archiving(char *path) {

// }

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

// int addFileInfo(FILE *dest, FILE *src, char *name, char *path) {
//     fseek(src, 0, SEEK_END);
//     int file_size = ftell(src);
//     fseek(src, 0, SEEK_SET);

//     struct file_info FileInfo;
//     FileInfo.d_size = file_size;
//     FileInfo.d_name = (char *)malloc(strlen(name) + 1);
//     strcpy(FileInfo.d_name, name);

//     FileInfo.d_path = (char *)malloc(strlen(path) + 1);
//     strcpy(FileInfo.d_path, path);

//     FileInfo.d_reclen = sizeof(FileInfo);

//     fseek(dest, 0, SEEK_END);
//     fwrite(&FileInfo, sizeof(FileInfo), 1, dest);

//     return 0;
// }

int printDir(char *path) {
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
                if (printDir(cur_path)) {
                    free(cur_path);
                    closeDir(directory);
                    if (dir) free(dir);
                    return ERROR;
                }
            } else
                printf("%s\n", dir->d_name);
            free(cur_path);
        }
        if (dir) free(dir);
    }
    closeDir(directory);
    return OK;
}