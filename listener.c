#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>

#define PORT 		5000            	// The port which is communicate with server
#define BACKLOG 	10
#define LENGTH  	512             	// Buffer length

int main() {
	int sockfd;                        // Socket file descriptor
	int nsockfd;                    	// New Socket file descriptor
	int num;
	int sin_size;                      	// to store struct size
	char revbuf[LENGTH];          	// Send buffer
	struct sockaddr_in addr_local;
	struct sockaddr_in addr_remote;

	/* Get the Socket file descriptor */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("ERROR: Failed to obtain Socket Despcritor.\n");
		return (0);
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
		return (0);
	} else {
		printf("OK: Bind the Port %d sucessfully.\n", PORT);
	}

	sin_size = sizeof(struct sockaddr);
	if (num = recvfrom(sockfd, revbuf, LENGTH, 0,
			(struct sockaddr *) &addr_remote, &sin_size) == -1) {
		printf("ERROR!\n");
	} else {
		printf("OK: %s.\n", revbuf);
	}

	close(sockfd);
	return (0);
}

