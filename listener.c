#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 		5000
#define LENGTH  	10

void init();
void startServer();
void sender(struct sockaddr_in addr_remote, unsigned char* cmd, int len);
void *receiver();
void printip(struct sockaddr_in addr);

int sockfd;
pthread_t recvThread;

struct sockaddr_in addr_local;
struct sockaddr_in addr_remote;

struct sockaddr_in addr_remote_box;  // T-BOX info
struct sockaddr_in addr_remote_phone; // Phone info

int main() {
	init();
	startServer();
	close(sockfd);
	return (0);
}

void init() {
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("ERROR: Failed to obtain Socket Descriptor.\n");
		return;
	} else {
		printf("OK: Obtain Socket Descriptor successfully.\n");
	}

	/* Fill the local socket address struct */
	addr_local.sin_family = AF_INET;           		// Protocol Family
	addr_local.sin_port = htons(PORT);         		// Port number
	addr_local.sin_addr.s_addr = htonl(INADDR_ANY);  // AutoFill local address
	memset(addr_local.sin_zero, 0, 8);          	// Flush the rest of struct

	/*  Blind a special Port */
	if (bind(sockfd, (struct sockaddr*) &addr_local, sizeof(struct sockaddr))
			== -1) {
		printf("ERROR: Failed to bind Port %d.\n", PORT);
		return;
	} else {
		printf("OK: Bind the Port %d successfully.\n", PORT);
	}
}

void startServer() {
	if (pthread_create(&recvThread, NULL, receiver, NULL) != 0) {
		printf("OK: Receiver thread create failed!\n");
	} else {
		printf("OK: Receiver thread created!\n");
	}

	if (recvThread == 0)
		return;

	pthread_join(recvThread, NULL);
	printf("Attention:  RecvThread Ended!\n");
}

void *receiver() {
	int sin_size;                      	// to store struct size
	unsigned char revbuf[LENGTH];          	// Send buffer

	while (1) {
		sin_size = sizeof(struct sockaddr);
		memset(revbuf, 0, LENGTH);

		printf("\n");
		printf("=========================\n");
		printf("OK: Listening ... \n");

		if (recvfrom(sockfd, revbuf, LENGTH, 0,
				(struct sockaddr *) &addr_remote, &sin_size) == -1) {
			printf("ERROR: Receive data error!\n");
		} else
			printf("OK: Receive data: 0x%X 0x%X\n", revbuf[0], revbuf[1]);

		printip(addr_remote);

		// From T-BOX
		if (revbuf[0] == 0xD0) {
			memset(&addr_remote_box, 0, sizeof(struct sockaddr_in));
			memcpy(&addr_remote_box, &addr_remote, sizeof(struct sockaddr_in));
		}

		// From Phone
		else if (revbuf[0] == 0xD1) {
			memset(&addr_remote_phone, 0, sizeof(struct sockaddr_in));
			memcpy(&addr_remote_phone, &addr_remote,
					sizeof(struct sockaddr_in));

			unsigned char cmd = revbuf[1];
			sender(addr_remote_box, &cmd, sizeof(cmd));
		}
	}

	pthread_exit(NULL);
}

void printip(struct sockaddr_in addr) {
	char ip[20];
	int port;
	inet_ntop(AF_INET, (void*) &addr.sin_addr, ip, 16);
	port = addr.sin_port;
	printf("OK: Print IP %s : %d\n", ip, port);
}

void sender(struct sockaddr_in addr_remote, unsigned char* cmd, int len) {
	int num;                       				// Counter of received bytes

	char remoteIp[20];
	int remotePort;
	inet_ntop(AF_INET, (void*) &addr_remote.sin_addr, remoteIp, 16);
	remotePort = addr_remote.sin_port;

	num = sendto(sockfd, cmd, len, 0, (struct sockaddr *) &addr_remote,
			sizeof(struct sockaddr_in));

	if (num < 0) {
		printf("ERROR: Failed to send your data!\n");
	} else {
		printf("OK: Sent to %s : %d total %d bytes !\n", remoteIp, remotePort,
				num);
		printf("OK: Sent command : 0x%X\n", cmd[0]);
	}
}

