#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_

#include "../data/queue.h"

#include <pthread.h>

struct thread_job {
	void *(*job)(void *arg);
	void *arg;
};

struct thread_pool {
	int num_threads;
	int active;
	struct queue work;
	pthread_t *pool;
	pthread_mutex_t lock;
	pthread_cond_t signal;
};

struct thread_pool thread_pool_create(int num_threads);
struct thread_job thread_job_create(void *(*job)(void *arg), void *arg);

void thread_pool_add_work(struct thread_pool *thread_pool, struct thread_job thread_job);

void thread_pool_destroy(struct thread_pool *thread_pool);

void *__thread_pool_generic(void *arg);

#endif /* __THREAD_POOL_H_ */