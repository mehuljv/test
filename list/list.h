
struct list 
{
	struct list_node *head;
	int number_of_nodes;
};

struct list_node 
{
	void *data;
	struct list_node *next;
};

struct list *list_create();
int list_add(struct list *list, void *data);
struct list_node *list_search(struct list *list, void *data);
