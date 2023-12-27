#ifndef __NETWORK_H_
#define __NETWORK_H_

#include <netinet/in.h>
#include <sys/socket.h>

#include "data/linkedlist.h"

struct server {
	int domain;
	int service;
	int protocol;
	u_long interface;
	int port;
	int backlog;

	// IPv4
	struct sockaddr_in address;
	int socket;
};

struct client {
	int socket;

	int domain;
	int service;
	int protocol;
	int port;
	u_long interface;
};

struct server network_create_server(int domain, int service, int protocol, u_long interface, int port, int backlog);
struct client network_create_client(int domain, int service, int protocol, int port, u_long interface);

void *network_server(void *known_hosts);
void network_client(char *request, struct linked_list *known_hosts);

/****
 * PRIVATE FUNCTIONS
 ****/

char *__client_make_request(struct client *client, char *server_ip, char *request);
void *__network_server_loop(void *arg);

#endif /* __NETWORK_H_ */