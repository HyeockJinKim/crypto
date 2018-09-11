//
// Created by khz on 18. 9. 11.
//

//
// Created by khz on 18. 9. 10.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void encryptDecryptXor(char *fileName, const char *key, long fSize, int mode);

long getFileSize(char * fileName);
void setKey(char * key, long size);

int main() {
    int mode;
    char fileName[256];
    char string[32];
    char *key;

    printf(">> Input File Name : ");
    scanf("%s", fileName);

    printf(">> Input Mode [ 0: Encrypt, 1 : Decrypt ] : ");
    scanf("%s", string);
    mode = (int) strtol(string, NULL, 10);

    long fSize = getFileSize(fileName);
    key = malloc(sizeof(char)*(fSize+1));
    printf(">> Input Key : ");
    scanf("%s", key);

    setKey(key, fSize);

    printf("File Size = %li\n", fSize);

    encryptDecryptXor(fileName, key, fSize, mode);

    free(key);
    return 0;
}

void encryptDecryptXor(char *fileName, const char *key, long fSize, int mode) {
    FILE * fp_s = fopen(fileName, "rb");
    if (fp_s) {
        FILE * fp_d;
        if (mode == 0) {
            fp_d = fopen("encrypt.txt", "wb");
        } else if (mode == 1) {
            fp_d = fopen("decrypt.txt", "wb");
        } else {
            printf("Mode Error!!\n");
            return ;
        }

        char *alpha = malloc(sizeof(char)*fSize); // 크기만큼의 공간 할당
        fread(alpha, sizeof(char)*fSize, 1, fp_s); // 한번에 크기만큼을 읽어 옴

        for (int i = 0; i < fSize; ++i) {
            *(alpha+i) ^= *(key+i); // 대응되는 Key 의 XOR 연산
        }

        fwrite(alpha, sizeof(char)*fSize, 1, fp_d);

        free(alpha);
        fclose(fp_s);
        fclose(fp_d);
    } else
        printf("File Read Fail!!\n");
}

long getFileSize(char * fileName) {
    FILE * fp = fopen(fileName, "rb");
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size;
}


void setKey(char * key, long size) {
    int len = (int) strlen(key);
    for (int i = len; i < size; ++i) {
        int index = i % len;
        *(key+i) = *(key+index);
    }
}

