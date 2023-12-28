#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#include "data/linkedlist.h"
#include "net/network.h"
#include "net/p2p.h"
#include "privanet.h"

int main()
{
	struct p2p p2p = p2p_create(AF_INET, SOCK_STREAM, 0, PRIVANET_PORT, INADDR_ANY);
	p2p_user_portal(&p2p);

	return 0;
}