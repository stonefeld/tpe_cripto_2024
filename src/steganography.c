#include "steganography.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"
#include "cipher.h"
#include "obfuscate.h"
#include "utils.h"

void steg_embed(struct args args) {
    FILE* input_file = fopen(args.input_file, "rb");
    if (input_file == NULL) {
        printf("Error: Could not open input file.\n");
        exit(EXIT_FAILURE);
    }

    int size = utl_filesize(input_file) - 1;
    char* ext = utl_fileext(args.input_file);
    int ext_len = strlen(ext);
    char* content = utl_filecontent(input_file, size);

    int len = 4 + size + ext_len;
    char* message = malloc(len + 1);

    // sprintf(message, "%.*d", 4, size);
    message[0] = (size >> 24) & 0xFF;
    message[1] = (size >> 16) & 0xFF;
    message[2] = (size >> 8) & 0xFF;
    message[3] = size & 0xFF;
    memcpy(message + 4, content, size);
    memcpy(message + 4 + size, ext, ext_len);
    message[len] = '\0';

    free(content);
    fclose(input_file);

    if (args.password) {
        char* encrypted_message = encrypt(message, args.encryption_algo, args.mode, args.password, len, &len);
        if (encrypted_message) {
            free(message);
            message = encrypted_message;
        } else {
            printf("Encryption failed.\n");
            free(message);
            exit(EXIT_FAILURE);
        }
    }

    FILE* bitmap_file = fopen(args.bitmap_file, "rb");
    if (bitmap_file == NULL) {
        printf("Error: Could not open bitmap file.\n");
        exit(EXIT_FAILURE);
    }

    int bmp_size = utl_filesize(bitmap_file);
    char* bitmap_data = utl_filecontent(bitmap_file, bmp_size);
    struct bmp_header bitmap_header = bmp_get_header(bitmap_data);

    fclose(bitmap_file);

    if (bitmap_header.bits_per_pixel != 24) {
        printf("Error: Bitmap must be 24 bits per pixel.\n");
        exit(EXIT_FAILURE);
    }
    if (bitmap_header.compression != BI_RGB) {
        printf("Error: Bitmap must not be compressed.\n");
        exit(EXIT_FAILURE);
    }

    char* hidden_bmp = obf_hide(message, len, bitmap_data, bmp_size, args.steg_algo);

    if (hidden_bmp) {
        FILE* f = fopen(args.output_file, "wb");
        if (f == NULL) {
            printf("Error: Could not open output file.\n");
            exit(EXIT_FAILURE);
        }

        fwrite(hidden_bmp, 1, bmp_size, f);
        fclose(f);
    } else {
        printf("Error: Could not hide message.\n");
        exit(EXIT_FAILURE);
    }

    free(bitmap_data);
    free(message);
}

void steg_extract(struct args args) {
    FILE* bitmap_file = fopen(args.bitmap_file, "rb");
    if (bitmap_file == NULL) {
        printf("Error: Could not open bitmap file.\n");
        exit(EXIT_FAILURE);
    }

    int bmp_size = utl_filesize(bitmap_file);
    char* bitmap_data = utl_filecontent(bitmap_file, bmp_size);
    struct bmp_header bitmap_header = bmp_get_header(bitmap_data);

    fclose(bitmap_file);

    if (bitmap_header.bits_per_pixel != 24) {
        printf("Error: Bitmap must be 24 bits per pixel.\n");
        exit(EXIT_FAILURE);
    }
    if (bitmap_header.compression != BI_RGB) {
        printf("Error: Bitmap must not be compressed.\n");
        exit(EXIT_FAILURE);
    }

    char* message = obf_reveal(bitmap_data, bmp_size, args.steg_algo);
    if (!message) {
        printf("Error: Could not reveal message.\n");
        exit(EXIT_FAILURE);
    }

    if (args.password) {
        int len = strlen(message);
        char* decrypted_message = decrypt(message, args.encryption_algo, args.mode, args.password, len, &len);
        if (decrypted_message) {
            free(message);
            message = decrypted_message;
        } else {
            printf("Decryption failed.\n");
            free(message);
            exit(EXIT_FAILURE);
        }
    }

    unsigned int size = 0;
    for (int i = 0; i < 4; i++)
        size |= (unsigned char)message[i] << (24 - (i * 8));

    char *content = message + 4;
    char* ext = content + size;
    if (ext[0] != '.') {
        printf("Error: Invalid file extension.\n");
        exit(EXIT_FAILURE);
    }

    char output_file_name[strlen(args.output_file) + strlen(ext)];
    strcpy(output_file_name, args.output_file);
    strcat(output_file_name, ext);

    FILE* output_file = fopen(output_file_name, "wb");
    if (output_file == NULL) {
        printf("Error: Could not open output file.\n");
        exit(EXIT_FAILURE);
    }

    fwrite(content, 1, size, output_file);
    fclose(output_file);

    free(bitmap_data);
    free(message);
}
