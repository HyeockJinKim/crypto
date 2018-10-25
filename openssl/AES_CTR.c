//
// Created by khz on 18. 10. 15.
//
#include <openssl/aes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTES_SIZE 1024
#define KEY_SIZE 128
unsigned char iv[8]={0,};
typedef struct _state {
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned int num;
    unsigned char ecount[AES_BLOCK_SIZE];
} state;
unsigned char encrypt_key[8];

AES_KEY key;

void init_block(state *aes_state, const unsigned char *iv){
    aes_state->num = 0;
    memset(aes_state->ecount, 0, AES_BLOCK_SIZE);
    memset(aes_state->ivec+8 , 0, 8); // 다음 9~16비트까지의 값은 0으로 초기화해둠
    memcpy(aes_state->ivec, iv, 8); // 초기 8비트는 입력받은 iv값으로 받고
}

void encrypt_decrypt(unsigned char *input, unsigned char *out, int bytes_read){

    int i=0;
    int remain=0;

    AES_set_encrypt_key(encrypt_key, KEY_SIZE, &key);
    state aes_state; // state 만들어 둔 후 init 해가면서 새로운 값을 쓸 수 있게 만듦
    if( bytes_read < BYTES_SIZE){ // BYTE SIZE 보다 작은 경우는 한 블록만 암호화하면 끝
        init_block(&aes_state, iv);
        AES_ctr128_encrypt(input, out, bytes_read, &key, aes_state.ivec, aes_state.ecount, &aes_state.num);
        return;
    }
    // loop block size  = [ BYTES_SIZE ]
    for(i=BYTES_SIZE; i <= bytes_read ;i+=BYTES_SIZE){ // 클 경우는 여러 블록으로 나누어서 암호화하게 됨
        init_block(&aes_state, iv);
        AES_ctr128_encrypt(input, out, BYTES_SIZE, &key, aes_state.ivec, aes_state.ecount, &aes_state.num);
        input+=BYTES_SIZE;
        out+=BYTES_SIZE;
    }

    remain = bytes_read % BYTES_SIZE; // 마지막 블록은 남은 byte를 계산해서 암호
    if( remain != 0 ){
        init_block(&aes_state, iv);
        AES_ctr128_encrypt(input, out, remain, &key, aes_state.ivec, aes_state.ecount, &aes_state.num);
    }

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

void writeFile(unsigned char * msg, long fSize, int mode) {
    FILE * fp_d;
    if (mode == 1)
        fp_d = fopen("AES_CTR_encrypt.txt", "wb"); // mode에 따른 파일 이름
    else
        fp_d = fopen("AES_CTR_decrypt.txt", "wb");
    fwrite(msg, sizeof(char)*fSize, 1, fp_d);
    fclose(fp_d);
}

int main(int argc, char *argv[]) {
    int mode;
    char fileName[256];
    char string[32];

    printf(">> Input File Name : ");
    scanf("%s", fileName);

    printf(">> Input Mode [ 1: Encrypt, 2: Decrypt ] : ");
    scanf("%s", string);
    mode = (int) strtol(string, NULL, 10);

    long fSize = getFileSize(fileName);
    printf(">> Input Key : ");
    scanf("%s", encrypt_key);
    unsigned char *msg = readFile(fileName, fSize);
    if (msg) {
        encrypt_decrypt(msg, msg, (int) fSize);
        writeFile(msg, fSize, mode);
        free(msg);
    } else {
        printf("No File Data");
    }

    return 0;
}