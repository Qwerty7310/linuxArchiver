#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define OK 0
#define ERROR 1

int printDir(char *path);

int main() {
    char *path1 = "/home/qwerty/OS/test1/";
    char *path2 = "../../test1/";

    if (printDir(path2) == ERROR) {
        printf("Error\n");
    }

    return 0;
}

int printDir(char *path) {
    DIR *directory = opendir(path);
    if (directory == NULL) {
        return ERROR;
    }
    struct dirent *dir;
    while ((dir = readdir(directory)) != NULL) {
        if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
            struct stat stat_buf;
            char *cur_path = (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1);  // for "\0" and "/"
            strcpy(cur_path, path);
            strcat(cur_path, dir->d_name);
            if (stat(cur_path, &stat_buf) != 0) {
                printf("Error. Could not get information about the file\n");
                return ERROR;
            }
            if (S_ISDIR(stat_buf.st_mode)) {
                strcat(cur_path, "/");
                if (printDir(cur_path)) {
                    return ERROR;
                }
            } else {
                printf("%s\n", dir->d_name);
            }
            free(cur_path);
        }
    }

    return OK;
}