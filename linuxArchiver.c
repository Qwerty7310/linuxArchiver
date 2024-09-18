
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

int main(int argc, char *argv[]) {
    if (argc == 3) {
#ifdef ARCH
        // FILE *res = archive("/home/qwerty/OS/test1/", "../../archive.arch");
        // FILE *res = archive("../../test1/", "../../archive.arch");
        FILE *res =
            archive(argv[1], argv[2]);  // ./linux_archiver '/home/qwerty/OS/test1/' '../../archive.arch'
        fclose(res);
#endif

#ifdef UNARCH
        unarchive(argv[1], argv[2]);  // ./linux_unarchiver '../../archive.arch' '/home/qwerty/OS/Lab_1/'
#endif

    } else
        puts("incorrect input");

    return 0;
}
