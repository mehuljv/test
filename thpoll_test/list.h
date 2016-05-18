
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
int list_search(struct list *list, void *data);
struct list_node *list_get_first(struct list *list);
void *list_get_data(struct list_node *node);
struct list_node *list_get_next(struct list_node *node);

