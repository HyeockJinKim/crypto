#include <openssl/aes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 16
#define FREAD_COUNT 4096
#define KEY_BIT 256
#define IV_SIZE 16
#define RW_SIZE 1

AES_KEY aes_key;
unsigned char iv[IV_SIZE];
unsigned char key[32];

void encrypt_aes(char *input)
{
    int len;
    int padding_len;
    char buf[FREAD_COUNT+BLOCK_SIZE];

    FILE *fp=fopen(input, "rb");
    FILE *wfp=fopen("AES_CBC_encrypt.txt", "wb");

    memset(iv, 0, sizeof(iv)); // init iv
    AES_set_encrypt_key(key ,KEY_BIT ,&aes_key);
    while( len = fread( buf ,RW_SIZE ,FREAD_COUNT, fp) ){ // 파일에서 버퍼크기만큼씩 읽어서 처리
        if( FREAD_COUNT != len ){ // 4096보다 작은 경우 == 나머지 처리
            break;
        }

        AES_cbc_encrypt(buf ,buf ,len ,&aes_key ,iv ,AES_ENCRYPT);
        fwrite(buf ,RW_SIZE ,len ,wfp);
    }

    padding_len=BLOCK_SIZE - len % BLOCK_SIZE;  // 4096에 모자라서 padding 이 필요한 경우
    memset(buf+len, padding_len, padding_len);

    AES_cbc_encrypt(buf ,buf ,len+padding_len ,&aes_key, iv,AES_ENCRYPT); // ENCRYPT를 넣음
    fwrite(buf ,RW_SIZE ,len+padding_len ,wfp);

    fclose(wfp);
    fclose(fp);
}

void decrypt_aes(char *input)
{
    char buf[FREAD_COUNT+BLOCK_SIZE];
    int len;
    int total_size;
    int pass_len=0;
    int w_len=0;

    FILE *fp=fopen(input,"rb");
    FILE *wfp=fopen("AES_CBC_decrypt.txt","wb");

    memset(iv, 0, sizeof(iv)); // initial vector는 같아야함
    AES_set_decrypt_key(key ,KEY_BIT ,&aes_key);

    fseek(fp, 0, SEEK_END);
    total_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    while( len = fread( buf ,RW_SIZE ,FREAD_COUNT ,fp) ){
        pass_len += len;
        w_len = len;

        AES_cbc_encrypt(buf ,buf, len, &aes_key, iv, AES_DECRYPT); // DECRYPT를 넣어줌
        if( pass_len == total_size ){ // block 크기를 계산
            w_len = len - buf[len-1];
        }

        fwrite(buf ,RW_SIZE ,w_len ,wfp);
    }

    fclose(wfp);
    fclose(fp);
}

int main(int argc, char *args[])
{
    int mode;
    char fileName[256];
    char string[32];

    printf(">> Input File Name : ");
    scanf("%s", fileName);

    printf(">> Input Mode [ 1: Encrypt, 2: Decrypt ] : ");
    scanf("%s", string);
    mode = (int) strtol(string, NULL, 10);

    printf(">> Input Key : ");
    scanf("%s", key);
    if (mode == 1) {
        encrypt_aes(fileName);
    } else {
        decrypt_aes(fileName);
    }

    return 0;
}
