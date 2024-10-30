#include "utils.h"

#include <stdlib.h>
#include <string.h>

int utl_filesize(FILE *f) {
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

char *utl_fileext(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot;
}

char *utl_filecontent(FILE *f, int size) {
    fseek(f, 0, SEEK_SET);
    char *content = malloc(size);
    fread(content, 1, size, f);
    return content;
}

struct bmp_header utl_bmpheader(char *bitmap_data) {
    struct bmp_header header;
    memcpy(&header, bitmap_data, sizeof(struct bmp_header));
    return header;
}
