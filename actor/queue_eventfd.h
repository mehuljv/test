#ifndef __QUEUE_H
#define __QUEUE_H
#include <pthread.h>
#include "list.h"
struct queue_element
{
	void *data;
	struct list_head list;
};

struct queue
{
	unsigned long long enqueued;
	unsigned long long dequeued;
	pthread_mutex_t lock;	
	int (*enqueue)(struct queue *, void *data);	
	void *(*dequeue)(struct queue *);
	int eventfd;
	int threshold;
	struct list_head list;
};

/*
Creates and initializes the queue. 
Parameters: 
flag: 
	FLAG_QUEUE_WITHOUT_LOCK - Use this if queue has single producer/consumer. 
	FLAG_QUEUE_WITH_LOCK - Use this if queue has multiple producer/consumer.
threshold:
	Sets the queue threshold value. 'enqueue' returns FAST_FILLING if number of already enqueued elements reaches this threshold, caller
	can use this value to create back-pressure.
Return : Returns queue.
*/
#define FLAG_QUEUE_WITHOUT_LOCK 1
#define FLAG_QUEUE_WITH_LOCK	2
struct queue *create_queue(int flag, int threshold, int id);

//Return Value On Enqueue Operation.
#define ENQUEUE_SUCCESS	1
#define ENQUEUE_FILLING_FAST 2
#define ENQUEUE_FAILED	3

#endif
