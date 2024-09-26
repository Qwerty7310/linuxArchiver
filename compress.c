#include <stdio.h>
#include <string.h>

#define WINDOW_SIZE 1024
#define LOOKAHEAD_BUFFER_SIZE 32

typedef struct {
    int offset;
    int length; // or char, if offset = 0
} token;

// Поиск наибольшего совпадения строки в окне
token find_longest_match(const char *window, const char *lookahead, int window_size,
                              int lookahead_size) {
    // lz77_token token = {0, 0, lookahead[0]}; // Начальные значения
    token token = {0, 0};  // Начальные значения

    for (int i = 0; i < window_size; i++) {
        int j = 0;
        while (j < lookahead_size && window[i + j] == lookahead[j]) {
            j++;
        }

        if (j > token.length) {
            token.length = j;
            token.offset = window_size - i;
            // if (j < lookahead_size) {
            //     token.next_char = lookahead[j];
            // }
        }
    }

    return token;
}

// Основная функция сжатия с использованием LZ77
void deflate_lz77(const char *input, int input_size) { // inflate
    char window[WINDOW_SIZE + 1] = {0};  // Скольжение окна
    int window_size = 0;

    int pos = 0;
    while (pos < input_size) {
        int lookahead_size =
            input_size - pos > LOOKAHEAD_BUFFER_SIZE ? LOOKAHEAD_BUFFER_SIZE : input_size - pos;

        lz77_token token = find_longest_match(window, &input[pos], window_size, lookahead_size);

        // Выводим токен в формате (offset, length, next_char)
        // printf("(%d, %d, %c)\n", token.offset, token.length, token.next_char);
        printf("(%d, %d)\n", token.offset, token.length);

        // Обновляем окно
        int copy_size = token.length + 1;
        if (window_size + copy_size > WINDOW_SIZE) {
            int shift_size = window_size + copy_size - WINDOW_SIZE;
            memmove(window, window + shift_size, window_size - shift_size);
            window_size -= shift_size;
        }

        strncpy(window + window_size, &input[pos], copy_size);
        window_size += copy_size;

        pos += copy_size;
    }
}

int main() {
    const char *data = "ABABABABABABABABABABABABAB";
    int data_size = strlen(data);

    printf("Исходные данные: %s\n", data);
    printf("Сжатие данных с использованием LZ77:\n");
    deflate_lz77(data, data_size);

    return 0;
}
