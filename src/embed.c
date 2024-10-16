#include "embed.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encrypt.h"
#include "hide.h"
#include "utils.h"

void embed(struct args args) {
    FILE *input_file = fopen(args.input_file, "rb");
    if (input_file == NULL) {
        printf("Error: Could not open input file.\n");
        exit(EXIT_FAILURE);
    }

    int size = get_file_size(input_file) - 1;
    char *ext = get_file_extension(args.input_file);
    int ext_len = strlen(ext);
    char *content = get_file_content(input_file, size);

    int len = 4 + size + ext_len;
    char *message = malloc(len + 1);

    sprintf(message, "%.*d", 4, size);
    memcpy(message + 4, content, size);
    memcpy(message + 4 + size, ext, ext_len);
    message[len] = '\0';

    free(content);
    fclose(input_file);

    if (args.password) {
        char *encrypted_message = encrypt(message, args.encryption_algo, args.mode, args.password, len, &len);
        if (encrypted_message) {
            free(message);
            message = encrypted_message;
        } else {
            printf("Encryption failed.\n");
            free(message);
            exit(EXIT_FAILURE);
        }
        /*
        char *decrypted_message = decrypt(message, args.encryption_algo, args.mode, args.password, len, &len);
        if (decrypted_message) {
            free(message);
            message = decrypted_message;
        } else {
            printf("Decryption failed.\n");
            free(message);
            exit(EXIT_FAILURE);
        }
        */
    }

    FILE* bitmap_file = fopen(args.bitmap_file, "rb");
    if (bitmap_file == NULL) {
        printf("Error: Could not open bitmap file.\n");
        exit(EXIT_FAILURE);
    }
    int bmp_size = get_file_size(bitmap_file);

    char* bitmap_data = (char*)malloc(bmp_size);
    fread(bitmap_data, 1, bmp_size, bitmap_file);
    fclose(bitmap_file);

    hide(message, bitmap_data, bmp_size, args.steg_algo, args.output_file);

    // puts(message);

    free(bitmap_data);
    free(message);
}
