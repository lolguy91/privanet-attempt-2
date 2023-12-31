#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "../privanet.h"
#include "network.h"
#include "p2p.h"

#include "../data/linkedlist.h"

#include "../system/thread_pool.h"

struct server_arg {
	struct server *server;
	struct linked_list *known_hosts;

	int client;
};

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

void *network_server(void *arg)
{
	struct server server = network_create_server(AF_INET, SOCK_STREAM, 0, INADDR_ANY, PRIVANET_PORT, 20);
	struct sockaddr *address = (struct sockaddr *)&server.address;
	socklen_t address_length = (socklen_t)sizeof(server.address);

	struct thread_pool thread_pool = thread_pool_create(50);

	printf("[*] Server started on port :%d\n", server.port);

	while(1) {
		struct server_arg server_loop_arg;
		server_loop_arg.client = accept(server.socket, address, &address_length);
		server_loop_arg.server = &server;
		server_loop_arg.known_hosts = arg;

		struct thread_job server_job = thread_job_create(__network_server_loop, &server_loop_arg);
		thread_pool_add_work(&thread_pool, server_job);
	}

	return NULL;
}

void *network_server_no_multithreading(void *arg)
{
	struct p2p *p2p = arg;
	struct server server = network_create_server(p2p->domain, p2p->service, p2p->protocol, p2p->interface, p2p->port, 20);
	struct sockaddr *address = (struct sockaddr *)&p2p->server.address;
	socklen_t address_length = (socklen_t)sizeof(server.address);

	struct thread_pool thread_pool = thread_pool_create(50);

	printf("[*] Server started on port :%d\n", server.port);

	while(1) {
		int client = accept(server.socket, address, &address_length);

		char request[REQUEST_BUFFER_SIZE];
		memset(request, 0, REQUEST_BUFFER_SIZE);
		read(client, request, REQUEST_BUFFER_SIZE);

		char *client_addr = inet_ntoa(p2p->server.address.sin_addr);

		printf("Received %s from %s\n", request, client_addr);
		close(client);

		// Do we already know the client?
		bool found = false;
		for (int i = 0; i < p2p->known_hosts.length && !found; i++) {
			if (strcmp(client_addr, linked_list_retrieve(&p2p->known_hosts, i)) == 0) {
				found = true;
			}
		}

		if (!found) {
			linked_list_insert(&p2p->known_hosts, p2p->known_hosts.length, client_addr, sizeof(client_addr));
		}
	}

	return NULL;
}

void *network_client(void *arg)
{
	struct p2p *p2p = arg;

	// send the request to everyone we know
	while (1) {
		struct client client = network_create_client(p2p->domain, p2p->service, p2p->protocol, p2p->port, p2p->interface);
		
		char request[REQUEST_BUFFER_SIZE];
		memset(request, 0, REQUEST_BUFFER_SIZE);
		fgets(request, 255, stdin);

		for (int i = 0; i < p2p->known_hosts.length; i++) {
			__client_make_request(&client, linked_list_retrieve(&p2p->known_hosts, i), request);
		}
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

void *__network_server_loop(void *arg)
{
	struct server_arg *server_arg = arg;

	char request[REQUEST_BUFFER_SIZE];
	memset(request, 0, REQUEST_BUFFER_SIZE);
	read(server_arg->client, request, REQUEST_BUFFER_SIZE);
	
	char *client_addr = inet_ntoa(server_arg->server->address.sin_addr);

	printf("Received %s from %s\n", request, client_addr);
	close(server_arg->client);

	// Do we already know the client?
	bool found = false;
	for (int i = 0; i < server_arg->known_hosts->length && !found; i++) {
		if (strcmp(client_addr, linked_list_retrieve(server_arg->known_hosts, i)) == 0) {
			found = true;
		}
	}

	if (!found) {
		linked_list_insert(server_arg->known_hosts, server_arg->known_hosts->length, client_addr, sizeof(client_addr));
	}

	return NULL;
}