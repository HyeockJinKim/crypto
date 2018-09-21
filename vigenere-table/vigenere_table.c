//
// Created by khz on 18. 9. 11.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decryptVigenereTable(char *fileName, const int *key, const int * numTable, long fSize);
void encryptVigenereTable(char *fileName, const int *key, const int * numTable, long fSize);
long getFileSize(char * fileName);

void setKey(char * key, int kSize, int * k, long fSize);
void setKeyTable(const char * keyTable, int * numTable, int mode);

int main() {
    int mode;
    char fileName[256];
    char string[32];
    char *key;
    char keyTable[] = "koegbpdwitncmfryqzulashxvj";
    int numTable[26];
    printf(">> Input File Name : ");
    scanf("%s", fileName);

    printf(">> Input Mode [ 0: Encrypt, 1 : Decrypt ] : ");
    scanf("%s", string);
    mode = (int) strtol(string, NULL, 10);

    long fSize = getFileSize(fileName);
    key = malloc(sizeof(char)*(fSize+1));

    printf(">> Input Key : ");
    scanf("%s", key);
    int * k = malloc(sizeof(int) * fSize);

    setKey(key, (int) strlen(key), k, fSize); // 입력받은 key 에 대한 설정 (파일크기만큼)
    setKeyTable(keyTable, numTable, mode); // 입력받은 keyTable을 숫자로 변경

    printf("File Size = %li\n", fSize);


    if (mode == 1)
        decryptVigenereTable(fileName, k, numTable, fSize);
    else if (mode == 0)
        encryptVigenereTable(fileName, k, numTable, fSize);
    else
        printf("Mode Error!!\n");

    free(key);
    free(k);
    return 0;
}

void decryptVigenereTable(char *fileName, const int *key, const int * numTable, long fSize) {
    FILE * fp_s = fopen(fileName, "rb");
    if (fp_s) {
        FILE * fp_d = fopen("decrypt.txt", "wb");
        char *alpha = malloc(sizeof(char)*fSize); // 크기만큼의 공간 할당
        fread(alpha, sizeof(char)*fSize, 1, fp_s);
        int offset = 0, check = 0;
        // 한번에 크기만큼을 읽어 옴
        for (int i = 0; i < fSize; ++i) {
            if (*(alpha + i) >= 'a' && *(alpha + i) <= 'z') {  // 소문자만 처리
                int t = *(numTable + (*(alpha + i) -'a')); // 테이블에 맞게 변경한 후에
                t += offset; // 횟수만큼 돌려주면 된다.
                t %= 26; // 돌리는 효과를 내기 위한 나머지 연산

                *(alpha+i) = (char) (t + 'a');
                offset += *(key+i-check);
                offset %= 26; // offset 이 26이상인 경우 의미 없기 때문에 나머지 연산
            } else { // 입력받은 값이 소문자가 아닌 경우 key의 순서를 맞추기 위해
                ++check;
            }
        }
        fwrite(alpha, sizeof(char)*fSize, 1, fp_d);
        free(alpha);
        fclose(fp_s);
        fclose(fp_d);
    } else
        printf("File Read Fail!!\n");

}

void encryptVigenereTable(char *fileName, const int *key, const int * numTable, long fSize) {
    FILE * fp_s = fopen(fileName, "rb");
    if (fp_s) {
        FILE * fp_d = fopen("encrypt.txt", "wb");
        char *alpha = malloc(sizeof(char)*fSize); // 크기만큼의 공간 할당
        fread(alpha, sizeof(char)*fSize, 1, fp_s);
        // 한번에 크기만큼을 읽어 옴
        int offset = 0, check = 0;
        for (int i = 0; i < fSize; ++i) {
            if (*(alpha + i) >= 'a' && *(alpha + i) <= 'z') { // 소문자만 처리
                int t = (26+*(alpha + i) -'a')-offset; // 암호화시에는 돌린 후에
                t %= 26;
                t = *(numTable + t); // 테이블에서 꺼내야 맞다

                *(alpha+i) = (char) (t + 'a');
                offset += *(key+i-check);
                offset %= 26;
            } else { // 입력받은 값이 소문자가 아닌 경우 key의 순서를 맞추기 위해
                ++check;
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

void setKey(char * key, int kSize, int * k, long fSize) {
    int len = 0;
    int t = (int) strtol(key, NULL, 10);
    *(k+len) = t;
    ++len;
    printf("%d\n", t);
    for (int i = 0; i < kSize; ++i) {
        if (*(key+i) == ',') {
            t = (int) strtol(key+i+1, NULL, 10); // , 뒤의 값들을 숫자로 변경해준다
            *(k+len) = t;
            ++len;
            printf("%d\n", t); // 확인용 출력
        }
    }
    for (int i = len; i < (int)fSize; ++i) {
        *(k+i) = *(k+(i%len)); // 그 값들을 순서대로 file 크기만큼 반복한다
    }
}

void setKeyTable(const char * keyTable, int * numTable, int mode) {
    if (mode) { // Decrypt
        for (int i = 0; i < 26; ++i) { // encrypt 때와 decrypt 때를 다르게 하여 쓰기 편하게 함
            *(numTable + (*(keyTable+i) -'a')) = i; // 역으로 찾을수 있게 함
        }
    } else { // Encrypt
        for (int i = 0; i < 26; ++i) {
            *(numTable + i) = *(keyTable+i) -'a'; // 암호테이블대로 읽게 함
        }
    }
}