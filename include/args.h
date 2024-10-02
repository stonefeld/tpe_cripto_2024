#ifndef ARGS_H
#define ARGS_H

enum tipo_cb { ECB = 0, CFB, OFB, CBC };
enum tipo_enc { AES128 = 0, AES192, AES256, DES3 };
enum tipo_steg { LSB1 = 0, LSB4, LSBI };

struct args {
    int embed_flag;
    int extract_flag;
    char *input_file;
    char *bitmap_file;
    char *output_file;
    enum tipo_steg steg_algo;
    enum tipo_enc encryption_algo;
    enum tipo_cb mode;
    char *password;
};

struct args parse_args(int argc, char **argv);

#endif
