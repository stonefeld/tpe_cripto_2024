#ifndef OBFUSCATE_H
#define OBFUSCATE_H

#include "args.h"

#define CONTROL_BYTES_SIZE 4 * 8

enum lsbi_state {
    CHANGED = 0,
    NOT_CHANGED,
};

struct lsbi_counters {
    int c00[2];
    int c01[2];
    int c10[2];
    int c11[2];
};

char* obf_hide(char* message, int msg_size, char* bitmap_data, int bmp_size, enum tipo_steg steg_algo);
char* obf_reveal(char* bitmap_data, int bmp_size, enum tipo_steg steg_algo, char is_encrypted);

#endif
