#ifndef __NODE_H_
#define __NODE_H_

struct node {
	void *data;

	struct node *next;
	struct node *prev;
};

struct node node_create(void *data, unsigned long size);
void node_destroy(struct node *node);

#endif /* __NODE_H_ */