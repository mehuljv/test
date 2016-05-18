#include <string.h>
#include <stdlib.h>
#include "queue_eventfd.h"
#include <sys/eventfd.h>

//enqueue/dequeue implementation.
int enqueue_element(struct queue *q, void *data)
{
	struct queue_element *tmp = malloc(sizeof(struct queue_element));
	if(!tmp)
		return ENQUEUE_FAILED;
	
	pthread_mutex_lock(q->lock);
	q->tail->data = data;
	q->tail->next = tmp;
	q->enqueued++;
	q->tail = tmp;
	pthread_mutex_unlock(q->lock);
	
	//Signal that we have writtent the data.
	eventfd_write(q->eventfd, 1ULL);

	if((q->enqueued - q->dequeued) > q->threshold)
		return ENQUEUE_FILLING_FAST;
	else
		return ENQUEUE_SUCCESS;
}

void *dequeue_element(struct queue *q)
{
	struct queue_element *tmp;
	void *data;
	uint64_t junk;

	if(q->head == q->tail)
		return NULL;
	
	//Read the enqueued data.	
	if(eventfd_read(q->eventfd, &junk) == -1)
		return NULL;

	pthread_mutex_lock(q->lock);
	tmp = q->head;
	data = tmp->data;
	q->head = tmp->next;
	q->dequeued++;
	pthread_mutex_unlock(q->lock);
	
	free(tmp);
	return data;
}



//Lockless enqueue/dequeue implementation.
int enqueue_element_lockless(struct queue *q, void *data)
{
	struct queue_element *tmp = malloc(sizeof(struct queue_element));
	if(!tmp)
		return ENQUEUE_FAILED;
	
	q->tail->data = data;
	q->tail->next = tmp;
	q->enqueued++;
	q->tail = tmp;
	//Signal that we have written the data.
	eventfd_write(q->eventfd, 1ULL);

	if((q->enqueued - q->dequeued) > q->threshold)
		return ENQUEUE_FILLING_FAST;
	else
		return ENQUEUE_SUCCESS;
}

void *dequeue_element_lockless(struct queue *q)
{
	struct queue_element *tmp;
	void *data;
	uint64_t junk;

	if(q->head == q->tail)
		return NULL;
	//Read the enqueued data.	
	eventfd_read(q->eventfd, &junk);
	tmp = q->head;
	data = tmp->data;
	q->head = tmp->next;
	free(tmp);
	q->dequeued++;
	return data;
}


struct queue *create_queue(int flag, int threshold, int id)
{
	struct queue *q;

	if(flag != FLAG_QUEUE_WITHOUT_LOCK && flag != FLAG_QUEUE_WITH_LOCK)
		return NULL;

	q = malloc(sizeof(struct queue));
	if(!q)
		return NULL;
	memset(q, 0, sizeof(struct queue));
	q->threshold = threshold;
	q->index = id;

	//Validae flag.
	if(flag == FLAG_QUEUE_WITH_LOCK){
		q->enqueue = enqueue_element;
		q->dequeue = dequeue_element;
		q->lock = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(q->lock, NULL);
	}else {
		q->enqueue = enqueue_element_lockless;
		q->dequeue = dequeue_element_lockless;
	}
	q->head = q->tail = malloc(sizeof(struct queue_element));
	
	if(!q->head)
		goto exit1;
	
	//Create eventfd.
	q->eventfd = eventfd(0, EFD_NONBLOCK|EFD_SEMAPHORE);
	if(q->eventfd == -1)
		goto exit2;

	return q;

exit2: 
	free(q->head);
exit1:
	free(q);
	return NULL;	
}

void delete_queue(struct queue *q)
{
	struct queue_element *tmp;

	while(q->head != NULL){
		tmp = q->head->next;
		free(q->head);
		q->head = tmp;
	}
	close(q->eventfd);
	free(q);
	return;
}
