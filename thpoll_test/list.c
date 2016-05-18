#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct list *list_create()
{
	struct list *list= malloc(sizeof(struct list));
	if(!list)
		return NULL;
	memset(list, 0, sizeof(struct list));
	return list;
}

int list_add(struct list *list, void *data)
{
	struct list_node *new;
	if(!list)
		return -EINVAL;

	new = (struct list_node *)malloc(sizeof(struct list_node));
	if(!new)
		return -ENOMEM;
	new->data = data;
	new->next = list->head;
	list->head = new;
	list->number_of_nodes++;
}

int list_remove(struct list *list, void *data)
{
	struct list_node *node = list->head;
	struct list_node *prev = NULL;
	
	while(node){
		if(node->data == data){
			if(!prev)
				list->head = node->next;
			else
				prev->next = node->next;
			free(node);
			list->number_of_nodes--;
			return 0;
		}
		prev = node;
		node = node->next;
	}
	return -EINVAL;
}


int list_search(struct list *list, void *data)
{
	struct list_node *tmp;
	if(!list)
		return 0;
	tmp = list->head;
	while(tmp){
		if(tmp->data == data)
			return 1;
		tmp = tmp->next;
	}
	return 0;
}

int list_destroy(struct list* list)
{
	struct list_node *tmp, *tmp1;
	if(!list)
		return -EINVAL;
	tmp = list->head;
	
	while(tmp){
		tmp1 = tmp->next;
		free(tmp);
		tmp = tmp1;	
	}
	free(list);
}

int list_print_all(struct list *list)
{
	struct list_node *node;
	int i = 0;
	if(!list)
		return -EINVAL;
	node = list->head;
	while(node){
		printf("Element %d ==> %ld\n", i++, (unsigned long)node->data);
		node =  node->next;
	}
	return 0;
}

struct list_node *list_get_first(struct list *list)
{
	if(!list || !list->head)
		return NULL;
	return list->head;
}


struct list_node *list_get_next(struct list_node *node)
{
	if(!node)
		return NULL;
	return node->next;
}

void *list_get_data(struct list_node *node)
{
	return node->data;
}
