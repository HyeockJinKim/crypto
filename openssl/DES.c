//
// Created by khz on 18. 10. 2.
//

#include <openssl/opensslconf.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int DES_LONG;

typedef unsigned char DES_cblock[8];
typedef /* const */ unsigned char const_DES_cblock[8];

typedef struct DES_ks {
    union {
        DES_cblock cblock;
        /*
         * make sure things are correct size on machines with 8 byte longs
         */
        DES_LONG deslong[2];
    } ks[16];
} DES_key_schedule;

# define DES_ENCRYPT     1
# define DES_DECRYPT     0
//
#define BLOCK_SIZE 128
#define KEY_SIZE 8


void DES_ncbc_encrypt(const unsigned char *input, unsigned char *output,
                      long length, DES_key_schedule *schedule,
                      DES_cblock *ivec, int enc);

int DES_set_key_checked(const_DES_cblock *key, DES_key_schedule *schedule);
void DES_string_to_key(const char *str, DES_cblock *key);

unsigned int des(unsigned char * msg, unsigned char * key, unsigned int msg_len, int mode);
long getFileSize(char * fileName);
char *readFile(char * fileName, long fSize);
void writeFile(char * msg, long fSize, int mode);
// gcc -o test DES.c -lssl -lcrypto

int main () {
    int mode;
    char fileName[256];
    char string[32];
    char *key;

    printf(">> Input File Name : ");
    scanf("%s", fileName);

    printf(">> Input Mode [ 1: Encrypt, 2: Decrypt ] : ");
    scanf("%s", string);
    mode = (int) strtol(string, NULL, 10);

    long fSize = getFileSize(fileName);
    key = malloc(sizeof(char)*(fSize+1));
    printf(">> Input Key : ");
    scanf("%s", key);
    char * msg = readFile(fileName, fSize);
    if (msg) {
        int size = des(msg, key, fSize, mode);
        writeFile(msg, size, mode);
        free(msg);
    } else {
        printf("No File Data");
    }
}

unsigned int des(unsigned char * msg, unsigned char * key, unsigned int msg_len, int mode) {
    DES_key_schedule des_ks;
    DES_cblock des_key = {0, };
    DES_cblock  iv = {0, };

    unsigned int result, padding;
    unsigned char block_in[BLOCK_SIZE] = {0, };
    unsigned char block_out[BLOCK_SIZE] = {0, };

    DES_string_to_key(key, &des_key); // 키 생성
    DES_set_key_checked(&des_key, &des_ks); // 스케줄 생성

    memcpy(block_in, msg, msg_len);
    if (mode == 1) {
        if (msg_len < BLOCK_SIZE) {
            padding = BLOCK_SIZE - msg_len;
            block_in[BLOCK_SIZE-1] = padding;
        }

        DES_ncbc_encrypt(block_in, block_out, BLOCK_SIZE, &des_ks, &iv, DES_ENCRYPT);
        result = BLOCK_SIZE;

    } else if (mode == 2) {
        DES_ncbc_encrypt(block_in, block_out, BLOCK_SIZE, &des_ks, &iv, DES_DECRYPT);
        padding = block_out[BLOCK_SIZE-1];
        result = BLOCK_SIZE - padding;
    }
    memcpy(msg, block_out, BLOCK_SIZE);

    return result; // size 를 return 함
}

long getFileSize(char * fileName) {
    FILE * fp = fopen(fileName, "rb");
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size;
}

char *readFile(char * fileName, long fSize) {
    FILE * fp_s = fopen(fileName, "rb");
    if (fp_s) {
        char *alpha = malloc(sizeof(char) * fSize); // 크기만큼의 공간 할당
        fread(alpha, sizeof(char) * fSize, 1, fp_s); // 한번에 크기만큼을 읽어 옴
        return alpha;
    }
    return NULL;
}

void writeFile(char * msg, long fSize, int mode) {
    FILE * fp_d;
    if (mode == 1)
        fp_d = fopen("DES_encrypt.txt", "wb");
    else
        fp_d = fopen("DES_decrypt.txt", "wb");
    fwrite(msg, sizeof(char)*fSize, 1, fp_d);
    fclose(fp_d);
}