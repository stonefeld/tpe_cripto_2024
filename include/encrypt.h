#ifndef ENCRYPT_H
#define ENCRYPT_H

#include "args.h"

char* encrypt(char* message, enum tipo_enc encryption_algo, enum tipo_cb mode, char* password, int size, int* encsize);
char* decrypt(char* message, enum tipo_enc encryption_algo, enum tipo_cb mode, char* password, int size, int* decsize);

#endif
