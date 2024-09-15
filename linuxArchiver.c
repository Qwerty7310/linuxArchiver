#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

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

// int printDir(char *path);
int openDir(char *path);
int printDir(char *path);

int main() {
    char *path1 = "/home/qwerty/OS/test1/";
    char *path2 = "../../test1/";

    // if (printDir(path2) == ERROR) {
    //     printf("Error\n");
    // }

    if (printDir(path2) == -1) {
        perror("Error reading directory\n");
        return 0;
    }

    return 0;
}

int openDir(char *path) {
    int directory = open(path, O_RDONLY | O_DIRECTORY);
    return directory;
}

int printDir(char *path) {
    int nread;
    char buffer[BUF_SIZE];
    int directory = openDir(path);
    int res = OK;
    while ((nread = syscall(SYS_getdents64, directory, buffer, BUF_SIZE)) > 0) {
        for (int bpos = 0; bpos < nread;) {
            struct linux_dirent *dir = (struct linux_dirent *)(buffer + bpos);
            if (strcmp(".", dir->d_name) && strcmp("..", dir->d_name)) {
                struct stat stat_buf;
                char *cur_path = (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1); //for '/' and '\0'
                strcpy(cur_path, path);
                strcat(cur_path, dir->d_name);
                if (stat(cur_path, &stat_buf) != 0) {
                    perror("Error\n");
                    return ERROR;
                }
                if (S_ISDIR(stat_buf.st_mode)) {
                    strcat(cur_path, "/");
                    int res = printDir(cur_path);
                } else {
                    printf("%s\n", dir->d_name);
                }

                free(cur_path);
            }
            if (res == ERROR) {
                return ERROR;
            }

            bpos += dir->d_reclen;
        }
    }

    if (nread == -1) {
        return ERROR;
    }

    return OK;
}

// int printDir(char *path) {
//     DIR *directory = opendir(path);
//     if (directory == NULL) {
//         return ERROR;
//     }
//     struct dirent *dir;
//     while ((dir = readdir(directory)) != NULL) {
//         if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
//             struct stat stat_buf;
//             char *cur_path = (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1);  // for "\0" and
//             "/" strcpy(cur_path, path); strcat(cur_path, dir->d_name); if (stat(cur_path, &stat_buf) != 0)
//             {
//                 printf("Error. Could not get information about the file\n");
//                 return ERROR;
//             }
//             if (S_ISDIR(stat_buf.st_mode)) {
//                 strcat(cur_path, "/");
//                 if (printDir(cur_path)) {
//                     return ERROR;
//                 }
//             } else {
//                 printf("%s\n", dir->d_name);
//             }
//             free(cur_path);
//         }
//     }

//     return OK;
// }