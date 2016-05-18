#include <stdio.h>
#include "graph.h"

int main()
{
	int nodes;
	struct graph *g;
	int i,j;
	int dst_node;

	printf("Enter Number of nodes:\n");
	scanf("%d", &nodes);
	g = create_graph(nodes);

	print_graph_table(g);
	for(i=0; i<nodes; i++){
		printf("Enter node number to create edges from node %d (Enter -1 to exit)\n", i);
		for(j=0; j<nodes; j++){
			scanf("%d", &dst_node);
			if(dst_node == -1)
				break;
			create_undirected_edge(g, i, dst_node);
		}
	}
	print_graph_table(g);
	
}
