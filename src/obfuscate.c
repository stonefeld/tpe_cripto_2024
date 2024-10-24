#include "obfuscate.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

static char* _hide_lsb1(char* message, int msg_size, char* bitmap_data, int bmp_size);
static char* _hide_lsb4(char* message, int msg_size, char* bitmap_data, int bmp_size);
static char* _hide_lsbi(char* message, int msg_size, char* bitmap_data, int bmp_size);
static char* _reveal_lsb1(char* bitmap_data, int bmp_size, char is_encrypted);
static char* _reveal_lsb4(char* bitmap_data, int bmp_size, char is_encrypted);
static char* _reveal_lsbi(char* bitmap_data, int bmp_size, char is_encrypted);

char* obf_hide(char* message, int msg_size, char* bitmap_data, int bmp_size, enum tipo_steg steg_algo) {
    switch (steg_algo) {
        case LSB1: return _hide_lsb1(message, msg_size, bitmap_data, bmp_size);
        case LSB4: return _hide_lsb4(message, msg_size, bitmap_data, bmp_size);
        case LSBI: return _hide_lsbi(message, msg_size, bitmap_data, bmp_size);
        default: return NULL;
    }
}

char* obf_reveal(char* bitmap_data, int bmp_size, enum tipo_steg steg_algo, char is_encrypted) {
    switch (steg_algo) {
        case LSB1: return _reveal_lsb1(bitmap_data, bmp_size, is_encrypted);
        case LSB4: return _reveal_lsb4(bitmap_data, bmp_size, is_encrypted);
        case LSBI: return _reveal_lsbi(bitmap_data, bmp_size, is_encrypted);
        default: return NULL;
    }
}

/* START EMBED FUNCTIONS */
static char* _hide_lsb1(char* message, int msg_size, char* bitmap_data, int bmp_size) {
    int message_length = msg_size * 8;
    int capacity = bmp_size - BMP_HEADER_SIZE;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough. Max capacity: %d\n", capacity / 8);
        return NULL;
    }

    char* ret = bitmap_data;
    bitmap_data += BMP_HEADER_SIZE;

    for (int i = 0; i <= msg_size; i++) {
        for (int j = 0; j < 8; j++) {
            int idx = (i * 8) + j;
            bitmap_data[idx] = (bitmap_data[idx] & 0xFE) | ((message[i] >> (7 - j)) & 0x01);
        }
    }

    return ret;
}

static char* _hide_lsb4(char* message, int msg_size, char* bitmap_file, int bmp_size) {
    int message_length = msg_size * 2;
    int capacity = bmp_size - BMP_HEADER_SIZE;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough. Max capacity: %d\n", capacity / 2);
        return NULL;
    }

    char* ret = bitmap_file;
    bitmap_file += BMP_HEADER_SIZE;

    for (int i = 0; i <= msg_size; i++) {
        for (int j = 0; j < 2; j++) {
            int idx = (i * 2) + j;
            bitmap_file[idx] = (bitmap_file[idx] & 0xF0) | ((message[i] >> ((1 - j) * 4)) & 0x0F);
        }
    }

    return ret;
}

static char* _hide_lsbi(char* message, int msg_size, char* bitmap_data, int bmp_size) {
    int message_length = msg_size * 8 + 4;
    int capacity = bmp_size - BMP_HEADER_SIZE;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough. Max capacity: %d\n", capacity / 8);
        return NULL;
    }

    char* ret = bitmap_data;
    char* og_bitmap_data = malloc(bmp_size);
    memcpy(og_bitmap_data, bitmap_data, bmp_size);

    bitmap_data += BMP_HEADER_SIZE;
    struct lsbi_counters counters = {0};

    unsigned int red_count = 0;

    for (int i = 0; i <= msg_size; i++) {
        for (int j = 0; j < 8; j++) {
            int idx = (i * 8) + j + 4 + red_count;
            if (idx % 3 == 2) {
                idx++;
                red_count++;
            }

            bitmap_data[idx] = (bitmap_data[idx] & 0xFE) | ((message[i] >> (7 - j)) & 0x01);

            int changed = (og_bitmap_data[idx] & 0x01) != (bitmap_data[idx] & 0x01);

            char bit2 = (bitmap_data[idx] >> 1) & 0x01;
            char bit3 = (bitmap_data[idx] >> 2) & 0x01;

            if (bit2 == 0) {
                if (bit3 == 0)
                    counters.c00[changed]++;
                else if (bit3 == 1)
                    counters.c10[changed]++;
            } else if (bit2 == 1) {
                if (bit3 == 0)
                    counters.c01[changed]++;
                else if (bit3 == 1)
                    counters.c11[changed]++;
            }
        }
    }

    free(og_bitmap_data);

    int must_change[4];
    must_change[0] = counters.c00[1] > counters.c00[0];
    must_change[1] = counters.c01[1] > counters.c01[0];
    must_change[2] = counters.c10[1] > counters.c10[0];
    must_change[3] = counters.c11[1] > counters.c11[0];

    for (int i = 0; i < 4; i++) {
        bitmap_data[i] = (bitmap_data[i] & 0xFE) | must_change[i];
    }

    for (int i = 0; i <= msg_size; i++) {
        for (int j = 0; j < 8; j++) {
            int idx = (i * 8) + j + 4;
            if (must_change[(bitmap_data[idx] & 0x06) >> 1] == 1)
                bitmap_data[idx] ^= 0x01;
        }
    }

    return ret;
}
/* END EMBED FUNCTIONS */

