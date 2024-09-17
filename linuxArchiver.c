#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// #include <libgen.h> // for basename()

// #include <unistd.h>

#include <string.h>
#include <sys/stat.h>
// #include <sys/syscall.h>

#include "archiver.h"
#include "my_dirent.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

// struct linux_dirent {
//     uint64_t d_ino;
//     int64_t d_off;
//     unsigned short d_reclen;
//     unsigned char d_type;
//     char d_name[];
// };

// struct file_info {
//     unsigned int d_size;
//     char d_path[BUF_SIZE];
//     char d_name[BUF_SIZE];
// };

int readFileInfo(FILE *f_info);

int main() {
    char *path1 = "/home/qwerty/OS/test1/";
    char *path2 = "../../test1/";
    char *path3 = "../../dferfe/";

    int cnt = 0;
    // char buf[BUF_SIZE];

    FILE *res = archive(path1, "../../archive.arch");

    fclose(res);

    res = fopen("../../archive.arch", "r");
    readFileInfo(res);

    /*
    puts(realpath(path1, buf));
    if (realpath(path1, buf) != NULL) {
        char *dir_name = basename(buf);  // Получаем имя каталога
        printf("Имя каталога: %s\n", dir_name);
    } else {
        perror("Ошибка получения реального пути");
    }
    */

    // f_info = fopen("f_info", "r");
    // for (int i = 0; i < cnt; i++) {
    //     readFileInfo(f_content);
    // }
    // fclose(f_info);

    // f_content = fopen("f_content", "r");
    // for (int i = 0; i < 20; i++) {
    //     char c = getc(f_content);
    //     printf("%c", c);
    // }
    // fclose(f_content);

    // DIR *directory = openDir(path1);
    // for (int i = 0; i < 10; i++) {
    //     readDir(directory);
    // }

    // printDir(path2);

    return 0;
}

int readFileInfo(FILE *file) {
    int n;
    fscanf(file, "%d", &n);

    char root_dir_name[BUF_SIZE];
    fread(root_dir_name, sizeof(root_dir_name), 1, file);
    printf("root_dir: %s\n", root_dir_name);

    struct FileInfo *buffer = (struct FileInfo *)malloc(n * sizeof(struct FileInfo));
    fread(buffer, sizeof(struct FileInfo), n, file);

    // fread(buffer, sizeof(struct file_info), 1, f_info);

    for (int i = 0; i < n; i++) {
        printf("path: %s\n", buffer[i].d_path);
        printf("name: %s\n", buffer[i].d_name);
    }

    return 0;
}
