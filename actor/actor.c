#include "common.h"

struct thread_info
{
	char name[20];
	int id;
	int epoll_fd;
	int max_efd;
};

static atomic_t actor_count;
static int g_actor_hash_buckets;
static int *g_epoll_fds;
static struct hash_table *g_hash_table;
static struct thread_info **g_thread_info;
static pthread_t *g_slave_threads;

struct thread_info *create_thread_info(char *name, int id, int epoll_fd, int max_efd)
{
	struct thread_info *thinfo = malloc(sizeof(struct thread_info));

	if(!thinfo)
		return NULL;
	thinfo->id = id;
	thinfo->epoll_fd = epoll_fd;
	thinfo->max_efd= max_efd;
	strcpy(thinfo->name, name);
	return thinfo;
}

void process_actor(struct actor *actor)
{
	struct queue *q;
	void *data;

	pthread_mutex_lock(&actor->lock);
	if(actor->status == STATUS_BUSY){
		//Somebody else is processing this actor. Skip.
		pthread_mutex_unlock(&actor->lock);
		return;
	}
	actor->status = STATUS_BUSY;
	pthread_mutex_unlock(&actor->lock);

	q = actor->mailbox;
	while(data = q->dequeue(q)){
		//Drain all jobs from this actor.
		actor->func(data, actor, actor->func_args);
	}
	actor->status = STATUS_FREE;
}

void *thread_pool_func(void *args)
{
	struct thread_info *thrinfo = (struct thread_info *)args;
	int efd = thrinfo->epoll_fd;
	int max_efd = thrinfo->max_efd;
	struct hash_list *hlist;
	struct actor *actor;
	struct epoll_event *events;
	int i = 0;
	int n = 0;
	struct list_head *pos;

	events = malloc(sizeof(struct epoll_event) * max_efd);
	while(1){
		n = epoll_wait(efd, events, max_efd, -1);
		for(i=0; i<n; i++){
			hlist = events[i].data.ptr;
			pthread_rwlock_rdlock(&hlist->lock);
			//Process all actors from this list.
			list_for_each(pos, &hlist->list){
				actor = list_entry(pos, struct actor, list);	
				process_actor(actor);
			}			
			pthread_rwlock_unlock(&hlist->lock);
		}
	}
}

void init_thread_pool(int num_threads, int num_efds)
{
	int i = 0, j = 0;
	int epoll_fd;
	struct epoll_event event;
	struct thread_info *thrinfo;
	char name[20];
	int fd;

	g_thread_info = (struct thread_info **)malloc(sizeof(struct thread_info *)*num_threads);
	g_slave_threads = (pthread_t *)malloc(sizeof(pthread_t)*num_threads);

	Message("num_threads %d, num_efds %d\n", num_threads, num_efds);
	for(i=0; i<num_threads; i++){
		epoll_fd = epoll_create1(0);
		for(j=0; j<num_efds; j++)
		{
			fd = g_hash_table->hlist[j].eventfd;
			event.data.ptr = &(g_hash_table->hlist[j]);
			event.events = EPOLLIN;
			epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
		}
		sprintf(name, "slave-%d", i);
		g_thread_info[i] = create_thread_info(name, i, epoll_fd, num_efds);
		pthread_create(&g_slave_threads[i], NULL, thread_pool_func, g_thread_info[i]);
	}
}

int actor_initialize_subsystem(int num_threads, int num_efds)
{
	Message("");
	g_hash_table = create_hash_table(num_efds);
	Message("");
	init_thread_pool(num_threads, num_efds);
	Message("");
}

static int get_unique_id()
{
	return atomic_add(1, &actor_count);
}

struct actor *create_actor(int id, char *name, void (*func) (void *, struct actor *, void *), void *args)
{
	int hlistid;
	struct hash_list *hlist;
	struct actor *actor = (struct actor *)malloc(sizeof(struct actor));
	memset(actor, 0, sizeof(struct actor));
	Message("");
	strncpy(actor->name, name, 20);
	actor->id = id;//get_unique_id();
	actor->func = func;	
	actor->func_args = args;
	Message("");
	pthread_mutex_init(&actor->lock, NULL);
	hlistid = hash_compute(g_hash_table, actor->id);
	hlist = &g_hash_table->hlist[hlistid];
	actor->mailbox = create_queue(FLAG_QUEUE_WITH_LOCK, 100, hlist->eventfd);
	Message("Actor%d-'s Queue %p", actor->id, actor->mailbox);
	hash_insert(hlist, &actor->list);
	Message("");
	return actor;
}

int destroy_actor(struct actor *actor)
{

}

struct actor *get_actor(struct hash_list *hlist, int id)
{
	struct actor *actor = NULL;
	struct list_head *pos;
	//Scan all actors from this list.
	list_for_each(pos, &hlist->list){
			actor = list_entry(pos, struct actor, list);	
			if(actor->id == id){
				return actor;
			}
	}			
	return NULL;
}

int enqueue_actor_work(int id, void (*func) (void *, struct actor *, void *), void *args, void *job)
{
	struct actor *actor;
	char name[20];
	int hlistid = hash_compute(g_hash_table, id);
	struct hash_list *hlist = &g_hash_table->hlist[hlistid];

	pthread_rwlock_wrlock(&hlist->lock);
	actor = get_actor(hlist, id);
	if(!actor){
		sprintf(name, "actor-%d", id);
		actor = create_actor(id, name, func, args);
	}
	pthread_rwlock_unlock(&hlist->lock);
	/*Enqueue Job.*/
	actor->mailbox->enqueue(actor->mailbox, job);
	return 0;
}
