#include "hide.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define BMP_HEADER_SIZE 54

static char* _hide_lsb1(char* message, char* bitmap_data, int bmp_size);
static char* _hide_lsb4(char* message, char* bitmap_data, int bmp_size);
static char* _hide_lsbi(char* message, char* bitmap_data, int bmp_size);

void hide(char* message, char* bitmap_data, int bmp_size, enum tipo_steg steg_algo, char* output_file) {
    char* hidden_message = NULL;

    switch (steg_algo) {
        case LSB1: hidden_message = _hide_lsb1(message, bitmap_data, bmp_size); break;
        case LSB4: hidden_message = _hide_lsb4(message, bitmap_data, bmp_size); break;
        case LSBI: hidden_message = _hide_lsbi(message, bitmap_data, bmp_size); break;
    }

    if (hidden_message) {
        FILE* f = fopen(output_file, "wb");
        if (f == NULL) {
            printf("Error: Could not open output file.\n");
            exit(EXIT_FAILURE);
        }

        fwrite(hidden_message, 1, bmp_size, f);
        fclose(f);
    } else {
        printf("Error: Could not hide message.\n");
        exit(EXIT_FAILURE);
    }
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
    return NULL;
}
