#include "common.h"

struct hash_table *create_hash_table(int num_buckets)
{
	struct hash_table *htable;
	struct hash_list *hlist;
	int i;
	int size = sizeof(struct hash_table) + num_buckets * sizeof(struct hash_list);
	Message("");
	
	htable = (struct hash_table *)malloc(size);
	Message("");
	memset(htable, 0, size);

	Message("");
	htable->num_buckets = num_buckets;
	Message("Num Buckets %d", num_buckets);
	for(i=0; i<num_buckets; i++){
		hlist = &htable->hlist[i];
		INIT_LIST_HEAD(&hlist->list);
		pthread_rwlock_init(&hlist->lock, NULL);
		hlist->eventfd = eventfd(0, EFD_NONBLOCK|EFD_SEMAPHORE);
		Message("hlist %d, fd %d", i, hlist->eventfd);
	}
	Message("");
	return htable;
}

int hash_compute(struct hash_table *htable, int key)
{
	return key % htable->num_buckets;
}

int hash_insert(struct hash_list *hlist, struct list_head *entry)
{
	Message("hlist %p, list %p, lock %p, fd %p", hlist, &hlist->list, &hlist->lock, &hlist->eventfd);
	list_add(entry, &hlist->list);
	return 0;
}

int hash_delete(struct hash_list *hlist, struct list_head *entry)
{
	//pthread_rwlock_wrlock(&hlist->lock);
	list_del(entry);
	//pthread_rwlock_unlock(&hlist->lock);
	return -1;
}

