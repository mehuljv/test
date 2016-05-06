#include <string.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include "common.h"

//enqueue/dequeue implementation.
static int enqueue_element(struct queue *q, void *data)
{
	struct queue_element *tmp = malloc(sizeof(struct queue_element));
	if(!tmp)
		return ENQUEUE_FAILED;

	tmp->data = data;
	
	pthread_mutex_lock(&q->lock);
	list_add_tail(&tmp->list, &q->list);
	q->enqueued++;
	pthread_mutex_unlock(&q->lock);
	
	//Signal that we have writtent the data.
	eventfd_write(q->eventfd, 1ULL);

	if((q->enqueued - q->dequeued) > q->threshold)
		return ENQUEUE_FILLING_FAST;
	else
		return ENQUEUE_SUCCESS;
}

static void *dequeue_element(struct queue *q)
{
	struct queue_element *work;
	void *data;
	uint64_t junk;

	pthread_mutex_lock(&q->lock);

	if(list_empty(&q->list)){
		pthread_mutex_unlock(&q->lock);
		return NULL;
	}
	work = list_first_entry(&q->list, struct queue_element, list);
	list_del(&work->list);
	q->dequeued++;
	pthread_mutex_unlock(&q->lock);

	//Read the enqueued data.	
	eventfd_read(q->eventfd, &junk);

	data = work->data;
	free(work);
	return data;
}

struct queue *create_queue(int flag, int threshold, int eventfd)
{
	struct queue *q;

	if(flag != FLAG_QUEUE_WITHOUT_LOCK && flag != FLAG_QUEUE_WITH_LOCK)
		return NULL;

	q = malloc(sizeof(struct queue));
	if(!q)
		return NULL;

	memset(q, 0, sizeof(struct queue));
	q->threshold = threshold;
	q->eventfd = eventfd;

	//Validae flag.
	if(flag == FLAG_QUEUE_WITH_LOCK){
		q->enqueue = enqueue_element;
		q->dequeue = dequeue_element;
		pthread_mutex_init(&q->lock, NULL);
	}else {
		//q->enqueue = enqueue_element_lockless;
		//q->dequeue = dequeue_element_lockless;
	}
	INIT_LIST_HEAD(&q->list);
	return q;
}

void delete_queue(struct queue *q)
{
	//TODO:
	return;
}
