#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	if (sem_init(&q->sem_mutex, 0, 1) != 0) {
		printf("queue_init: sem_init(mutex) failed\n");
		free(q);
		abort();
	}

	if (sem_init(&q->sem_empty, 0, max_count) != 0) {
		printf("queue_init: sem_init(empty) failed\n");
		sem_destroy(&q->sem_mutex);
		free(q);
		abort();
	}

	if (sem_init(&q->sem_full, 0, 0) != 0) {
		printf("queue_init: sem_init(full) failed\n");
		sem_destroy(&q->sem_empty);
		sem_destroy(&q->sem_mutex);
		free(q);
		abort();
	}

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		sem_destroy(&q->sem_full);
		sem_destroy(&q->sem_empty);
		sem_destroy(&q->sem_mutex);
		free(q);
		abort();
	}

	return q;
}

void queue_destroy(queue_t *q) {
	pthread_cancel(q->qmonitor_tid);
	pthread_join(q->qmonitor_tid, NULL);
	
	qnode_t *current = q->first;
	while (current != NULL) {
		qnode_t *next = current->next;
		free(current);
		current = next;
	}
	
	sem_destroy(&q->sem_full);
	sem_destroy(&q->sem_empty);
	sem_destroy(&q->sem_mutex);
	
	free(q);
}

int queue_add(queue_t *q, int val) {
	q->add_attempts++;

	qnode_t *new = malloc(sizeof(qnode_t));
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		abort();
	}
	new->val = val;
	new->next = NULL;

	sem_wait(&q->sem_empty);

	sem_wait(&q->sem_mutex);

	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	q->add_count++;

	sem_post(&q->sem_mutex);

	// sem_full += 1
	sem_post(&q->sem_full);

	return 1;
}

int queue_get(queue_t *q, int *val) {
	q->get_attempts++;

	sem_wait(&q->sem_full);

	sem_wait(&q->sem_mutex);

	qnode_t *tmp = q->first;
	*val = tmp->val;
	q->first = q->first->next;

	q->count--;
	q->get_count++;

	sem_post(&q->sem_mutex);

	free(tmp);

	sem_post(&q->sem_empty);

	return 1;
}

void queue_print_stats(queue_t *q) {
	sem_wait(&q->sem_mutex);
	
	int count = q->count;
	long add_attempts = q->add_attempts;
	long get_attempts = q->get_attempts;
	long add_count = q->add_count;
	long get_count = q->get_count;
	
	sem_post(&q->sem_mutex);
	
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		count,
		add_attempts, get_attempts, add_attempts - get_attempts,
		add_count, get_count, add_count - get_count);
}