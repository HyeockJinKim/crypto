//
// Created by khz on 18. 11. 3.
//
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <string.h>

#define KEY_LENGTH 1024
#define PUB_EXP 3

#define RSA_PKCS1_PADDING 1
#define RSA_SSLV23_PADDING 2
#define RSA_NO_PADDING 3
#define RSA_PKCS1_0AEP_PADDING 4
#define RSA_X931_PADDING 5
int prk_len, puk_len;
char *pri_key;
char *pub_key;

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

int main() {
    // key 생성
    RSA *keypair = RSA_generate_key(KEY_LENGTH, PUB_EXP, NULL, NULL);
    BIO *prk = BIO_new(BIO_s_mem());
    BIO *puk = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(prk, keypair, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_RSAPublicKey(puk, keypair); // 개인키와 공개키를 적음

    prk_len = BIO_pending(prk);
    puk_len = BIO_pending(puk);

    pri_key = calloc(prk_len+1, 1);
    pub_key = calloc(puk_len+1, 1); // +1 은 '\0'를 위해

    BIO_read(prk, pri_key, prk_len);
    BIO_read(puk, pub_key, puk_len);

    pri_key[prk_len] = '\0';
    pub_key[puk_len] = '\0';

    FILE *key_file;
    key_file = fopen("prkey.pem", "wb");
    fprintf(key_file, "%s", pri_key); // '\0'가 없었다면 메모리 오버플로우
    fclose(key_file);
    key_file = fopen("pukey.pem", "wb");
    fprintf(key_file, "%s", pub_key); // '\0'가 없었다면 메모리 오버플로우
    fclose(key_file);

    key_file = fopen("rsa.key", "w");
    RSA_print_fp(key_file, keypair, 0);
    fclose(key_file);

    int flen = (int) getFileSize("in.txt");
    unsigned char *input = (unsigned char *) readFile("in.txt", flen);
    unsigned char *output = malloc(RSA_size(keypair));
    int t = 0;
    printf("plain.txt::\n");
    printf("%s", input);
    printf("\n\n");

    t =RSA_public_encrypt(flen, input, output, keypair, RSA_PKCS1_PADDING); // public key로 암호화 함
    printf("Epuk[M]::\n");
    printf("%s", output);
    printf("\n\n");

    RSA_private_decrypt(t, output, output, keypair, RSA_PKCS1_PADDING); // 해당 값은 private key 로 복호화된다.
    printf("Dprk[Epuk[M]]::\n");
    output[flen] = '\0'; // 마지막 값에 \0을 두어 이후 값을 무시시킬 수 있게 함
    printf("%s", output);
    printf("\n\n");

    RSA_private_encrypt(flen, input, output, keypair, RSA_PKCS1_PADDING); // private key 로 암호화함
    printf("Eprk[M]::\n");
    printf("%s", output);
    printf("\n\n");

    t = RSA_public_decrypt(t, output, output, keypair, RSA_PKCS1_PADDING); //  그 값은 public key로 복호화해야함
    output[flen] = '\0';
    printf("Dpuk[Eprk[M]]::\n");
    printf("%s", output);
    printf("\n\n");
    printf("%d", t);

}