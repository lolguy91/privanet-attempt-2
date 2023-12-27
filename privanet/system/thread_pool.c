#include <stdlib.h>
#include <stdio.h>

#include "thread_pool.h"

struct thread_pool thread_pool_create(int num_threads)
{
	struct thread_pool new_pool;
	new_pool.num_threads = num_threads;
	new_pool.active = 1;
	new_pool.work = queue_create();
	new_pool.lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	new_pool.signal = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
	new_pool.pool = malloc(sizeof(pthread_t[num_threads]));

	pthread_mutex_lock(&new_pool.lock);
	for (int i = 0; i < num_threads; i++) {
		pthread_create(&new_pool.pool[i], NULL, __thread_pool_generic, &new_pool);
	}
	pthread_mutex_unlock(&new_pool.lock);

	return new_pool;
}

struct thread_job thread_job_create(void *(*job)(void *arg), void *arg)
{
	struct thread_job new_job;
	new_job.job = job;
	new_job.arg = arg;

	return new_job;
}

void thread_pool_add_work(struct thread_pool *thread_pool, struct thread_job thread_job)
{
	pthread_mutex_lock(&thread_pool->lock);
	queue_push(&thread_pool->work, &thread_job, sizeof(thread_job));
	pthread_cond_signal(&thread_pool->signal);
	pthread_mutex_unlock(&thread_pool->lock);
}

void thread_pool_destroy(struct thread_pool *thread_pool)
{
	thread_pool->active = 0;
	for (int i = 0; i < thread_pool->num_threads; i++) {
		pthread_cond_signal(&thread_pool->signal);
	}
	for (int i = 0; i < thread_pool->num_threads; i++) {
		pthread_join(thread_pool->pool[i], NULL);
	}
	
	free(thread_pool->pool);
	queue_destroy(&thread_pool->work);
}

void *__thread_pool_generic(void *arg)
{
	struct thread_pool *thread_pool = (struct thread_pool *)arg;

	while (thread_pool->active == 1) {
		pthread_mutex_lock(&thread_pool->lock);
		pthread_cond_wait(&thread_pool->signal, &thread_pool->lock);

		struct thread_job thread_job = *(struct thread_job *)queue_peek(&thread_pool->work);

		queue_pop(&thread_pool->work);
		pthread_mutex_unlock(&thread_pool->lock);

		if (thread_job.job) {
			thread_job.job(thread_job.arg);
		}
	}

	return NULL;
}