#ifndef __P2P_H_
#define __P2P_H_

#include <unistd.h>

#include "../data/linkedlist.h"
#include "network.h"

struct p2p {
	struct server server;
	struct linked_list known_hosts;

	int domain;
	int service;
	int protocol;
	int port;
	u_long interface;

	void *(*server_func)(void *arg);
	void *(*client_func)(void *arg);
};

struct p2p p2p_create(int domain, int service, int protocol, int port, u_long interface);

void p2p_user_portal(struct p2p *p2p);

#endif /* __P2P_H_ */