#ifndef FILE_INFO
#define FILE_INFO

#define BUF_SIZE 1024

struct FileInfo {
    unsigned int d_size;
    char d_path[BUF_SIZE];
    char d_name[BUF_SIZE];
};

#endif