#include "obfuscate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

static char* _hide_lsb1(char* message, int msg_size, char* bitmap_data, int bmp_size);
static char* _hide_lsb4(char* message, int msg_size, char* bitmap_data, int bmp_size);
static char* _hide_lsbi(char* message, int msg_size, char* bitmap_data, int bmp_size);
static char* _reveal_lsb1(char* bitmap_data, int bmp_size);
static char* _reveal_lsb4(char* bitmap_data, int bmp_size);
static char* _reveal_lsbi(char* bitmap_data, int bmp_size);

char* obf_hide(char* message, int msg_size, char* bitmap_data, int bmp_size, enum tipo_steg steg_algo) {
    char* hidden_message = NULL;

    switch (steg_algo) {
        case LSB1: hidden_message = _hide_lsb1(message, msg_size, bitmap_data, bmp_size); break;
        case LSB4: hidden_message = _hide_lsb4(message, msg_size, bitmap_data, bmp_size); break;
        case LSBI: hidden_message = _hide_lsbi(message, msg_size, bitmap_data, bmp_size); break;
    }

    return hidden_message;
}

char* obf_reveal(char* bitmap_data, int bmp_size, enum tipo_steg steg_algo) {
    char* hidden_message = NULL;

    switch (steg_algo) {
        case LSB1: hidden_message = _reveal_lsb1(bitmap_data, bmp_size); break;
        case LSB4: hidden_message = _reveal_lsb4(bitmap_data, bmp_size); break;
        case LSBI: hidden_message = _reveal_lsbi(bitmap_data, bmp_size); break;
    }
    return hidden_message;
}

/* START EMBED FUNCTIONS */
static char* _hide_lsb1(char* message, int msg_size, char* bitmap_data, int bmp_size) {
    int message_length = msg_size * 8;
    int capacity = bmp_size - BMP_HEADER_SIZE;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough.\n");
        return NULL;
    }

    char* ret = bitmap_data;
    bitmap_data += BMP_HEADER_SIZE;

    for (int i = 0; i <= msg_size; i++) {
        for (int j = 0; j < 8; j++) {
            int idx = (i * 8) + j;
            bitmap_data[idx] = (bitmap_data[idx] & 0xFE) | ((message[i] >> j) & 0x01);
        }
        if (message[i] == '\0')
            break;
    }

    return ret;
}

// TODO: arreglar
static char* _hide_lsb4(char* message, int msg_size, char* bitmap_file, int bmp_size) {
    int message_length = strlen(message) * 2;
    int capacity = bmp_size - BMP_HEADER_SIZE;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough.\n");
        return NULL;
    }

    char* ret = bitmap_file;
    bitmap_file += BMP_HEADER_SIZE;

    for (int i = 0; i < capacity; i++) {
        for (int j = 0; j < 2; j++) {
            int idx = (i * 2) + j;
            bitmap_file[idx] = (bitmap_file[idx] & 0xF0) | ((message[i] >> (j * 4)) & 0x0F);
        }
        if (message[i] == '\0')
            break;
    }

    return ret;
}

// TODO: arreglar
static char* _hide_lsbi(char* message, int msg_size, char* bitmap_data, int bmp_size) {
    message[0] = bmp_size;
    bitmap_data[0] = '\0';
    return NULL;
}
/* END EMBED FUNCTIONS */

/* START EXTRACT FUNCTIONS */
static char* _reveal_lsb1(char* bitmap_data, int bmp_size) {
    int max_msg_size = (bmp_size - BMP_HEADER_SIZE) / 8;
    char* message = malloc(max_msg_size);

    bitmap_data += BMP_HEADER_SIZE;

    int i;
    for (i = 0; i < max_msg_size; i++) {
        char byte = 0;
        for (int j = 0; j < 8; j++) {
            int idx = (i * 8) + j;
            byte |= (bitmap_data[idx] & 0x01) << j;
        }
        message[i] = byte;
        if (byte == '\0')
            break;
    }

    return realloc(message, i);
}

static char* _reveal_lsb4(char* bitmap_data, int bmp_size) {
    char* message = malloc(bmp_size / 2);
    int capacity = (bmp_size - BMP_HEADER_SIZE) / 2;

    bitmap_data += BMP_HEADER_SIZE;

    int i;
    for (i = 0; i < capacity; i += 2) {
        char byte = 0;
        for (int j = 0; j < 2; j++)
            byte |= (bitmap_data[i + j] & 0x0F) << (j * 4);
        message[i / 2] = byte;
        if (byte == '\0')
            break;
    }

    return realloc(message, i / 2);
}

static char* _reveal_lsbi(char* bitmap_data, int bmp_size) {
    bitmap_data[0] = bmp_size;
    return NULL;
}
/* END EXTRACT FUNCTIONS */
