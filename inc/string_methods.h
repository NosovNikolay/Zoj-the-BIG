#pragma once

#include <stdlib.h>
#include <string.h>

int mx_count_words(const char *str, char delimiter);

char *mx_strnew(const int size);

char *mx_strncpy(char *dst, const char *src, int len);

char **mx_strsplit(char const *s, char c, int *len);

int mx_strlen(const char *);
