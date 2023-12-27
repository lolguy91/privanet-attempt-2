#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include "data/linkedlist.h"
#include "privanet.h"
#include "network.h"

int main()
{
	struct linked_list known_hosts = linked_list_create();
	linked_list_insert(&known_hosts, 0, "127.0.0.1", 10);

	pthread_t thread_server;

	// create server/client threads	
	pthread_create(&thread_server, NULL, network_server, &known_hosts);
	
	while(1) {
		char request[REQUEST_BUFFER_SIZE];
		memset(request, 0, REQUEST_BUFFER_SIZE);
		printf("REQUEST: ");
		fgets(request, REQUEST_BUFFER_SIZE, stdin);
		network_client(request, &known_hosts);
	}

	return 0;
}