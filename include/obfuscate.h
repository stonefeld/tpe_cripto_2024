#ifndef OBFUSCATE_H
#define OBFUSCATE_H

#include "args.h"

char* hide(char* message, char* bitmap_data, int bmp_size, enum tipo_steg steg_algo);

char* reveal(char* bitmap_data, int bmp_size, enum tipo_steg steg_algo);

#endif
