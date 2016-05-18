#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if 0
struct graph *create_graph_list(int num_vertices)
{
	struct graph *graph;
	int i;
	int size;

	size = sizeof(struct graph) + sizeof(struct graph_node)*num_vertices;

	graph = (struct graph *)malloc(size);

	memset(graph, 0, size);
	graph->num_vertices = num_vertices;
	
	for(i=0; i<num_vertices; i++){
		graph->nodes[i].index = i;
		INIT_LIST_HEAD(&graph->nodes[i].list);
	}
	return graph;
}
#endif

void print_graph_table(struct graph *graph)
{
	int **nodes = graph->nodes;
	int vertices = graph->num_vertices;
	int i,j;


	printf("Node\t\t");
	for(i=0; i<vertices; i++){
		printf("%d\t", i);
	}
	printf("\n\n");
	for(i=0; i<vertices; i++){
		printf("%d\t\t", i);
		for(j=0; j<vertices; j++){
			printf("%d\t", nodes[i][j]);
		}
		printf("\n");
	} 
}

struct graph *create_graph(int num_vertices)
{
	struct graph *graph;
	int i;
	int size;

	size = sizeof(struct graph);
	graph = (struct graph *)malloc(size);
	memset(graph, 0, size);

	graph->nodes = (int **)(malloc(sizeof(int *)*num_vertices));

	for(i=0; i<num_vertices; i++){
		graph->nodes[i] = (int *)malloc(sizeof(int)*num_vertices);
		graph->nodes[i][i] = 1;
	}
	graph->num_vertices = num_vertices;	
	return graph;
}

int create_undirected_edge(struct graph *graph, int node1, int node2)
{
	graph->nodes[node1][node2] = graph->nodes[node2][node1] = 1;
}

int create_directed_edge(struct graph *graph, int src, int dest)
{
	graph->nodes[src][dest] = 1;
}

int create_weighted_undirected_edge(struct graph *graph, int node1, int node2, int weight)
{
	graph->nodes[node1][node2] = graph->nodes[node2][node1] = weight;
}

int create_weighted_directed_edge(struct graph *graph, int src, int dest, int weight)
{
	graph->nodes[src][dest] = weight;
}


