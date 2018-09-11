//
// Created by khz on 18. 9. 10.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decryptCaesar(char *fileName, int key, long fSize);
void encryptCaesar(char *fileName, int key, long fSize);

long getFileSize(char * fileName);

int main() {
    int mode, key;
    char fileName[256];
    char string[32];
	printf(">> Input File Name : ");
	scanf("%s", fileName);

	printf(">> Input Mode [ 0: Encrypt, 1 : Decrypt ] : ");
	scanf("%s", string); // 암호화할지 복호화할 지 결정
	mode = (int) strtol(string, NULL, 10); // int 값을 strtol을 이용해 값을 받음

	printf(">> Input Key : ");
    scanf("%s", string);
    key = (int) strtol(string, NULL, 10);

    long fSize = getFileSize(fileName); // 파일 크기를 받는다.
    printf("File Size = %li\n", fSize);

	if (mode == 1) // 읽을 파일과 key, 파일 크기를 인자로 넘겨준다.
        decryptCaesar(fileName, key, fSize);
	else if (mode == 0)
        encryptCaesar(fileName, key, fSize);
	else
	    printf("Mode Error!!\n");

    return 0;
}


void decryptCaesar(char *fileName, int key, long fSize) {
    FILE * fp_s = fopen(fileName, "rb");
    key %= 26;
    key += 26;
    key %= 26;
    key -= 26; // decrypt는 역으로 계산해야 하므로
    if (fp_s) {
        FILE * fp_d = fopen("decrypt.txt", "wb");
        char *alpha = malloc(sizeof(char)*fSize); // 크기만큼의 공간 할당
        fread(alpha, sizeof(char)*fSize, 1, fp_s);
        // 한번에 크기만큼을 읽어 옴
        for (int i = 0; i < fSize; ++i) {
            if (*(alpha+i) != ' ' && *(alpha+i) != '\n') {
                if (*(alpha+i) >= 'a') {  // 소문자 처리
                    *(alpha+i) -= key+'a';
                    *(alpha+i) %= 26;
                    *(alpha+i) += 'a';
                } else {  // 대문자 처리
                    *(alpha+i) -= key+'A';
                    *(alpha+i) %= 26;  // % 연산을 통해 알파벳 영역을 유지
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

void encryptCaesar(char *fileName, int key, long fSize) {
    FILE * fp_s = fopen(fileName, "rb");
    key %= 26; // 26 이상의 값에 대해 일반화
    key += 26;
    key %= 26; // 음수에 대해 일반화
    if (fp_s) {
        FILE * fp_d = fopen("encrypt.txt", "wb");
        char *alpha = malloc(sizeof(char)*fSize); // 크기만큼의 공간 할당
        fread(alpha, sizeof(char)*fSize, 1, fp_s);
        // 한번에 크기만큼을 읽어 옴
        for (int i = 0; i < fSize; ++i) {
            if (*(alpha+i) != ' ' && *(alpha+i) != '\n') {
                if (*(alpha+i) >= 'a') {  // 소문자 처리
                    *(alpha+i) += key-'a';
                    *(alpha+i) %= 26;
                    *(alpha+i) += 'a';
                } else {  // 대문자 처리
                    *(alpha+i) += key-'A';
                    *(alpha+i) %= 26;  // % 연산을 통해 알파벳 영역을 유지
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

