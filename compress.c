#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define NULL ((void *)0)

#define WINDOW_SIZE 256
#define LOOKAHEAD_BUFFER_SIZE 32

typedef struct {
    unsigned char offset;
    unsigned char length;  // or char, if offset = 0
} Token;

// typedef struct {
//     int offset;
//     int length;  // or char, if offset = 0
// } Token;

int getFileSizeInBytes(FILE *file);
Token *findLongestMatch(char *str, int str_size, int buf_size);
FILE *deflate(FILE *file, char *path);
int inflate(FILE *file, char *path);

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

    // printf("Исходные данные: %s\n", data);
    // printf("Сжатие данных с использованием LZ77:\n");
    // deflate_lz77(data, data_size);
    // printf("%ld\n", sizeof((char)(255)));

    return 0;
}

// Поиск наибольшего совпадения строки в окне
Token *findLongestMatch(char *str, int str_size, int buf_size) {
    if (buf_size >= str_size) return NULL;
    Token *token = (Token *)malloc(sizeof(Token));
    if (buf_size < sizeof(Token)) {
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
            // *off += j - token->length;
            token->length = j;
            token->offset = buf_size - i;
            if (token->offset < 0) {
                printf("AAAAAAAAAAAAAAAAAAAAAAAAA %d %d %d\n", buf_size, i, token->offset);
            }
        }
    }
    if (token->offset == 0) {
        token->length = str[buf_size];
    }

    return token;
}

FILE *deflate(FILE *file, char *path) {
    FILE *zip_file = fopen(path, "w");
    int file_size = getFileSizeInBytes(file);
    int steps = ((int)(file_size / WINDOW_SIZE) == (file_size / WINDOW_SIZE)) ? (file_size / WINDOW_SIZE)
                                                                              : (file_size / WINDOW_SIZE + 1);
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
        printf("cur_size = %d\n", cur_size);
        fread(buffer, sizeof(char), cur_size, file);

        for (int i = 0; i < cur_size;) {
            Token *token = findLongestMatch(buffer, cur_size, i);
            if (token == NULL) {
                perror("token");
                return NULL;
            }

            if (token->offset < 0) {
                perror("token->offset < 0");
                return NULL;
            }
            fwrite(token, 2, 1, zip_file);
            // fwrite(token, sizeof(Token), 1, zip_file);


            // printf("token: %d %d\n", token->offset, token->length);
            if (token->offset == 0)
                printf("%d %c\n", token->offset, token->length);
            else
                printf("%d %d\n", token->offset, token->length);

            if (token->offset != 0) {
                i += token->length;
                // printf("%d\n", token->length);
            } else {
                i += 1;
            }

            // printf("sizeof(token): %ld\n", sizeof(token));
            free(token);
        }
        printf("-----------\n");
    }
    fclose(zip_file);
}

int inflate(FILE *file, char *path) {
    printf("-----INFLATE-----\n");
    FILE *new_file = fopen(path, "w");
    char buffer[WINDOW_SIZE] = {0};
    int ptr = 0;
    int file_size = getFileSizeInBytes(file);
    if (file_size % 2 != 0) return 0;
    // if (file_size % sizeof(Token) != 0) return 0;


    for (int i = 0; i < file_size / 2; i++) {
    // for (int i = 0; i < file_size / sizeof(Token); i++) {

        unsigned char off = 0, len = 0;
        // int off = 0, len = 0;

        fscanf(file, "%c%c", &off, &len);
        // Token *token = (Token *)malloc(sizeof(Token));
        // fread(token, sizeof(Token), 1, file);

        printf("off = %d; len = %d\n", off, len);

        if (off == 0) {
            buffer[ptr] = len;
            // printf("%c\n", buffer[ptr]);
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

        // if (token->offset == 0) {
        //     buffer[ptr] = token->length;
        //     // printf("%c\n", buffer[ptr]);
        //     ptr += 1;
        // } else {
        //     for (int j = 0; j < token->length; j++) {
        //         buffer[ptr] = buffer[ptr - token->offset];
        //         printf("%c\n", buffer[ptr]);
        //         ptr += 1;
        //     }
        // }
        // if (ptr >= WINDOW_SIZE) {
        //     fwrite(buffer, 1, WINDOW_SIZE, new_file);
        //     ptr = 0;
        // }
    }
    if (ptr != 0) {
        fwrite(buffer, 1, ptr, new_file);
    }
}

// Основная функция сжатия с использованием LZ77
// void deflate_lz77(const char *input, int input_size) {  // inflate
//     char window[WINDOW_SIZE + 1] = {0};                 // Скольжение окна
//     int window_size = 0;

//     int pos = 0;
//     while (pos < input_size) {
//         int lookahead_size =
//             input_size - pos > LOOKAHEAD_BUFFER_SIZE ? LOOKAHEAD_BUFFER_SIZE : input_size - pos;

//         lz77_token token = find_longest_match(window, &input[pos], window_size, lookahead_size);

//         // Выводим токен в формате (offset, length, next_char)
//         // printf("(%d, %d, %c)\n", token.offset, token.length, token.next_char);
//         printf("(%d, %d)\n", token.offset, token.length);

//         // Обновляем окно
//         int copy_size = token.length + 1;
//         if (window_size + copy_size > WINDOW_SIZE) {
//             int shift_size = window_size + copy_size - WINDOW_SIZE;
//             memmove(window, window + shift_size, window_size - shift_size);
//             window_size -= shift_size;
//         }

//         strncpy(window + window_size, &input[pos], copy_size);
//         window_size += copy_size;

//         pos += copy_size;
//     }
// }

int getFileSizeInBytes(FILE *file) {
    int size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    return size;
}
