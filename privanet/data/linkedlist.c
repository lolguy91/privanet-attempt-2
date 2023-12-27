#include <stdlib.h>
#include <stdio.h>

#include "linkedlist.h"

struct linked_list linked_list_create()
{
	struct linked_list new_list;
	new_list.head = NULL;
	new_list.length = 0;

	return new_list;
}

void linked_list_destroy(struct linked_list *list)
{
	for (int i = 0; i < list->length; i++) {
		linked_list_remove_node(list, 0);
	}
}

void linked_list_insert(struct linked_list *list, int index, void *data, unsigned long size)
{
	struct node *new_node = (struct node *)malloc(sizeof(struct node));
	*new_node = node_create(data, size);

	if (index == 0) {
		new_node->next = list->head;
		list->head = new_node;
	} else {
		struct node *cur = linked_list_iterate(list, index - 1);
		new_node->next = cur->next;
		cur->next = new_node;
	}

	list->length += 1;
}

void linked_list_remove_node(struct linked_list *list, int index)
{
	if (index == 0) {
		struct node *node = list->head;
		if (node) {
			list->head = node->next;
			node_destroy(node);
		}
	} else {
		struct node *cur = linked_list_iterate(list, index - 1);
		struct node *node = cur->next;
		cur->next = node->next;
		node_destroy(node);
	}

	list->length -= 1;
}

void *linked_list_retrieve(struct linked_list *list, int index)
{
	struct node *cur = linked_list_iterate(list, index);

	// NOTE: Is this needed?
	if (cur) {
		return cur->data;
	} else {
		return NULL;
	}
}

struct node *linked_list_iterate(struct linked_list *list, int index)
{
	if (index < 0 || index >= list->length) {
		return NULL;
	}

	struct node *cur = list->head;
	for (int i = 0; i < index; i++) {
		cur = cur->next;
	}

	return cur;
}
