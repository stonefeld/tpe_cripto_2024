#include "steganography.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cipher.h"
#include "obfuscate.h"
#include "utils.h"

void embed(struct args args) {
    FILE* input_file = fopen(args.input_file, "rb");
    if (input_file == NULL) {
        printf("Error: Could not open input file.\n");
        exit(EXIT_FAILURE);
    }

    int size = get_file_size(input_file) - 1;
    char* ext = get_file_extension(args.input_file);
    int ext_len = strlen(ext);
    char* content = get_file_content(input_file, size);

    int len = 4 + size + ext_len;
    char* message = malloc(len + 1);

    sprintf(message, "%.*d", 4, size);
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
    int bmp_size = get_file_size(bitmap_file);

    char* bitmap_data = get_file_content(bitmap_file, bmp_size);
    fclose(bitmap_file);

    char* hidden_bmp = hide(message, bitmap_data, bmp_size, args.steg_algo);

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

void extract(struct args args) {
    FILE* bitmap_file = fopen(args.bitmap_file, "rb");
    if (bitmap_file == NULL) {
        printf("Error: Could not open bitmap file.\n");
        exit(EXIT_FAILURE);
    }
    int bmp_size = get_file_size(bitmap_file);

    char* bitmap_data = get_file_content(bitmap_file, bmp_size);
    fclose(bitmap_file);

    char* message = reveal(bitmap_data, bmp_size, args.steg_algo);

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

    char size[4] = {message[0], message[1], message[2], message[3]};
    int wasa = atoi(size);

    puts(message);
    char content[wasa];
    memcpy(content, message + 4, wasa);

    char* ext = message + 4 + wasa;

    char output_file_name[strlen(args.output_file) + strlen(ext)];
    strcpy(output_file_name, args.output_file);
    strcat(output_file_name, ext);

    FILE* output_file = fopen(output_file_name, "wb");
    if (output_file == NULL) {
        printf("Error: Could not open output file.\n");
        exit(EXIT_FAILURE);
    }

    fwrite(content, 1, wasa, output_file);
    fclose(output_file);

    free(bitmap_data);
    free(message);
}
