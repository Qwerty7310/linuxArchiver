
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
        char *path1 = "/home/qwerty/OS/test3/";
        char *path2 = "../../test1/";
        char *path3 = "../../dferfe/";

        // FILE *res = archive(path1, "../../archive.arch");
        puts(argv[1]);
        FILE *res =
            archive(argv[1], argv[2]);  // ./linux_archiver '/home/qwerty/OS/test3/' '../../archive.arch'
        fclose(res);
#endif

#ifdef UNARCH
        unarchive(argv[1], argv[2]);  // ./linux_unarchiver '../../archive.arch' '/home/qwerty/OS/Lab_1/'
#endif

    } else {
        perror("incorrect input");
    }

    return 0;
}
