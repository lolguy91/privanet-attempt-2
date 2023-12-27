#ifndef __LINKEDLIST_H_
#define __LINKEDLIST_H_

#include "node.h"

struct linked_list {
	// first node in the chain
	struct node *head;

	// number of nodes in the chain
	int length;
};

struct linked_list linked_list_create(void);
void linked_list_destroy(struct linked_list *list);

void linked_list_insert(struct linked_list *list, int index, void *data, unsigned long size);
void linked_list_remove_node(struct linked_list *list, int index);
struct node *linked_list_iterate(struct linked_list *list, int index);

#endif /* __LINKEDLIST_H_ */