#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PT_IDENT "PT_IDENT"
#define PT_RESP_IDENT_STRING "PT_RESP_IDENT"

#define PORT 9999

void *beam_listener(void *arg)
{
	int sock;
	int yes = 1;
	struct sockaddr_in broadcast_addr;
	struct sockaddr_in server_addr;
	socklen_t addr_len;
	int count;
	int ret;
	fd_set readfd;
	char buffer[1024];
	int i;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket error");
		exit(-1);
	}

	ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));
	if (ret == -1) {
		perror("setsockopt error");
		exit(-2);
	}

	addr_len = sizeof(struct sockaddr_in);

	memset((void*)&broadcast_addr, 0, addr_len);
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	broadcast_addr.sin_port = htons(PORT);

	while (1) {
		ret = sendto(sock, PT_IDENT, strlen(PT_IDENT), 0, (struct sockaddr*) &broadcast_addr, addr_len);

		FD_ZERO(&readfd);
		FD_SET(sock, &readfd);

		ret = select(sock + 1, &readfd, NULL, NULL, NULL);

		if (ret > 0) {
			if (FD_ISSET(sock, &readfd)) {
				count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_len);
				printf("\trecvmsg is %s\n", buffer);
				if (strncmp(buffer, PT_RESP_IDENT_STRING, strlen(PT_RESP_IDENT_STRING))) {
					printf("\tfound server IP is %s, Port is %d\n", inet_ntoa(server_addr.sin_addr),htons(server_addr.sin_port));
				}
			}
		}
	}

	return NULL;
}

void *discovery_broadcast_beam(void *arg)
{
	int sock;
	int yes = 1;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	socklen_t addr_len;
	int count;
	int ret;
	fd_set readfd;
	char buffer[1024];

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("sock error\n");
		exit(-1);
	}

	addr_len = sizeof(struct sockaddr_in);

	memset((void*)&server_addr, 0, addr_len);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	ret = bind(sock, (struct sockaddr*)&server_addr, addr_len);
	if (ret < 0) {
		perror("bind error\n");
		exit(-1);
	}
	
	while (1) {
		FD_ZERO(&readfd);
		FD_SET(sock, &readfd);

		ret = select(sock+1, &readfd, NULL, NULL, 0);
		if (ret > 0) {
			if (FD_ISSET(sock, &readfd)) {
				count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_len);
				if (strstr(buffer, PT_IDENT)) {
					printf("\nNode connection information:\n\t IP: %s, Port: %d\n", 
					inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
					memcpy(buffer, PT_RESP_IDENT_STRING, strlen(PT_RESP_IDENT_STRING)+1);
					count = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, addr_len);
				}
			}
		}
	}
}