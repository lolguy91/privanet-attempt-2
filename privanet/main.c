#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include "discovery.h"

int main()
{
	pthread_t beam_listener_thread;
	pthread_t beam_broadcast_thread;
	pthread_create(&beam_listener_thread, NULL, beam_listener, NULL);
	pthread_create(&beam_broadcast_thread, NULL, discovery_broadcast_beam, NULL);
	while(1);
	return 0;
}