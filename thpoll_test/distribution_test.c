#include "queue_eventfd.h"
#include "list.h"
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_THREADS	64
#define MAX_QUEUES 	64
#define Message(a, b...) //{printf("[%s @ %d] "a"\n", __FUNCTION__, __LINE__, ##b); fflush(stdout);}

static struct queue *g_req_queue_map[MAX_THREADS][MAX_QUEUES];
static struct queue *g_rsp_queue_map[MAX_THREADS][MAX_QUEUES];
static int g_num_slave_threads;

struct thread_info
{
	struct list *req_q_list;
	struct list *rsp_q_list;
	int thread_id;
};

struct job_info
{
	int slave_thread_id;
	int rsp_thread_id;
	int rsp_q_id;
	int result;
	int op;	//op=1/add, op=2/sub, op=3/mul, op=4/div
	int a,b;
};

static struct thread_info *create_thread_info(int id)
{
	struct thread_info *thinfo = malloc(sizeof(struct thread_info));

	if(!thinfo)
		return NULL;

	thinfo->thread_id = id;

	thinfo->req_q_list = list_create();

	if(!thinfo->req_q_list){
		free(thinfo);
		return NULL;
	}
	
	thinfo->rsp_q_list = list_create();
	if(!thinfo->rsp_q_list){
		list_destroy(thinfo->req_q_list);
		free(thinfo);
		return NULL;
	}
	
	return thinfo;
}

static int add_req_q_to_thread_info(struct thread_info *thinfo, struct queue *q)
{
	if(!thinfo || !q)
		return -EINVAL;

	g_req_queue_map[thinfo->thread_id][q->index] = q;

	return list_add(thinfo->req_q_list, q);
}

static int add_rsp_q_to_thread_info(struct thread_info *thinfo, struct queue *q)
{
	if(!thinfo || !q)
		return -EINVAL;

	g_rsp_queue_map[thinfo->thread_id][q->index] = q;

	return list_add(thinfo->rsp_q_list, q);
}


static int setup_epoll_fd(struct list *list, int id)
{
	struct list_node *node = list_get_first(list);	
	struct queue *q;
	struct epoll_event event;
	struct epoll_event *events;
	int efd = epoll_create1(0);

	while(node){
		q = (struct queue *)list_get_data(node);		
		event.data.ptr = q;
		event.events = EPOLLIN;
		epoll_ctl(efd, EPOLL_CTL_ADD, q->eventfd, &event);
		node = list_get_next(node);
		printf("Thread%d, Settingup Request Queue %p\n", id, q);
	}
	return efd;
}

static int setup_master_epoll_fd(struct list *list, int id)
{
	struct list_node *node = list_get_first(list);	
	struct queue *q;
	struct epoll_event event;
	struct epoll_event *events;
	int efd = epoll_create1(0);

	while(node){
		q = (struct queue *)list_get_data(node);		
		event.data.fd = -1;
		event.data.ptr = q;
		event.events = EPOLLIN;
		epoll_ctl(efd, EPOLL_CTL_ADD, q->eventfd, &event);
		node = list_get_next(node);
		printf("Thread%d, Settingup Response Queue %p\n", id, q);
	}

	event.events = EPOLLIN|EPOLLPRI|EPOLLERR;
    event.data.fd = STDIN_FILENO;
	
    if (epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &event) != 0) {
        perror("epoll_ctr add stdin failed.");
        return 1;
    }
	
	return efd;
}

static struct queue *get_response_queue(int thread, int queue)
{
	return g_rsp_queue_map[thread][queue];
}

static struct queue *get_request_queue(int x, int y, int z)
{
	int hash = (x*y*z) | ((x) | (y) | (z));
	//int hash = y;
	return g_req_queue_map[(hash % g_num_slave_threads) + 1][0];
}

