#include "compress.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "my_dirent.h"

#define WINDOW_SIZE 256
#define LOOKAHEAD_BUFFER_SIZE 32

/*
int main() {
    const char *data = "ABABABABABABABABABABABABAB";
    int data_size = strlen(data);
    char arr[WINDOW_SIZE];

    // FILE *file = fopen("./file.txt", "r");
    // FILE *file = fopen("./Pushkin.jpg", "r");
    FILE *file = fopen("./Im.png", "r");


    if (!file) {
        perror("open");
    } else {
        printf("%ld\n", sizeof(Token));
        deflate(file, "./zip_file.txt");

        FILE *zip_file = fopen("./zip_file.txt", "r");
        // inflate(zip_file, "./new_file.txt");
        // inflate(zip_file, "./new_Pushkin.jpg");
        inflate(zip_file, "./new_im.png");
    }

    return 0;
}
*/

// Поиск наибольшего совпадения строки в окне
Token *findLongestMatch(char *str, int str_size, int buf_size) {
    if (buf_size >= str_size) return NULL;
    Token *token = (Token *)malloc(sizeof(Token));
    if (buf_size < (int)sizeof(Token)) {
        token->length = str[buf_size];
        token->offset = 0;
        return token;
    }

    token->length = 0;
    token->offset = 0;
    for (int i = 0; i < buf_size; i++) {
        int j = 0;
        while ((buf_size + j < str_size) && (i + j < buf_size) && (str[i + j] == str[buf_size + j])) {
            j++;
        }
        if (token->length < j) {
            token->length = j;
            token->offset = buf_size - i;
        }
    }
    if (token->offset == 0) token->length = str[buf_size];

    return token;
}

FILE *deflate(FILE *file, char *path) {
    FILE *zip_file = fopen(path, "w");
    int file_size = getFileSizeInBytes(file);
    // int steps = ((int)(file_size / WINDOW_SIZE) == (file_size / WINDOW_SIZE)) ? (file_size / WINDOW_SIZE)
    //   : (file_size / WINDOW_SIZE + 1);
    char buffer[WINDOW_SIZE];
    while (file_size > 0) {
        int cur_size = 0;
        if (file_size >= WINDOW_SIZE) {
            cur_size = WINDOW_SIZE;
            file_size -= WINDOW_SIZE;
        } else {
            cur_size = file_size;
            file_size = 0;
        }
        fread(buffer, sizeof(char), cur_size, file);

        for (int i = 0; i < cur_size;) {
            Token *token = findLongestMatch(buffer, cur_size, i);
            if (token == NULL) {
                perror("token");
                return NULL;
            }

            // if (token->offset < 0) {
            //     perror("token->offset < 0");
            //     return NULL;
            // }
            fwrite(token, 2, 1, zip_file);

            if (token->offset != 0) {
                i += token->length;
            } else
                i += 1;

            free(token);
        }
    }

    // fclose(zip_file);
    return zip_file;
}

FILE *inflate(FILE *file, char *path) {
    FILE *new_file = fopen(path, "w");
    char buffer[WINDOW_SIZE] = {0};
    int ptr = 0;
    int file_size = getFileSizeInBytes(file);
    if (file_size % 2 != 0) return NULL;

    for (int i = 0; i < file_size / 2; i++) {
        unsigned char off = 0, len = 0;

        fscanf(file, "%c%c", &off, &len);

        // printf("off = %d; len = %d\n", off, len);

        if (off == 0) {
            buffer[ptr] = len;
            ptr += 1;
        } else {
            for (int j = 0; j < len; j++) {
                buffer[ptr] = buffer[ptr - off];
                ptr += 1;
            }
        }
        if (ptr >= WINDOW_SIZE) {
            fwrite(buffer, 1, WINDOW_SIZE, new_file);
            ptr = 0;
        }
    }
    if (ptr != 0) {
        fwrite(buffer, 1, ptr, new_file);
    }

    return new_file;
}

// int getFileSizeInBytes(FILE *file) {
//     int size = 0;
//     fseek(file, 0, SEEK_END);
//     size = ftell(file);
//     rewind(file);
//     return size;
// }
