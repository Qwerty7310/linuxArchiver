
#ifdef ARCH
#include "archiver.h"
#endif

#ifdef UNARCH
#include "unarchiver.h"
#endif

#include "my_dirent.h"

#define OK 0
#define ERROR 1
#define BUF_SIZE 1024

#define RESET "\033[0m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define WHITE "\033[1;37m"
#define GREEN "\033[1;32m"

int main(int argc, char *argv[]) {
    if (argc == 3) {
#ifdef ARCH
        // FILE *res = archive("/home/qwerty/OS/test1/", "../../archive.arch");
        // FILE *res = archive("../../test1/", "../../archive.arch");
        FILE *res =
            archive(argv[1], argv[2]);  // ./bin/linux_archiver '/home/qwerty/OS/test1/' '../../archive.arch'
        if (!res)
            printf("%sError. The archive was not created.\n%s", RED, RESET);
        else
            printf("%sThe arhcive was created successfully.\n%s", GREEN, RESET);

        fclose(res);
#endif

#ifdef UNARCH
        // int code = 0;
        int code = unarchive(
            argv[1], argv[2]);  // ./bin/linux_unarchiver '../../archive.arch' '/home/qwerty/OS/Lab_1/'
        if (code == ERROR)
            printf("%sError. The archive could not be unzipped.\n%s", RED, RESET);
        else
            printf("%sThe arhcive was unpacked successfully.\n%s", GREEN, RESET);

#endif

    } else
        puts("incorrect input");

    return 0;
}
