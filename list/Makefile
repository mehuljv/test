
all : test.o list.o double_list.o 
	gcc -o linklist test.o list.o double_list.o

test.o : test.c
	gcc -c test.c

list.o : list.h
	gcc -c list.c

double_list.o : double_list.h
	gcc -c double_list.c

clean:
	rm linklist *.o
