#ifndef CIPHER_H
#define CIPHER_H

#include "args.h"

char* cip_encrypt(char* message, enum tipo_enc encryption_algo, enum tipo_cb mode, char* password, unsigned int size, unsigned int* encsize);
char* cip_decrypt(char* message, enum tipo_enc encryption_algo, enum tipo_cb mode, char* password, unsigned int size, unsigned int* decsize);

#endif
