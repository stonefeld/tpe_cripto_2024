#include "utils.h"

#include <stdlib.h>
#include <string.h>

int get_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

char *get_file_extension(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot;
}

char *get_file_content(FILE *f, int size) {
    fseek(f, 0, SEEK_SET);
    char *content = malloc(size);
    fread(content, 1, size, f);
    return content;
}