static void *master_thread_func(void *data)
{
	struct thread_info *thrinfo = (struct thread_info *)data;
	struct list *list= thrinfo->rsp_q_list;
	int efd = setup_master_epoll_fd(list, thrinfo->thread_id);
	int num_queues = list->number_of_nodes;
	struct job_info *job;
	int n;
	struct epoll_event *events;
	int i ;
	struct queue *req_q, *rsp_q;

	events = malloc(sizeof(struct epoll_event) * (num_queues+1));
	while(1){
		n = epoll_wait(efd, events, num_queues, -1);
		for(i=0; i<n; i++){
			if(events[i].data.fd == STDIN_FILENO){
				job = malloc(sizeof(struct job_info));
				job->rsp_thread_id = thrinfo->thread_id;
				scanf("%d",&job->op);
				scanf("%d",&job->a);
				scanf("%d",&job->b);
				job->rsp_q_id = job->b % g_num_slave_threads;
				req_q = get_request_queue(job->op, job->a, job->b);
				req_q->enqueue(req_q, job);
			}else{
				rsp_q = events[i].data.ptr;
				job = (struct job_info *)rsp_q->dequeue(rsp_q);
				printf("Operation %d, Operand - %d & %d, Result %d, ThreadServed %d\n", job->op, job->a, job->b, job->result, job->slave_thread_id);
				free(job);
			}
		}	
	}	
}

static void *slave_thread_func(void *data)
{
	struct thread_info *thrinfo = (struct thread_info *)data;
	struct job_info *job;
	struct list *list= thrinfo->req_q_list;
	int efd = setup_epoll_fd(list, thrinfo->thread_id);
	int num_queues = list->number_of_nodes;
	struct epoll_event *events;
	int n,i;

	events = malloc(sizeof(struct epoll_event) * num_queues);

	while(1){
		n = epoll_wait(efd, events, num_queues, -1);
		printf("Thread%d wokenup with %d events.\n",thrinfo->thread_id, n);
		for(i=0; i<n; i++){
			struct queue *q = (struct queue *)events[i].data.ptr;
			struct queue *rsp_q;
			job = (struct job_info *)q->dequeue(q);
			if(!job)
				continue;
			switch(job->op){
				case 1:
					job->result = job->a + job->b;	
					break;
				case 2:
					job->result = job->a - job->b;	
					break;
				case 3:
					job->result = job->a * job->b;	
					break;
				case 4:
				default:
					job->result = job->a / job->b;	
					break;
			}
			rsp_q = get_response_queue(job->rsp_thread_id, job->rsp_q_id);
			printf("Queuing Response in Queue %p\n", rsp_q);
			job->slave_thread_id = thrinfo->thread_id;
			rsp_q->enqueue(rsp_q, job);
			sleep(15);
		}
	}
	return NULL;
}

int distribution_test(int thread_count)
{
	int i = 0;
	struct thread_info *master;
	struct thread_info **slave;
	struct queue *req_q;
	struct queue *rsp_q;
	pthread_t slave_threads[MAX_THREADS];
	pthread_t master_thread;
	g_num_slave_threads = thread_count;

	slave = (struct thread_info **)malloc(sizeof(struct thread_info *)*thread_count);	

	if(!slave)
		return -ENOMEM;

	//Create master thread info.
	master = create_thread_info(0);

	for(i=0; i<thread_count; i++){
		slave[i] = create_thread_info(i+1);		
		//One req_q per thread.
		req_q = create_queue(FLAG_QUEUE_WITH_LOCK, 100, 0);	
		add_req_q_to_thread_info(slave[i], req_q);

		//Create corrosponding rsp queue.	
		rsp_q = create_queue(FLAG_QUEUE_WITH_LOCK, 100, i);
		add_rsp_q_to_thread_info(master, rsp_q);
	}

	/*Start Slave Threads.*/
	for(i=0; i<thread_count; i++)
	{
		char name[20];
		pthread_create(&slave_threads[i], NULL, slave_thread_func, slave[i]);
		sprintf(name, "slave-%d", i);
		pthread_setname_np(slave_threads[i], name);
	}

	//Start master thread.
	pthread_create(&master_thread, NULL, master_thread_func, master); 	
	pthread_setname_np(master_thread, "master");

	//Wait for master thread.
	pthread_join(master_thread, NULL);
}
