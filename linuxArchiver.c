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
#define ERROR_OPEN_DIR 2
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
int openDir(char *path);
int printDir(char *path);

int printFileName(char *name, char *path);
int readDir(int directory, char *path, int (*function)(char *name, char *path));

int main() {
    char *path1 = "/home/qwerty/OS/test1/";
    char *path2 = "../../test1/";
    char *path3 = "../../dferfe/";

    if (printDir(path1) == ERROR) {
        perror("Error reading directory");
        return 0;
    }

    return 0;
}

int openDir(char *path) {
    int directory = open(path, O_RDONLY | O_DIRECTORY);
    return directory;
}

int printDir(char *path) {
    int directory = openDir(path);
    if (directory == -1) {
        return ERROR_OPEN_DIR;
    }
    readDir(directory, path, &printFileName);
    close(directory);
    return OK;
}

int readDir(int directory, char *path, int (*function)(char *name, char *path)) {
    int nread;
    char buffer[BUF_SIZE];
    int res = OK;
    while ((nread = syscall(SYS_getdents64, directory, buffer, BUF_SIZE)) > 0) {
        for (int bpos = 0; bpos < nread;) {
            struct linux_dirent *dir = (struct linux_dirent *)(buffer + bpos);
            if (strcmp(".", dir->d_name) && strcmp("..", dir->d_name)) {
                struct stat stat_buf;
                char *cur_path =
                    (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1);  // for '/' and '\0'
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
                    (*function)(dir->d_name, path);
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
}

int printFileName(char *name, char *path) {
    printf("%s %s\n", path, name);
    return 0;
}

int archiving(char *path) {

}

int addFileToArchive(FILE *dest, char *name, char *path) {
    char *full_name = (char *)malloc(strlen(path) + strlen(name) + 1);
    strcpy(full_name, path);
    strcat(full_name, name);

    FILE *src = fopen(full_name, "r");

    addFileInfo(dest, src, name, path);
    addFileContent();

    fclose(src);
    free(full_name);
    return 0;
}

int addFileInfo(FILE *dest, FILE *src, char *name, char *path) {
    fseek(src, 0, SEEK_END);
    int file_size = ftell(src);
    fseek(src, 0, SEEK_SET);

    struct file_info 

    return 0;    
}