#ifndef OBFUSCATE_H
#define OBFUSCATE_H

#include "args.h"

char* obf_hide(char* message, int msg_size, char* bitmap_data, int bmp_size, enum tipo_steg steg_algo);
char* obf_reveal(char* bitmap_data, int bmp_size, enum tipo_steg steg_algo);

#endif
