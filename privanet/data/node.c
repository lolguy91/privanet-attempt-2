#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "node.h"

struct node node_create(void *data, unsigned long size)
{
	if (size < 1) {
		printf("[!] Invalid node size!\n");
		exit(-67);
	}

	struct node new_node;

	new_node.data = malloc(size);
	memcpy(new_node.data, data, size);

	new_node.next = NULL;
	new_node.prev = NULL;

	return new_node;
}

void node_destroy(struct node *node)
{
	free(node->data);
	free(node);
}