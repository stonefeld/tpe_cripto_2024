#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

int get_file_size(FILE *f);

char *get_file_extension(char *filename);

char *get_file_content(FILE *f, int size);

#endif
