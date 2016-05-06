all : main.o queue_eventfd.o actor.o hash.o
	gcc main.o queue_eventfd.o actor.o hash.o -lpthread -o test

main.o: main.c list.h queue_eventfd.h
	gcc -c main.c

queue_eventfd.o: queue_eventfd.c queue_eventfd.h list.h
	gcc -c queue_eventfd.c 

hash.o: hash.h hash.c list.h 
	gcc -c hash.c 

actor.o: hash.h actor.c actor.h list.h queue_eventfd.h atomic.h
	gcc -c actor.c -lpthread

clean:
	rm -f *.o
	rm test
