#ifndef __COMMON_H
#define __COMMON_H
#include "queue_eventfd.h"
#include "list.h"
#include "atomic.h"
#include "actor.h"
#include "hash.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "types.h"
#define Message(a, b...) {printf("[%s @ %d] "a"\n", __FUNCTION__, __LINE__, ##b); fflush(stdout);}
#endif
