#ifndef __LIST_H
#define __LIST_H
#include "types.h"
static inline INIT_LIST_HEAD(struct list_head *list)
{
	list->prev = list->next = list;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	struct list_head *tmp = head->next;
	new->next = tmp;
	new->prev = head;
	head->next = new;
	tmp->prev = new;
}

static inline void list_del(struct list_head *del)
{
	del->prev->next = del->next;
	del->next->prev = del->prev;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	struct list_head *tmp = head->prev;
	new->next = tmp->next;
	new->prev = tmp;
	tmp->next = new;
	head->prev = tmp;
}

#define list_entry(listptr, type, field_name) (type *)((unsigned long)listptr - (unsigned long)(&(((type *)(0UL))->field_name)))

#define list_first_entry(listptr, type, field_name) list_entry((listptr)->next, type, field_name)

#define list_for_each(pos, head) \
			for(pos=(head)->next; pos!=(head); pos=pos->next)

#define list_empty(head)	((head)->next == (head))

#endif
