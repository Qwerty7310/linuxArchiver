#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>



#include "my_dirent.h"

#define BUF_SIZE 1024

struct linux_dirent64 {
    uint64_t d_ino;
    int64_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

DIR *openDir(char *name) {
    int fd = open(name, O_RDONLY | O_DIRECTORY);
    if (fd == -1) return NULL;

    DIR *dir = malloc(sizeof(DIR));
    if (!dir) {
        close(fd);
        return NULL;
    }

    dir->fd = fd;
    dir->buf_pos = 0;
    dir->buf_end = 0;

    return dir;
}

int closeDir(DIR *dir) {
    if (dir == NULL) return -1;
    close(dir->fd);
    free(dir);
    return 0;
}

struct dirent *readDir(DIR *dir) {
    if (dir->buf_pos >= dir->buf_end) {
        int nread = syscall(SYS_getdents64, dir->fd, dir->buffer, BUF_SIZE);
        if (nread == -1) {
            perror("syscall");
            return NULL;
        }
        if (nread == 0) 
            return NULL;
        

        dir->buf_pos = 0;
        dir->buf_end = nread;
    }

    struct linux_dirent64 *entry = (struct linux_dirent64 *)(dir->buffer + dir->buf_pos);
    struct dirent *result = malloc(sizeof(struct dirent));
    if (!result) 
        return NULL;
    

    result->d_ino = entry->d_ino;
    result->d_off = entry->d_off;
    result->d_reclen = entry->d_reclen;
    result->d_type = entry->d_type;
    strncpy(result->d_name, entry->d_name, sizeof(result->d_name) - 1);
    result->d_name[sizeof(result->d_name) - 1] = '\0';

    dir->buf_pos += entry->d_reclen;
 
    return result;
}