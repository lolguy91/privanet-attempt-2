#ifndef __QUEUE_H_
#define __QUEUE_H_

#include "linkedlist.h"

struct queue {
	struct linked_list list;
};

struct queue queue_create();
void queue_destroy(struct queue *queue);
void queue_push(struct queue *queue, void *ata, unsigned long size);
void *queue_peek(struct queue *queue);
void queue_pop(struct queue *queue);

#endif /* __QUEUE_H_ */