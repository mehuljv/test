#include <stdio.h>
#include "list.h"
#include "double_list.h"

void test_double_list()
{
	struct double_list *list = double_list_create();
	double_list_add(list, (void *)1UL);
	double_list_add(list, (void *)2UL);
	double_list_add(list, (void *)3UL);
	printf("Printing all element.\n");
	double_list_print_all(list);

	printf("Deleting Non-Existant element.\n");
	double_list_remove(list, (void *)4UL);

	printf("Printing all element.\n");
	double_list_print_all(list);
	printf("Deleting Existant element.\n");
	double_list_remove(list, (void *)2UL);
	printf("Printing all element.\n");
	double_list_print_all(list);
	printf("Adding 2 back .\n");
	double_list_add(list, (void *)2UL);
	printf("Printing all element.\n");
	double_list_print_all(list);
	double_list_remove(list, (void *)1UL);
	double_list_remove(list, (void *)2UL);
	printf("Printing after deleting head & tail all element.\n");
	double_list_print_all(list);
	
	struct double_list_node *node=	double_list_search(list, (void *)1UL);
	if(node){
		printf("Search result %ld\n", (unsigned long)node->data);
	}else{
		printf("Element 1UL not found!\n");
	}
}

void test_single_list()
{
	struct list *list = list_create();
	list_add(list, (void *)1UL);
	list_add(list, (void *)2UL);
	list_add(list, (void *)3UL);
	printf("Printing all element.\n");
	list_print_all(list);

	printf("Deleting Non-Existant element.\n");
	list_remove(list, (void *)4UL);

	printf("Printing all element.\n");
	list_print_all(list);
	printf("Deleting Existant element.\n");
	list_remove(list, (void *)2UL);
	printf("Printing all element.\n");
	list_print_all(list);
	printf("Adding 2 back .\n");
	list_add(list, (void *)2UL);
	printf("Printing all element.\n");
	list_print_all(list);
	list_remove(list, (void *)2UL);
	printf("Printing after deleting head element.\n");
	list_print_all(list);
	
	struct list_node *node=	list_search(list, (void *)1UL);
	if(node){
		printf("Search result %ld\n", (unsigned long)node->data);
	}else{
		printf("Element 1UL not found!\n");
	}
}


int main()
{
	test_single_list();
	test_double_list();
}
