#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#define Message(a, b...) printf("[%s @ %d] "a"\n", __FUNCTION__, __LINE__, ##b)

int main(int argc, char *argv[])
{
	int c;
	int threadpool_thread_count = 0;
	int actor_count = 0;
	int i = 0;
	int thpool_test = 0;
	int distri_test = 0;
	pthread_t *slave_threads;
	pthread_t master_thread;
	
	while((c = getopt(argc, argv, "dts:")) != -1)
	{
		switch(c)
		{
			case 't':
				thpool_test = 1;
				break;
			case 'd':
				distri_test = 1;
				break;
			case 's':
				threadpool_thread_count = atoi(optarg);
				break;
			default:
				break;
		}
	}

	Message("threadpool_thread_count %d", threadpool_thread_count);
	if(thpool_test){
		/*
		-Multiple slave polling on the same queue.
		-Master puts work into this queue with a unique workid.
		-One of the slave picksup the work and processes and puts it back to the master's response queue with the same workid.
		-One Request Queue & One Response queue shared by all slaves & master -- use lock
		*/
		threadpool_test(threadpool_thread_count);
	}else if(distri_test){
		/*
		-Each thread has its own request queue.
		-Master thread assigns the job to each of the thread in its own queue and gets the response back in dedicated queue -- lockless.
		*/
		distribution_test(threadpool_thread_count);
	}else{

	}
}
