#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

int utl_filesize(FILE *f);
char *utl_fileext(char *filename);
char *utl_filecontent(FILE *f, int size);

#endif
