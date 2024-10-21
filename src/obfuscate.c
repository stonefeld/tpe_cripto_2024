#include "obfuscate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
        printf("Error: BMP file is not big enough. Max capacity: %d\n", capacity);
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
    int message_length = msg_size * 2;
    int capacity = bmp_size - BMP_HEADER_SIZE;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough. Max capacity: %d\n", capacity);
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

static char* _hide_lsbi(char* message, int msg_size, char* bitmap_data, int bmp_size) {
    int message_length = msg_size * 8;
    int capacity = bmp_size - BMP_HEADER_SIZE - CONTROL_BYTES_SIZE;

    if (message_length > capacity) {
        printf("Error: BMP file is not big enough. Max capacity: %d\n", capacity);
        return NULL;
    }

    lsbi_counters* counters = (lsbi_counters*)calloc(1, sizeof(lsbi_counters));
    if (counters == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    char* ret = bitmap_data;
    bitmap_data += BMP_HEADER_SIZE + CONTROL_BYTES_SIZE; // 4 bytes para registro de cambios realizados

    for (int i = 0; i < msg_size; i++) {
        for (int j = 0; j < 8; j++) {
            // me fijo si el bit va a cambiar
            int idx = (i * 8) + j;
            int change = ((message[i] >> j) & 0x01) == (bitmap_data[idx] & 0x01) ? NOT_CHANGED : CHANGED;
            bitmap_data[idx] = (bitmap_data[idx] & 0xFE) | ((message[i] >> j) & 0x01);

            // extraigo el 6to y 7mo bit
            uint8_t bit6 = (bitmap_data[idx] >> 5) & 0x01;
            uint8_t bit7 = (bitmap_data[idx] >> 6) & 0x01;

            // actualizo los contadores
            if (bit6 == 0 && bit7 == 0) {
                counters->c00[change]++;
            } else if (bit6 == 0 && bit7 == 1) {
                counters->c01[change]++;
            } else if (bit6 == 1 && bit7 == 0) {
                counters->c10[change]++;
            } else if (bit6 == 1 && bit7 == 1) {
                counters->c11[change]++;
            }
        }
        if (message[i] == '\0')
            break;
    }

    bitmap_data = ret + BMP_HEADER_SIZE;

    if (counters->c00[CHANGED] > counters->c00[NOT_CHANGED]) {
        counters->must_change[0] = 1;
        // el ultimo bit del primer byte lo pongo en 1 porque modifico para 00
        bitmap_data[0] |= 0x01;
    } else {
        // el ultimo bit del primer byte lo pongo en 0 porque no modifico para 00
        bitmap_data[0] &= 0xFE;
    }
    if (counters->c01[CHANGED] > counters->c01[NOT_CHANGED]) {
        counters->must_change[1] = 1;
        // el ultimo bit del segundo byte lo pongo en 1 porque modifico para 01
        bitmap_data[1] |= 0x01;
    } else {
        // el ultimo bit del segundo byte lo pongo en 0 porque no modifico para 01
        bitmap_data[1] &= 0xFE;
    }
    if (counters->c10[CHANGED] > counters->c10[NOT_CHANGED]) {
        counters->must_change[2] = 1;
        // el ultimo bit del tercer byte lo pongo en 1 porque modifico para 10
        bitmap_data[2] |= 0x01;
    } else {
        // el ultimo bit del tercer byte lo pongo en 0 porque no modifico para 10
        bitmap_data[2] &= 0xFE;
    }
    if (counters->c11[CHANGED] > counters->c11[NOT_CHANGED]) {
        counters->must_change[3] = 1;
        // el ultimo bit del cuarto byte lo pongo en 1 porque modifico para 11
        bitmap_data[3] |= 0x01;
    } else {
        // el ultimo bit del cuarto byte lo pongo en 0 porque no modifico para 11
        bitmap_data[3] &= 0xFE;
    }

    bitmap_data = ret + BMP_HEADER_SIZE + CONTROL_BYTES_SIZE;

    for (int i = 0; i < msg_size; i++) {
        for (int j = 0; j < 8; j++) {
            int idx = (i * 8) + j;

            // extraigo el 6to y 7mo bit
            uint8_t bit6 = (bitmap_data[idx] >> 5) & 0x01;
            uint8_t bit7 = (bitmap_data[idx] >> 6) & 0x01;

            // si tengo que cambiar el bit
            if (counters->must_change[(bit6 << 1) | bit7] == 1) {
                bitmap_data[idx] ^= 0x01;
            }
        }
    }

    free(counters);

    return ret;
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
    char* original_bitmap_data = bitmap_data;
    int max_msg_size = (bmp_size - BMP_HEADER_SIZE - CONTROL_BYTES_SIZE) / 8;

    int must_invert[4] = {0};
    
    bitmap_data += BMP_HEADER_SIZE;

    for (int i = 0; i < 4; i++) {
        must_invert[i] = (bitmap_data[i] & 0x01) == 0x01 ? 1 : 0;
    }
    
    bitmap_data += CONTROL_BYTES_SIZE;

    for (int i = 0; i < max_msg_size; i++) {
        for (int j = 0; j < 8; j++) {
            int idx = (i * 8) + j;

            // extraigo el 6to y 7mo bit
            uint8_t bit6 = (bitmap_data[idx] >> 5) & 0x01;
            uint8_t bit7 = (bitmap_data[idx] >> 6) & 0x01;

            // si tengo que cambiar el bit
            if (must_invert[(bit6 << 1) | bit7] == 1) {
                bitmap_data[idx] ^= 0x01;
            }
        }
    }
    
    // genero el nuevo char* que no tiene los 4 bytes de control
    char* new_bitmap_data = malloc(bmp_size - CONTROL_BYTES_SIZE);
    memcpy(new_bitmap_data, original_bitmap_data, BMP_HEADER_SIZE);
    memcpy(new_bitmap_data + BMP_HEADER_SIZE, original_bitmap_data + BMP_HEADER_SIZE + CONTROL_BYTES_SIZE, bmp_size - BMP_HEADER_SIZE - CONTROL_BYTES_SIZE);

    char* hidden_message = _reveal_lsb1(new_bitmap_data, bmp_size - CONTROL_BYTES_SIZE);
    free(new_bitmap_data);

    return hidden_message;
}
/* END EXTRACT FUNCTIONS */
