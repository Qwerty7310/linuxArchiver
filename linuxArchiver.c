
#ifdef ARCH
#include "archiver.h"
#endif

#ifdef UNARCH
#include "unarchiver.h"
#endif

#include "my_dirent.h"
#include <string.h>
#include <stdlib.h>

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

#define RESET "\033[0m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define WHITE "\033[1;37m"
#define GREEN "\033[1;32m"
#define ORANGE "\033[38;5;214m"

int main(int argc, char *argv[]) {
    if (argc == 3) {

#ifdef ARCH
        char *src_path = (char *)malloc(sizeof(argv[1]) + 1);
        strcpy(src_path, argv[1]);
        strcat(src_path, "/");

        FILE *res =
            archive(src_path, argv[2]);  // ./bin/linux_archiver '/home/qwerty/OS/test1/' '../../archive.arch'
        if (!res)
            printf("%sError. The archive was not created.\n%s", RED, RESET);
        else
            printf("%sThe archive was created successfully.\n%s", GREEN, RESET);

        if (res) fclose(res);
        free(src_path);
#endif

#ifdef UNARCH
        char *src_path = (char *)malloc(sizeof(argv[1]) + 1);
        strcpy(src_path, argv[2]);
        strcat(src_path, "/");

        int code = unarchive(
            argv[1], src_path);  // ./bin/linux_unarchiver '../../archive.arch' '/home/qwerty/OS/Lab_1/'
        if (code == ERROR)
            printf("%sError. The archive could not be unzipped.%s\n", RED, RESET);
        else
            printf("%sThe arhcive was unpacked successfully.%s\n", GREEN, RESET);

#endif

    } else

#ifdef ARCH
        printf("%sUsage: ./linux_archiver <directory_path> <archive_path>%s\n", ORANGE, RESET);
#endif

#ifdef UNARCH
        printf("%sUsage: ./linux_unarchiver <archive_path> <directory_path>%s\n", ORANGE, RESET);
#endif

    return 0;
}
