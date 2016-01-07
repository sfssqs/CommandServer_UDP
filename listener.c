#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 		5000            	    // The port which is communicate with server
#define BACKLOG 	10
#define LENGTH  	512             	// Buffer length

void startServer();
void sender(char *ipAddr, char* command);
void *receiver();

int sockfd;
pthread_t recvThread;

int main() {
	///////////////////////////////////  fd opened
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("ERROR: Failed to obtain Socket Despcritor.\n");
		return -1;
	}

	printf("OK: Obtain Socket Despcritor sucessfully.\n");

	////////////////////////////////////
	//  Start receiver server
	startServer();

	////////////////////////////////////    fd closed
	close(sockfd);

	return (0);
}

void startServer() {
	if (pthread_create(&recvThread, NULL, receiver, NULL) != 0) {
		printf("receiver thread create failed!/n");
	} else {
		printf("receiver thread created!/n");
	}

	if (recvThread == 0)
		return;

	pthread_join(recvThread, NULL);
	printf("recvThread ended!");
}

void *receiver() {
	int nsockfd;                    	// New Socket file descriptor
	int sin_size;                      	// to store struct size
	char revbuf[LENGTH];          	// Send buffer
	struct sockaddr_in addr_local;
	struct sockaddr_in addr_remote;

	/* Get the Socket file descriptor */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("ERROR: Failed to obtain Socket Despcritor.\n");
		return NULL;
	} else {
		printf("OK: Obtain Socket Despcritor sucessfully.\n");
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
		return NULL;
	} else {
		printf("OK: Bind the Port %d sucessfully.\n", PORT);
	}

	while (1) {
		sin_size = sizeof(struct sockaddr);
		if (recvfrom(sockfd, revbuf, LENGTH, 0,
				(struct sockaddr *) &addr_remote, &sin_size) == -1) {
			printf("ERROR!\n");
		} else {
			printf("OK: %s.\n", revbuf);

			// TODO: send command
			char remoteIp[20];
			inet_ntop(AF_INET, (void*)&addr_remote.sin_addr, remoteIp, 16);
			printf("ipAddress : %s | integer : %d\n", remoteIp, addr_remote.sin_addr.s_addr);

			sender(remoteIp, revbuf);
			// TODO: test code begin
//			char* ipAddr = "192.168.0.100";
//			char* command = "server command";
//			printf("test");
//			sender(ipAddr, command);
			// test code end
		}
	}

	pthread_exit(NULL);
}

void sender(char *ipAddr, char* command) {
	int num;                       				// Counter of received bytes
	char sdbuf[LENGTH];             			// Receive buffer
	struct sockaddr_in addr_remote;    			// Host address information
	char sdstr[] = { "EasyARM-iMX283 UDP Experiment." };

	/* Fill the socket address struct */
	addr_remote.sin_family = AF_INET;          		// Protocol Family
	addr_remote.sin_port = htons(PORT);          		// Port number
	inet_pton(AF_INET, ipAddr, &addr_remote.sin_addr); 	// Net Address
	memset(addr_remote.sin_zero, 0, 8);              // Flush the rest of struct

	/* Try to connect the server */
	memset(sdbuf, 0, LENGTH);
	num = sendto(sockfd, sdstr, strlen(sdstr), 0,
			(struct sockaddr *) &addr_remote, sizeof(struct sockaddr_in));
	if (num < 0) {
		printf("ERROR: Failed to send your data!\n");
	} else {
		printf("OK: Sent to %s total %d bytes !\n", ipAddr, num);
		printf("OK: Sent command : %s!\n", command);
	}
}

