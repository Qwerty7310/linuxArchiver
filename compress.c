#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define NULL ((void *)0)

#define WINDOW_SIZE 1024
#define LOOKAHEAD_BUFFER_SIZE 32

typedef struct {
    short offset;
    short length;  // or char, if offset = 0
} Token;

int getFileSizeInBytes(FILE *file);
Token *findLongestMatch(char *str, int str_size, int buf_size, int off);
FILE *deflate(FILE *file, char *path);

int main() {
    const char *data = "ABABABABABABABABABABABABAB";
    int data_size = strlen(data);

    FILE *file = fopen("./file.txt", "r");
    if (!file) {
        perror("open");
    }
    else {
        deflate(file, "");
    }

    // printf("Исходные данные: %s\n", data);
    // printf("Сжатие данных с использованием LZ77:\n");
    // deflate_lz77(data, data_size);
    // printf("%ld  %ld\n", sizeof(Token), sizeof(char));

    return 0;
}

// Поиск наибольшего совпадения строки в окне
Token *findLongestMatch(char *str, int str_size, int buf_size, int off) {
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
            token->length = j;
            token->offset = buf_size - i - off;
        }
    }
    if (token->offset == 0) {
        token->length = str[buf_size];
    }

    return token;
}

FILE *deflate(FILE *file, char *path) {
    // FILE *cmpr_file = fopen(path, "w");
    int file_size = getFileSizeInBytes(file);
    int steps = ((int)(file_size / WINDOW_SIZE) == (file_size / WINDOW_SIZE)) ? (file_size / WINDOW_SIZE) : (file_size / WINDOW_SIZE + 1);
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
        for (int i = 0; i < cur_size; i++) {
            Token *token = findLongestMatch(buffer, cur_size, i, 0);

            printf("token: %d %d\n", token->offset, token->length);
            if (token->offset == 0) printf("%d %c\n", token->offset, token->length);
            else printf("%d %d\n", token->offset, token->length);
            
            free(token);
        }
        printf("-----------\n");
        
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
