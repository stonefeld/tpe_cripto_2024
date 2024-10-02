#include "args.h"
#include <getopt.h>
#include <bits/getopt_core.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

void parse_args(int argc, char **argv) {

    int embed_flag = 0;
    int extract_flag = 0;
    char *input_file = NULL;
    char *bitmap_file = NULL;
    char *output_file = NULL;
    char *steg_algo = NULL;
    char *encryption_algo = NULL;
    char *mode = NULL;
    char *password = NULL;

    struct option long_options[] = {
        {"embed", no_argument, &embed_flag, 1},
        {"extract", no_argument, &extract_flag, 2},
        {"in", required_argument, 0, 3},
        {"out", required_argument, 0, 4},
        {"steg", required_argument, 0, 5},
        {"pass", required_argument, 0, 6},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "p:a:m:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 1:
                embed_flag = 1;
                break;
            case 2:
                extract_flag = 1;
                break;
            case 3:
                input_file = optarg;
                break;
            case 'p':
                bitmap_file = optarg;
                break;
            case 4:
                output_file = optarg;
                break;
            case 5:
                steg_algo = optarg;
                break;
            case 'a':
                encryption_algo = optarg;
                break;
            case 'm':
                mode = optarg;
                break;
            case 6:
                password = optarg;
                break;
            case '?':
                fprintf(stderr, "Uso: %c <--embed || --extract> --in <file> --p <bitmapfile> --out <bitmapfile> --steg <LSB1 | LSB4 | LSBI> [--a <aes128 | aes192 | aes256 | 3des>] [--m <ecb | cfb | ofb | cbc>] [--pass <password>]\n", opt);
                exit(EXIT_FAILURE);
        }
    }

    if (!embed_flag && !extract_flag) {
        fprintf(stderr, "Error: Debe especificar --embed o --extract.\n");
        exit(EXIT_FAILURE);
    }

    if (embed_flag && extract_flag) {
        fprintf(stderr, "Error: No puede usar --embed y --extract al mismo tiempo.\n");
        exit(EXIT_FAILURE);
    }

    if (input_file == NULL || bitmap_file == NULL || output_file == NULL || steg_algo == NULL) {
        if (input_file == NULL) {
            fprintf(stderr, "Error: Falta el archivo de entrada.\n");
        } else if (bitmap_file == NULL) {
            fprintf(stderr, "Error: Falta el archivo BMP portador.\n");
        } else if (output_file == NULL) {
            fprintf(stderr, "Error: Falta el archivo BMP de salida.\n");
        } else if (steg_algo == NULL) {
            fprintf(stderr, "Error: Falta el algoritmo de esteganografía.\n");
        }
        exit(EXIT_FAILURE);
    }

    if (embed_flag) {
        printf("Modo: Embed\n");
    } else if (extract_flag) {
        printf("Modo: Extract\n");
    }
    printf("Archivo de entrada: %s\n", input_file);
    printf("Archivo BMP portador: %s\n", bitmap_file);
    printf("Archivo BMP de salida: %s\n", output_file);
    printf("Algoritmo de esteganografía: %s\n", steg_algo);
    if (encryption_algo) {
        printf("Algoritmo de encriptación: %s\n", encryption_algo);
    }
    if (mode) {
        printf("Modo de encriptación: %s\n", mode);
    }
    if (password) {
        printf("Password: %s\n", password);
    }

}
