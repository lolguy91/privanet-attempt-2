#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "../privanet.h"

#include "../data/linkedlist.h"

#include "network.h"
#include "p2p.h"

struct p2p p2p_create(int domain, int service, int protocol, int port, u_long interface)
{
	struct p2p new_p2p;
	new_p2p.domain = domain;
	new_p2p.service = service;
	new_p2p.protocol = protocol;
	new_p2p.port = port;
	new_p2p.interface = interface;

	new_p2p.known_hosts = linked_list_create();
	
	// TODO: this is temporary so we don't have to run multiple instances
	// of this program to test it
	linked_list_insert(&new_p2p.known_hosts, 0, "127.0.0.1", 10);

	new_p2p.server_func = network_server_no_multithreading;
	new_p2p.client_func = network_client;

	return new_p2p;
}

void p2p_user_portal(struct p2p *p2p)
{
	pthread_t server_thread;
	pthread_create(&server_thread, NULL, p2p->server_func, p2p);

	p2p->client_func(p2p);
}