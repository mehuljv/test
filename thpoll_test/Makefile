all : main.o queue_eventfd.o list.o threadpoll_test.o distribution_test.o
	gcc main.o distribution_test.o threadpoll_test.o list.o queue_eventfd.o -lpthread -o test

main.o: main.c list.h queue_eventfd.h
	gcc -c main.c

list.o: list.c list.h
	gcc -c list.c 

queue_eventfd.o: queue_eventfd.c queue_eventfd.h
	gcc -c queue_eventfd.c 

threadpoll_test.o: threadpoll_test.c queue_eventfd.h list.h
	gcc -c threadpoll_test.c -lpthread

distribution_test.o: distribution_test.c queue_eventfd.h list.h
	gcc -c distribution_test.c -lpthread

clean:
	rm -f *.o
	rm test
