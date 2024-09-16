#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>


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
int openDir(char *path);
int printDir(char *path);

int printFileName(char *name, char *path);
int readDir(char *path);

int addFileToArchive(FILE *dest, char *name, char *path);
int addFileInfo(FILE *dest, FILE *src, char *name, char *path);


int printDir_1(char *path);
struct linux_dirent *readDir_1(int directory);

int main() {
    char *path1 = "/home/qwerty/OS/test1/";
    char *path2 = "../../test1/";
    char *path3 = "../../dferfe/";

    // readDir(path1);

    int directory = openDir(path1);
    for (int i = 0; i < 10; i++) {
        struct linux_dirent *dir = readDir_1(directory);
        printf("%ld ", lseek(directory, 0, SEEK_CUR));
        if (dir == NULL) {
            printf("NULL\n");
        } else {
            printf("%s\n", dir->d_name);
        }
    }
    if (printDir_1(path1) == ERROR) {
        perror("Reading directory");
    }

    return 0;
}

int openDir(char *path) {
    int directory = open(path, O_RDONLY | O_DIRECTORY);
    return directory;
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

int printDir_1(char *path) {
    int directory = openDir(path);
    if (directory == -1) {
        perror("Open directory");
        return ERROR;
    }
    struct linux_dirent *dir;
    while ((dir = readDir_1(directory)) != NULL) {
        if (strcmp(".", dir->d_name) && strcmp("..", dir->d_name)) {
            struct stat stat_buf;
            char *full_path = (char *)malloc(strlen(path) + strlen(dir->d_name) + 1 + 1); // for '/' and '\0'
            strcpy(full_path, path);
            strcat(full_path, dir->d_name);
            if (stat(full_path, &stat_buf) != 0) {
                perror("stat");
                return ERROR;
            }
            if (S_ISDIR(stat_buf.st_mode)) {
                strcat(full_path, "/");
                if (printDir_1(full_path) == ERROR) {
                    return ERROR;
                }
            } else {
                // printf("%s\n", dir->d_name);
            }

            free(full_path);
        }
    }
    readDir_1(directory);
    close(directory);
    return OK;
}

struct linux_dirent *readDir_1(int directory) {
    char buffer[BUF_SIZE];
    unsigned short current_pos = lseek(directory, 0, SEEK_CUR);
    if (current_pos == -1) {
        perror("lseek");
        return NULL;
    }
    int nread = syscall(SYS_getdents64, directory, buffer, BUF_SIZE);

    if (nread == -1) {
        perror("syscall");
        printf("nread == -1\n");
        return NULL;
    }
    if (nread == 0) {
        printf("nread == 0\n");
        return NULL;
    }

    // struct linux_dirent *dir = (struct linux_dirent *)malloc(sizeof(struct linux_dirent));
    struct linux_dirent *dir = (struct linux_dirent *) buffer;
    struct linux_dirent *dir1 = (struct linux_dirent *) (buffer + dir->d_reclen);
    if (dir == NULL) {
        perror("malloc");
        return NULL;
    }
    printf("%s\n", dir->d_name);
    printf("%s\n", dir1->d_name);
    // memcpy(dir, buffer, sizeof(struct linux_dirent));
    lseek(directory, 32, SEEK_SET);


    return dir;
}

int readDir(char *path) {
    int directory = openDir(path);
    if (directory == -1) {
        perror("Open directory");
        return ERROR;
    }

    int nread;
    char buffer[BUF_SIZE];
    int res = OK;
    int cnt = 0;
    while ((nread = syscall(SYS_getdents64, directory, buffer, BUF_SIZE)) > 0) {
        // printf("%d\n", ++cnt);
        // printf("nread = %d\n", nread);
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
                    int res = readDir(cur_path);
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
}

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