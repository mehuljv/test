#ifndef __GRAPH_H
#define __GRAPH_H

#include "list.h"

struct graph *create_graph(int num_vertices);

struct graph_node
{
	int index;
	struct list_head list;
};

struct graph
{
	int num_vertices;
	int **nodes;
//	struct graph_node nodes[0];
};

#endif