/* START EXTRACT FUNCTIONS */
static char* _reveal_lsb1(char* bitmap_data, int bmp_size, char is_encrypted) {
    int max_msg_size = (bmp_size - BMP_HEADER_SIZE) / 8;
    char* message = malloc(max_msg_size);

    bitmap_data += BMP_HEADER_SIZE;

    unsigned int size = 0;
    unsigned int position;
    unsigned char byte = '\0';

    for (position = 0; position < 4; position++) {
        byte = 0;
        for (int j = 0; j < 8; j++) {
            int idx = (position * 8) + j;
            byte |= (bitmap_data[idx] & 0x01) << (7 - j);
        }
        message[position] = byte;
        size |= byte << (24 - (position * 8));
    }

    for (; position < size + 4; position++) {
        byte = 0;
        for (int j = 0; j < 8; j++) {
            int idx = (position * 8) + j;
            byte |= (bitmap_data[idx] & 0x01) << (7 - j);
        }
        message[position] = byte;
    }

    if (!is_encrypted) {
        for (;; position++) {
            byte = 0;
            for (int j = 0; j < 8; j++) {
                int idx = (position * 8) + j;
                byte |= (bitmap_data[idx] & 0x01) << (7 - j);
            }
            message[position] = byte;
            if (byte == '\0')
                break;
        }
    }

    return realloc(message, position + 1);
}

static char* _reveal_lsb4(char* bitmap_data, int bmp_size, char is_encrypted) {
    int max_msg_size = (bmp_size - BMP_HEADER_SIZE) / 2;
    char* message = malloc(max_msg_size);

    bitmap_data += BMP_HEADER_SIZE;

    unsigned int size = 0;
    unsigned int position;
    unsigned char byte = '\0';

    for (position = 0; position < 4; position++) {
        byte = 0;
        for (int j = 0; j < 2; j++) {
            int idx = (position * 2) + j;
            byte |= (bitmap_data[idx] & 0x0F) << ((1 - j) * 4);
        }
        message[position] = byte;
        size |= byte << (24 - (position * 8));
    }

    for (; position < size + 4; position++) {
        byte = 0;
        for (int j = 0; j < 2; j++) {
            int idx = (position * 2) + j;
            byte |= (bitmap_data[idx] & 0x0F) << ((1 - j) * 4);
        }
        message[position] = byte;
    }

    if (!is_encrypted) {
        for (;; position++) {
            byte = 0;
            for (int j = 0; j < 2; j++) {
                int idx = (position * 2) + j;
                byte |= (bitmap_data[idx] & 0x0F) << ((1 - j) * 4);
            }
            message[position] = byte;
            if (byte == '\0')
                break;
        }
    }

    return realloc(message, position + 1);
}

static char* _reveal_lsbi(char* bitmap_data, int bmp_size, char is_encrypted) {
    int max_msg_size = (bmp_size - BMP_HEADER_SIZE) / 8 - 4;
    char* message = malloc(max_msg_size);

    bitmap_data += BMP_HEADER_SIZE;
    int must_change[4];
    for (int i = 0; i < 4; i++)
        must_change[i] = (bitmap_data[i] & 0x01);

    unsigned int size = 0;
    unsigned int position;
    unsigned char byte = '\0';

    unsigned int red_count = 0;

    for (position = 0; position < 4; position++) {
        byte = 0;
        for (int j = 0; j < 8; j++) {
            int idx = (position * 8) + j + 4 + red_count;
            if (idx % 3 == 2) {
                idx++;
                red_count++;
            }

            char bit = (bitmap_data[idx] & 0x01);
            if (must_change[(bitmap_data[idx] & 0x06) >> 1] == 1)
                bit ^= 0x01;
            byte |= bit << (7 - j);
        }
        message[position] = byte;
        size |= byte << (24 - (position * 8));
    }

    for (; position < size + 4; position++) {
        byte = 0;
        for (int j = 0; j < 8; j++) {
            int idx = (position * 8) + j + 4 + red_count;
            if (idx % 3 == 2) {
                idx++;
                red_count++;
            }

            char bit = (bitmap_data[idx] & 0x01);
            if (must_change[(bitmap_data[idx] & 0x06) >> 1] == 1)
                bit ^= 0x01;
            byte |= bit << (7 - j);
        }
        message[position] = byte;
    }

    if (!is_encrypted) {
        for (;; position++) {
            byte = 0;
            for (int j = 0; j < 8; j++) {
                int idx = (position * 8) + j + 4 + red_count;
                if (idx % 3 == 2) {
                    idx++;
                    red_count++;
                }

                char bit = (bitmap_data[idx] & 0x01);
                if (must_change[(bitmap_data[idx] & 0x06) >> 1] == 1)
                    bit ^= 0x01;
                byte |= bit << (7 - j);
            }
            message[position] = byte;
            if (byte == '\0')
                break;
        }
    }

    return realloc(message, position + 1);
}
/* END EXTRACT FUNCTIONS */
