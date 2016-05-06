#ifndef __HASH_H
#define __HASH_H
#include "list.h"

struct hash_table *create_hash_table(int num_buckets);
struct hash_list
{
	int eventfd;
	pthread_rwlock_t lock;
	struct list_head list;
};

struct hash_table
{
	int num_buckets;
	struct hash_list hlist[];
};
#endif
