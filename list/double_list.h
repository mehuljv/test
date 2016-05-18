
struct double_list
{
	struct double_list_node *head;
	struct double_list_node *tail;
	int number_of_nodes;
};

struct double_list_node
{
	void *data;
	struct double_list_node *next;
	struct double_list_node *prev;
};

struct double_list_node *double_list_search(struct double_list *list, void *data);
struct double_list *double_list_create();
int double_list_add(struct double_list *list, void *data);
int double_list_remove(struct double_list *list, void *data);
