#include "compress.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "my_dirent.h"

#define WINDOW_SIZE 256
#define LOOKAHEAD_BUFFER_SIZE 32
#define KEY_STR "#archive#"

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
    if (!zip_file) {
        perror("fopen");
        return NULL;
    }

    // ключ архива
    fprintf(zip_file, KEY_STR);

    int file_size = getFileSizeInBytes(file);
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
        if ((int)fread(buffer, sizeof(char), cur_size, file) < cur_size) {
            perror("fread");
            return NULL;
        }

        for (int i = 0; i < cur_size;) {
            Token *token = findLongestMatch(buffer, cur_size, i);
            if (token == NULL) {
                perror("token");
                return NULL;
            }

            if (fwrite(token, 2, 1, zip_file) < 1) {
                perror("fwrite");
                return NULL;
            }

            if (token->offset != 0) {
                i += token->length;
            } else
                i += 1;

            free(token);
        }
    }

    return zip_file;
}

FILE *inflate(FILE *file, char *path) {
    FILE *new_file = fopen(path, "w");
    if (!new_file) {
        perror("fopen");
        return NULL;
    }

    char buffer[WINDOW_SIZE] = {0};
    int ptr = 0;
    int file_size = getFileSizeInBytes(file);

    // проверка на архив
    char buf_key[10] = {0};
    fread(buf_key, 1, 9, file);
    buf_key[10] = '\0';

    if (strcmp(buf_key, KEY_STR) != 0) {
        puts("This file is not an archive.");
        return NULL;
    }

    file_size -= sizeof(KEY_STR) - 1;  // уменьшаем размер файла на размер ключа

    if ((file_size) % 2 != 0) {
        puts("Error.");
        return NULL;
    }

    for (int i = 0; i < file_size / 2; i++) {
        unsigned char off = 0, len = 0;

        if (fscanf(file, "%c%c", &off, &len) < 2) {
            perror("fscanf");
            return NULL;
        }

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
            if (fwrite(buffer, 1, WINDOW_SIZE, new_file) < WINDOW_SIZE) {
                perror("fwrite");
                return NULL;
            }
            ptr = 0;
        }
    }

    if (ptr != 0) {
        if ((int)fwrite(buffer, 1, ptr, new_file) < ptr) {
            perror("fwrite");
            return NULL;
        }
    }

    return new_file;
}
