#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "common.h"

#include <unistd.h>
#include <sys/syscall.h>

#ifdef SYS_gettid
#else
#error "SYS_gettid unavailable on this system"
#endif

struct job_req
{
	int data;
};

void actor_processing(void *data, struct actor *actor, void *args)
{
	struct job_req *job = (struct job_req *)data;

	Message("Thread %ld, Actor %d, Processed Data %d", syscall(SYS_gettid), actor->id, job->data);

	if(actor->id == 0){
		free(job);
	}else{
		enqueue_actor_work(actor->id - 1, actor_processing, NULL, job);
	}
	return;
}

void test_actors(int num_threads, int num_buckets)
{
	int i = 0;
	char name[20];
	int actor_id = 0;
	int data = 0;
	struct job_req *job;
	struct queue *q;

	Message("");
	actor_initialize_subsystem(num_threads, num_buckets);
	Message("");
	while(1){
		printf("Enter Actor ID\n");
		scanf("%d", &actor_id);
		printf("Enter Data\n");
		scanf("%d", &data);
		job = (struct job_req *)malloc(sizeof(struct job_req));
		job->data = data;
		/*Enqueue request to `actor_id` for `data`.*/
		enqueue_actor_work(actor_id, actor_processing, NULL, job);
	}
}

int main(int argc, char *argv[])
{
	int c;
	int num_threads = 16;
	int num_buckets = 8;
	
	while((c = getopt(argc, argv, "h:s:")) != -1)
	{
		switch(c)
		{
			case 's':
				num_threads = atoi(optarg);
				break;
			case 'h':
				num_buckets = atoi(optarg);
				break;
			default:
				break;
		}
	}

	Message("num_threads %d, num_buckets %d\n", num_threads, num_buckets);
	test_actors(num_threads, num_buckets);	
}
