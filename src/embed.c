#include "embed.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encrypt.h"

extern void mostrar_key(unsigned char key[], unsigned char keylen);

static int _get_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    // avoid reading the null or EOF terminator
    int size = ftell(f) - 1;
    fseek(f, 0, SEEK_SET);
    return size;
}

static char *_get_file_extension(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot;
}

static char *_get_file_content(FILE *f, int size) {
    fseek(f, 0, SEEK_SET);
    char *content = malloc(size);
    fread(content, 1, size, f);
    return content;
}

void embed(struct args args) {
    FILE *f = fopen(args.input_file, "rb");
    if (f == NULL) {
        printf("Error: Could not open file.\n");
        exit(EXIT_FAILURE);
    }

    int size = _get_file_size(f);
    char *ext = _get_file_extension(args.input_file);
    int ext_len = strlen(ext);
    char *content = _get_file_content(f, size);

    int len = 4 + size + ext_len;
    char *message = malloc(len + 1);

    sprintf(message, "%.*d", 4, size);
    memcpy(message + 4, content, size);
    memcpy(message + 4 + size, ext, ext_len);
    message[len] = '\0';

    free(content);
    fclose(f);

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
    }

    char *decrypted_message = decrypt(message, args.encryption_algo, args.mode, args.password, len, &len);
    if (decrypted_message) {
        free(message);
        message = decrypted_message;
    } else {
        printf("Decryption failed.\n");
        free(message);
        exit(EXIT_FAILURE);
    }

    free(message);
}
