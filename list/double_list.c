#include "double_list.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct double_list *double_list_create()
{
	struct double_list *list= malloc(sizeof(struct double_list));
	if(!list)
		return NULL;
	memset(list, 0, sizeof(struct double_list));
	return list;
}

int double_list_add(struct double_list *list, void *data)
{
	struct double_list_node *new;
	if(!list)
		return -EINVAL;

	new = (struct double_list_node *)malloc(sizeof(struct double_list_node));
	if(!new)
		return -ENOMEM;
	new->data = data;
	new->next = NULL;

	if(!list->head){
		//First Node;
		list->head = list->tail = new;
		new->prev = NULL;
	}else{
		new->prev = list->tail;
		list->tail->next = new;
		list->tail = new;
	}
}

int double_list_remove(struct double_list *list, void *data)
{
	struct double_list_node *node = double_list_search(list, data);
	struct double_list_node *prev;
	if(!node)
		return -EINVAL;
	prev = node->prev;

	if(node->prev == NULL && node->next == NULL){	
		/*This is the only node?*/
		list->head = list->tail = NULL;
	}else if(node->prev == NULL){
		//Node is HEAD pointer.
		list->head = node->next;
		node->next->prev = NULL;
	}else if(node->next == NULL){
		//Node is TAIL pointer.
		list->tail = node->prev;
		node->prev->next = NULL;
	}else{
		node->next->prev = node->prev;
		node->prev->next = node->next;
	}
	free(node);
	return 0;
}


struct double_list_node *double_list_search(struct double_list *list, void *data)
{
	struct double_list_node *tmp;
	if(!list)
		return NULL;
	tmp = list->head;
	while(tmp){
		if(tmp->data == data)
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

int double_list_destroy(struct double_list *list)
{
	struct double_list_node *node, *tmp;
	if(!list)
		return -EINVAL;
	node = list->head;
	while(node){
		tmp = node->next;
		free(node);
		node = tmp;
	}
	free(list);
	return 0;
}

int double_list_print_all(struct double_list *list)
{
	struct double_list_node *node;
	int i = 0;
	if(!list)
		return -EINVAL;
	node = list->head;
	while(node){
		printf("Element %d ==> %ld\n", i, (unsigned long)node->data);
		node =  node->next;
	}
	return 0;
}
