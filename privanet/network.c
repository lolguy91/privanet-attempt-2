#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "network.h"
#include "data/linkedlist.h"

struct server network_create_server(int domain, int service, int protocol, u_long interface, int port, int backlog)
{
	struct server new_server;

	new_server.domain = domain;
	new_server.service = service;
	new_server.protocol = protocol;
	new_server.interface = interface;
	new_server.port = port;
	new_server.backlog = backlog;

	new_server.address.sin_family = domain;
	new_server.address.sin_port = htons(port);
	new_server.address.sin_addr.s_addr = htonl(interface);

	new_server.socket = socket(domain, service, protocol);
	if (new_server.socket == 0) {
		perror("sock connect");
		exit(-1);
	}

	if ((bind(new_server.socket, (struct sockaddr *)&new_server.address, sizeof(new_server.address))) < 0) {
		perror("sock bind");
		exit(-2);
	}

	if (listen(new_server.socket, new_server.backlog) < 0) {
		perror("listen start");
		exit(-3);
	}

	return new_server;
}

struct client network_create_client(int domain, int service, int protocol, int port, u_long interface)
{
	struct client new_client;
	new_client.domain = domain;
	new_client.port = port;
	new_client.interface = interface;
	new_client.socket = socket(domain, service, protocol);

	return new_client;
}

void *network_server(void *hosts)
{
	struct linked_list *known_hosts = hosts;

	struct server server = network_create_server(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 9999, 20);
	struct sockaddr *address = (struct sockaddr *)&server.address;
	socklen_t address_length = (socklen_t)sizeof(server.address);

	printf("[*] Server started on port :%d\n", server.port);

	while (1) {
		int client = accept(server.socket, address, &address_length);
		char *client_addr = inet_ntoa(server.address.sin_addr);
		char request[256];
		memset(request, 0, 256);
		read(client, request, 255);
		printf("Received %s from %s\n", request, client_addr);
		close(client);

		// Do we already know the client?
		bool found = false;
		for (int i = 0; i < known_hosts->length && !found; i++) {
			if (strcmp(client_addr, linked_list_retrieve(known_hosts, i)) == 0) {
				found = true;
			}
		}

		if (!found) {
			linked_list_insert(known_hosts, known_hosts->length, client_addr, sizeof(client_addr));
		}
	}

	return NULL;
}

void network_client(char *request, struct linked_list *known_hosts)
{
	struct client client = network_create_client(AF_INET, SOCK_STREAM, 0, 9999, INADDR_ANY);

	// send the request to everyone we know
	for (int i = 0; i < known_hosts->length; i++) {
		__client_make_request(&client, linked_list_retrieve(known_hosts, i), request);
	}
}

char *__client_make_request(struct client *client, char *server_ip, char *request)
{
	struct sockaddr_in server_address;
	server_address.sin_family = client->domain;
	server_address.sin_port = htons(client->port);
	server_address.sin_addr.s_addr = (int)client->interface;

	inet_pton(client->domain, server_ip, &server_address.sin_addr);
	connect(client->socket, (struct sockaddr *)&server_address, sizeof(server_address));

	send(client->socket, request, strlen(request), 0);
	char *response = malloc(30000);
	read(client->socket, response, 30000);

	return response;
}