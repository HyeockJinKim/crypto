//
// Created by khz on 18. 9. 11.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decryptVigenere(char *fileName, const char *key, long fSize);
void encryptVigenere(char *fileName, const char *key, long fSize);

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

    if (mode == 1)
        decryptVigenere(fileName, key, fSize);
    else if (mode == 0)
        encryptVigenere(fileName, key, fSize);
    else
        printf("Mode Error!!\n");

    free(key);
    return 0;
}

void decryptVigenere(char *fileName, const char *key, long fSize) {
    FILE * fp_s = fopen(fileName, "rb");
    if (fp_s) {
        FILE * fp_d = fopen("decrypt.txt", "wb");
        char *alpha = malloc(sizeof(char)*fSize); // 크기만큼의 공간 할당
        fread(alpha, sizeof(char)*fSize, 1, fp_s);
        // 한번에 크기만큼을 읽어 옴
        for (int i = 0; i < fSize; ++i) {
            if (*(alpha+i) != ' ' && *(alpha+i) != '\n') {
                if (*(alpha + i) >= 'a') {  // 소문자 처리
                    *(alpha + i) -= *(key + i) + 'a';
                    if (*(alpha + i) < -26) {
                        *(alpha + i) += 52;
                        *(alpha + i) += 'a';
                    } else if (*(alpha + i) < 0) {
                        *(alpha + i) += 26;
                        *(alpha + i) += 'A';
                    } else {
                        *(alpha + i) += 'a';
                    }
                } else {  // 대문자 처리
                    *(alpha + i) -= *(key + i) + 'A';
                    if (*(alpha + i) < -26) {
                        *(alpha + i) += 52;
                        *(alpha + i) += 'A';
                    } else if (*(alpha + i) < 0) {
                        *(alpha + i) += 26;
                        *(alpha + i) += 'a';
                    } else {
                        *(alpha + i) += 'A';
                    }
                }
            }
        }
        fwrite(alpha, sizeof(char)*fSize, 1, fp_d);
        free(alpha);
        fclose(fp_s);
        fclose(fp_d);
    } else
        printf("File Read Fail!!\n");

}

void encryptVigenere(char *fileName, const char *key, long fSize) {
    FILE * fp_s = fopen(fileName, "rb");
    if (fp_s) {
        FILE * fp_d = fopen("encrypt.txt", "wb");
        char *alpha = malloc(sizeof(char)*fSize); // 크기만큼의 공간 할당
        fread(alpha, sizeof(char)*fSize, 1, fp_s);
        // 한번에 크기만큼을 읽어 옴
        for (int i = 0; i < fSize; ++i) {
            if (*(alpha+i) != ' ' && *(alpha+i) != '\n') {
                if (*(alpha+i) >= 'a') {  // 소문자 처리
                    *(alpha+i) += *(key+i)-'a';
                    if (*(alpha+i) >= 52) {
                        *(alpha+i) %= 26;
                        *(alpha+i) += 'a';
                    } else if (*(alpha+i) >= 26){
                        *(alpha+i) %= 26;
                        *(alpha+i) += 'A';
                    } else {
                        *(alpha+i) += 'a';
                    }
                } else {  // 대문자 처리
                    *(alpha+i) += *(key+i)-'A';
                    if (*(alpha+i) >= 52) {
                        *(alpha+i) %= 26;
                        *(alpha+i) += 'A';
                    } else if (*(alpha+i) >= 26) {
                        *(alpha+i) %= 26;
                        *(alpha+i) += 'a';
                    }
                    else
                        *(alpha+i) += 'A';
                }
            }
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
    for (int i = 0; i < len; ++i) {
        if (*(key+i) >= 'a')
            *(key+i) += 26-'a';
        else
            *(key+i) -= 'A';
    }
    for (int i = len; i < size; ++i) {
        int index = i % len;
        *(key+i) = *(key+index);
    }
}

