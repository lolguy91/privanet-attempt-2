struct queue queue_create()
{
	struct queue new_queue;
	new_queue.list = linked_list_create();

	return new_queue;
}

void queue_destroy(struct queue *queue)
{
	linked_list_destroy(&queue->list);
}

void queue_push(struct queue *queue, void *ata, unsigned long size)
{
	linked_list_insert(&queue->list, queue->list.length, data, size);
}

void *queue_peek(struct queue *queue)
{
	return linked_list_retrieve(&queue->list, 0);
}

void queue_pop(struct queue *queue)
{
	linked_list_remove(&queue->list, 0);
}