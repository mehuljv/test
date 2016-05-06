#ifndef __ACTOR_H
#define __ACTOR_H
#include "list.h"

struct actor *create_actor(int id, char *name, void (*func) (void *, struct actor *, void *), void *args);

#define STATUS_BUSY	1
#define STATUS_FREE	0

struct actor
{
	char name[20];
	int id;
	int status;
	void (*func) (void *, struct actor *, void *);
	void *func_args;
	pthread_mutex_t lock;
	struct list_head list;
	struct queue *mailbox;
};
#endif
