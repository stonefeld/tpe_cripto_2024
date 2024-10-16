#include "obfuscate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define BMP_HEADER_SIZE 54

static char* _hide_lsb1(char* message, char* bitmap_data, int bmp_size);
static char* _hide_lsb4(char* message, char* bitmap_data, int bmp_size);
static char* _hide_lsbi(char* message, char* bitmap_data, int bmp_size);
static char* _reveal_lsb1(char* bitmap_data, int bmp_size);
static char* _reveal_lsb4(char* bitmap_data, int bmp_size);
static char* _reveal_lsbi(char* bitmap_data, int bmp_size);

char* hide(char* message, char* bitmap_data, int bmp_size, enum tipo_steg steg_algo) {
    char* hidden_message = NULL;

    switch (steg_algo) {
        case LSB1: hidden_message = _hide_lsb1(message, bitmap_data, bmp_size); break;
        case LSB4: hidden_message = _hide_lsb4(message, bitmap_data, bmp_size); break;
        case LSBI: hidden_message = _hide_lsbi(message, bitmap_data, bmp_size); break;
    }

    return hidden_message;
}

static char* _hide_lsb1(char* message, char* bitmap_data, int bmp_size) {

    int message_length = strlen(message) * 8;
    int capacity = (bmp_size - BMP_HEADER_SIZE) * 8;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough.\n");
        return NULL;
    }

    for (int i = 0; message[i] != '\0'; i++) {
        for (int j = 0; j < 8; j++) {
            bitmap_data[(i*8)+j] = (bitmap_data[(i*8)+j] & 0xFE) | ((message[i] >> j) & 0x01);
        }
    }
    return bitmap_data;
}

static char* _hide_lsb4(char* message, char* bitmap_file, int bmp_size) {
    int message_length = strlen(message) * 8;
    int capacity = (bmp_size - BMP_HEADER_SIZE) * 4;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough.\n");
        return NULL;
    }

    for (int i = 0; message[i] != '\0'; i++) {
        for (int j = 0; j < 2; j++) {
            bitmap_file[BMP_HEADER_SIZE + (i * 2) + j] = (bitmap_file[BMP_HEADER_SIZE + (i * 2) + j] & 0xF0) | ((message[i] >> (j * 4)) & 0x0F);
        }
    }
    return bitmap_file;
}

static char* _hide_lsbi(char* message, char* bitmap_data, int bmp_size) {
    message[0] = bmp_size;
    bitmap_data[0] = '\0';
    return NULL;
}


char* reveal(char* bitmap_data, int bmp_size, enum tipo_steg steg_algo) {
    char* hidden_message = NULL;

    switch (steg_algo) {
        case LSB1: hidden_message = _reveal_lsb1(bitmap_data, bmp_size); break;
        case LSB4: hidden_message = _reveal_lsb4(bitmap_data, bmp_size); break;
        case LSBI: hidden_message = _reveal_lsbi(bitmap_data, bmp_size); break;
    }
    return hidden_message;
}

static char* _reveal_lsb1(char* bitmap_data, int bmp_size) {
    char* message = malloc(bmp_size / 8);
    
    int i;
    for (i = BMP_HEADER_SIZE; message[(i/8)-1] != '\0'; i += 8) {
        char byte = 0;
        for (int j = 0; j < 8; j++) {
            byte |= (bitmap_data[i + j] & 0x01) << j;
        }
        message[i/8] = byte;
    }

    return realloc(message, i/8);
}

static char* _reveal_lsb4(char* bitmap, int bmp_size) {
    char* message = malloc(bmp_size / 2);
    
    int i;
    for (i = BMP_HEADER_SIZE; message[(i/2)-1] != '\0'; i += 2) {
        char byte = 0;
        for (int j = 0; j < 2; j++) {
            byte |= (bitmap[i + j] & 0x0F) << (j * 4);
        }
        message[(i/2)] = byte;
    }

    return realloc(message, i/2);
}

static char* _reveal_lsbi(char* bitmap_data, int bmp_size) {
    bitmap_data[0] = bmp_size;
    return NULL;
}
