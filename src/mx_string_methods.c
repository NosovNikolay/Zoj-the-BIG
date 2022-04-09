#include "string_methods.h"

int mx_strlen(const char *s) {
    int count = 0;
    while (*s++) count++;
    return count;
}

int mx_count_words(const char *str, char delimiter) {
    while (*str == delimiter) str++;
    if (!*str) return 0;
    int count = 0;
    int skip = 0;
    while (*str) {
        char c = *str++;
        if (c == delimiter && skip) continue;
        if (c == delimiter) {
            skip = 1;
            count++;
            continue;
        }
        skip = 0;
    }
    if (*(str - 1) != delimiter) count++;
    return count;
}

char *mx_strnew(const int size) {
    if (size < 0) return NULL;
    char *str = (char *)malloc(size + 1);
    for (int i = 0; i <= size; i++) str[i] = '\0';
    return str;
}

char *mx_strncpy(char *dst, const char *src, int len) {
    char *init_dst = dst;
    for (int i = 0; i != len && *src; i++) *dst++ = *src++;
    *dst = '\0';
    return init_dst;
}

char **mx_strsplit(char const *s, char c, int *len) {
    if (s == NULL) return NULL;
    char **words = (char**)malloc((mx_count_words(s, c) + 1) * 8);
    int skip = 0;
    int char_count = 0;
    int word_idx = 0;
    while (*s == c) s++;
    while (*s) {
        if (*s++ == c) {
            if (!skip) {
                words[word_idx++] = mx_strncpy(mx_strnew(char_count), s - char_count - 1, char_count);
                skip = 1;
                char_count = 0;
            }
            continue;
        }
        skip = 0;
        char_count++;
        if (*s == '\0')
            words[word_idx++] = mx_strncpy(mx_strnew(char_count), s - char_count, char_count);
    }
    words[word_idx] = NULL;
    *len = word_idx;
    return words;
}
