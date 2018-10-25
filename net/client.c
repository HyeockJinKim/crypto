#define BLOCK_SIZE 128
#define KEY_SIZE 8

#include <openssl/aes.h>
#include "chat.h"

TALK	msg;

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

void encrypt_decrypt(unsigned char *input, unsigned char *out) {
    state aes_state; // state 만들어 둔 후 init 해가면서 새로운 값을 쓸 수 있게 만듦
    // 데이터가 256까지만 받기 때문에 나누어두지 않음
    init_block(&aes_state, iv);
    AES_ctr128_encrypt(input, out, MSG_SIZE, &key, aes_state.ivec, aes_state.ecount, &aes_state.num);
}

void * send_msg(void * arg)
{
	int sock = (int)arg;

	while(1) {
		fgets(msg.msg, MSG_SIZE, stdin);
		printf("\033[F\033[J");
		fprintf(stdout, "[%s] %s", msg.name, msg.msg);
        encrypt_decrypt(msg.msg, msg.msg); // 암호화 한다.
		write(sock, (void *)&msg, sizeof(TALK)); // 그리고 보낸다.
        memset(msg.msg, 0x0, MSG_SIZE); // 그리고 초기화
	}
}
// gcc -o client client.c -L/usr/local/lib -L./ -pthread -lcrypto -static
void * recv_msg(void * arg)
{
	int sock = (int)arg;
	int str_len;

	while(1) {
		TALK tmp;
		str_len = read(sock, (void *)&tmp, sizeof(TALK));
		if ( -1 == str_len ) { // 통신이 끊겼을 경우를 위한 것
			return (void *)1;
		}
        encrypt_decrypt(tmp.msg, tmp.msg); // 복호화 한다.
		fprintf(stdout, "[%s] %s", tmp.name, tmp.msg); // 그리고 적는다.
	}
}

int main(int argc, char *argv[])
{
	int					serv_sock;
	struct sockaddr_in	serv_addr;

	pthread_t			send_thread, recv_thread;
	void *				thread_result;

	// SIG HANDLER
	signal(SIGINT, (void *)handler);

	if ( argc != 4 ) {
		fprintf(stderr, "Usage : %s <ip> <port> <name>\n", argv[0]);
		exit(1);
	}
	sprintf(msg.name, "%s", argv[3]);

    fprintf(stderr, ">> Input Key : ");
    fgets(encrypt_key, KEY_SIZE, stdin); // 시작할 때에 key를 설정함
    AES_set_encrypt_key(encrypt_key, KEY_SIZE, &key);

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if ( -1 == serv_sock ) {
		fprintf(stderr, "[!] ERROR : Socket()\n");
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr(argv[1]);
	serv_addr.sin_port			= htons(atoi(argv[2]));

	if ( -1 == connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) ) {
		fprintf(stderr, "[!] ERROR : Connect()\n");
	}

	pthread_create(&send_thread, NULL, send_msg, (void *)serv_sock);
	pthread_create(&recv_thread, NULL, recv_msg, (void *)serv_sock);

	pthread_join(send_thread, &thread_result);
	pthread_join(recv_thread, &thread_result);

	close(serv_sock);

	return 0;
}
