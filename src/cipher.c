#include "cipher.h"

#include <openssl/bio.h>
#include <openssl/des.h>
#include <openssl/evp.h>
#include <string.h>

#define EXTRA_PADDING 1024

static const EVP_CIPHER* _get_evp_algorithm(enum tipo_enc encryption_algo, enum tipo_cb mode);

char* cip_encrypt(char* message, enum tipo_enc encryption_algo, enum tipo_cb mode, char* password, unsigned int size, unsigned int* encsize) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return NULL;

    EVP_CIPHER_CTX_init(ctx);

    const EVP_CIPHER* cipher = _get_evp_algorithm(encryption_algo, mode);
    if (!cipher)
        return NULL;

    unsigned char keylen = EVP_CIPHER_key_length(cipher);
    unsigned char ivlen = EVP_CIPHER_iv_length(cipher);

    unsigned char concat[keylen + ivlen];
    unsigned char salt[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    PKCS5_PBKDF2_HMAC(password, strlen(password), salt, sizeof(salt), 10000, EVP_sha256(), keylen + ivlen, concat);

    unsigned char* key = malloc(keylen);
    unsigned char* iv = malloc(ivlen);

    strncpy((char*)key, (char*)concat, keylen);
    strncpy((char*)iv, (char*)concat + keylen, ivlen);

    char* encrypted = calloc(1, size + EXTRA_PADDING);
    int outl = 0, templ = 0;

    if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    if (EVP_EncryptUpdate(ctx, (unsigned char*)encrypted, &outl, (unsigned char*)message, size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    if (EVP_EncryptFinal(ctx, (unsigned char*)encrypted + outl, &templ) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    *encsize = outl + templ;

    EVP_CIPHER_CTX_free(ctx);
    free(key);
    free(iv);

    return realloc(encrypted, *encsize);
}

char* cip_decrypt(char* message, enum tipo_enc encryption_algo, enum tipo_cb mode, char* password, unsigned int size, unsigned int* decsize) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return NULL;

    EVP_CIPHER_CTX_init(ctx);

    const EVP_CIPHER* cipher = _get_evp_algorithm(encryption_algo, mode);
    if (!cipher)
        return NULL;

    unsigned char keylen = EVP_CIPHER_key_length(cipher);
    unsigned char ivlen = EVP_CIPHER_iv_length(cipher);

    unsigned char concat[keylen + ivlen];
    unsigned char salt[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    PKCS5_PBKDF2_HMAC(password, strlen(password), salt, sizeof(salt), 10000, EVP_sha256(), keylen + ivlen, concat);

    unsigned char* key = malloc(keylen);
    unsigned char* iv = malloc(ivlen);

    strncpy((char*)key, (char*)concat, keylen);
    strncpy((char*)iv, (char*)concat + keylen, ivlen);

    char* decrypted = calloc(1, size);
    int outl = 0, templ = 0;

    if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    if (EVP_DecryptUpdate(ctx, (unsigned char*)decrypted, &outl, (unsigned char*)message, size) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    if (EVP_DecryptFinal(ctx, (unsigned char*)decrypted + outl, &templ) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    *decsize = outl + templ;

    EVP_CIPHER_CTX_free(ctx);
    free(key);
    free(iv);

    return realloc(decrypted, *decsize);
}

static const EVP_CIPHER* _get_evp_algorithm(enum tipo_enc encryption_algo, enum tipo_cb mode) {
    OpenSSL_add_all_ciphers();

    switch (encryption_algo) {
        case AES128: {
            switch (mode) {
                case ECB: return EVP_aes_128_ecb();
                case CFB: return EVP_aes_128_cfb8();
                case OFB: return EVP_aes_128_ofb();
                case CBC: return EVP_aes_128_cbc();
            }
        } break;

        case AES192: {
            switch (mode) {
                case ECB: return EVP_aes_192_ecb();
                case CFB: return EVP_aes_192_cfb8();
                case OFB: return EVP_aes_192_ofb();
                case CBC: return EVP_aes_192_cbc();
            }
        } break;

        case AES256: {
            switch (mode) {
                case ECB: return EVP_aes_256_ecb();
                case CFB: return EVP_aes_256_cfb8();
                case OFB: return EVP_aes_256_ofb();
                case CBC: return EVP_aes_256_cbc();
            }
        } break;

        case DES3: {
            switch (mode) {
                case ECB: return EVP_des_ede3_ecb();
                case CFB: return EVP_des_ede3_cfb8();
                case OFB: return EVP_des_ede3_ofb();
                case CBC: return EVP_des_ede3_cbc();
            }
        } break;
    }

    return NULL;
}
