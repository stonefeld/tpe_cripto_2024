#include "args.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct args args_parse(int argc, char **argv) {
    int embed_flag = 0, extract_flag = 0;
    char *input_file = NULL, *bitmap_file = NULL, *output_file = NULL, *password = NULL;
    enum tipo_steg steg_algo = -1;
    enum tipo_enc encryption_algo = AES128;
    enum tipo_cb mode = CBC;

    struct option long_options[] = {
        {"embed", no_argument, &embed_flag, 1},
        {"extract", no_argument, &extract_flag, 2},
        {"in", required_argument, 0, 3},
        {"out", required_argument, 0, 4},
        {"steg", required_argument, 0, 5},
        {"pass", required_argument, 0, 6},
        {0, 0, 0, 0},
    };

    int opt, option_index = 0;

    while ((opt = getopt_long(argc, argv, "p:a:m:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 3: {
                input_file = optarg;
            } break;

            case 'p': {
                bitmap_file = optarg;
            } break;

            case 4: {
                output_file = optarg;
            } break;

            case 5: {
                if (strcmp(optarg, "LSB1") == 0) {
                    steg_algo = LSB1;
                } else if (strcmp(optarg, "LSB4") == 0) {
                    steg_algo = LSB4;
                } else if (strcmp(optarg, "LSBI") == 0) {
                    steg_algo = LSBI;
                } else {
                    printf("Error: Algoritmo de esteganografía inválido.\n");
                    exit(EXIT_FAILURE);
                }
            } break;

            case 'a': {
                if (strcmp(optarg, "aes128") == 0) {
                    encryption_algo = AES128;
                } else if (strcmp(optarg, "aes192") == 0) {
                    encryption_algo = AES192;
                } else if (strcmp(optarg, "aes256") == 0) {
                    encryption_algo = AES256;
                } else if (strcmp(optarg, "3des") == 0) {
                    encryption_algo = DES3;
                } else {
                    printf("Error: Algoritmo de cifrado inválido.\n");
                    exit(EXIT_FAILURE);
                }
            } break;

            case 'm': {
                if (strcmp(optarg, "ecb") == 0) {
                    mode = ECB;
                } else if (strcmp(optarg, "cfb") == 0) {
                    mode = CFB;
                } else if (strcmp(optarg, "ofb") == 0) {
                    mode = OFB;
                } else if (strcmp(optarg, "cbc") == 0) {
                    mode = CBC;
                } else {
                    printf("Error: Modo de cifrado inválido.\n");
                    exit(EXIT_FAILURE);
                }
            } break;

            case 6: {
                password = optarg;
            } break;

            case '?': {
                fprintf(stderr,
                        "Uso: %c <--embed || --extract> --in <file> -p <bitmapfile> --out <bitmapfile> --steg <LSB1 | "
                        "LSB4 | LSBI> [-a <aes128 | aes192 | aes256 | 3des>] [-m <ecb | cfb | ofb | cbc>] [--pass "
                        "<password>]\n",
                        opt);
                exit(EXIT_FAILURE);
            } break;
        }
    }

    if (!embed_flag && !extract_flag) {
        printf("Error: Debe especificar --embed o --extract.\n");
        exit(EXIT_FAILURE);
    }

    if (embed_flag && extract_flag) {
        printf("Error: No puede usar --embed y --extract al mismo tiempo.\n");
        exit(EXIT_FAILURE);
    }

    if ((embed_flag && input_file == NULL) || bitmap_file == NULL || output_file == NULL || steg_algo < 0) {
        if (embed_flag && input_file == NULL)
            printf("Error: Falta el archivo de entrada.\n");
        else if (bitmap_file == NULL)
            printf("Error: Falta el archivo BMP portador.\n");
        else if (output_file == NULL)
            printf("Error: Falta el archivo BMP de salida.\n");
        else if (steg_algo < 0)
            printf("Error: Falta el algoritmo de esteganografía.\n");
        exit(EXIT_FAILURE);
    }

    return (struct args){
        .action = embed_flag ? EMBED : EXTRACT,
        .input_file = input_file,
        .bitmap_file = bitmap_file,
        .output_file = output_file,
        .steg_algo = steg_algo,
        .encryption_algo = encryption_algo,
        .mode = mode,
        .password = password,
    };
}
