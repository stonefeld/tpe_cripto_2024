#include "embed.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int _get_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    // avoid reading the null or EOF terminator
    int size = ftell(f) - 1;
    fseek(f, 0, SEEK_SET);
    return size;
}

static char *_get_file_extension(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot;
}

static char *_get_file_content(FILE *f, int size) {
    fseek(f, 0, SEEK_SET);
    char *content = malloc(size);
    fread(content, 1, size, f);
    return content;
}

void embed(struct args args) {
    FILE *f = fopen(args.input_file, "rb");
    if (f == NULL) {
        printf("Error: Could not open file.\n");
        exit(EXIT_FAILURE);
    }

    int size = _get_file_size(f);
    char *ext = _get_file_extension(args.input_file);
    int ext_len = strlen(ext) + 1;
    char *content = _get_file_content(f, size);

    char *message = malloc(4 + size + ext_len);
    sprintf(message, "%.*d", 4, size);
    memcpy(message + 4, content, size);
    memcpy(message + 4 + size, ext, ext_len);

    free(message);
    free(content);
    fclose(f);
}
