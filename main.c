#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "astar.h"
#include "gen.h"

/* matrix is cost from prize to prize, from start to prizes and prizes to end.
 * index 0: start
 * 1 .. num_prizes: prizes
 * index num_prizes+1: end
 */

TSP_Node *graph;

void swap_pos(int *pos1, int *pos2)
{
	int tmp;

	tmp = pos1[0];
	pos1[0] = pos2[0];
	pos2[0] = tmp;

	tmp = pos1[1];
	pos1[1] = pos2[1];
	pos2[1] = tmp;
}

int *inv_vet(int *vet, int sz)
{
	int *new_vet = malloc(sizeof(int)*sz);
	int i;

	for(i = 0; i < sz; i++)
	{
		new_vet[sz-i-1] = vet[i];
	}

	return new_vet;
}

void calc_cost_matrix(void)
{
	int i,j;
	int bkp_num_prizes = G.gi.num_prizes;

	TSP_Node *start_graph = malloc(sizeof(TSP_Node) * G.gi.num_prizes);

	for (i=0; i<G.gi.num_prizes; i++) 
	{
		TSP_Node *node = &start_graph[i];

		node->path = a_star_search(G.gi.start, G.gi.prizes[i], &node->cost, &node->steps);

		/* impossible to get there! */
		if(node->cost < 0) G.gi.prizes[i][0] = -1;
	}

	for (i=0; i<G.gi.num_prizes; i++) {
		if(start_graph[i].cost < 0)
		{
			swap_pos(G.gi.prizes[i], G.gi.prizes[G.gi.num_prizes-1]);
			G.gi.num_prizes--;
			i--;
		}
	}

	graph = malloc(sizeof(TSP_Node) * (G.gi.num_prizes+2) * (G.gi.num_prizes+2));

	for (i=j=0; i<bkp_num_prizes; i++) {
		if(start_graph[i].cost >= 0)
		{
			graph[mkcostidx(0, j+1)].path = start_graph[i].path;
			graph[mkcostidx(j+1, 0)].path = inv_vet(start_graph[i].path, start_graph[i].steps);

			graph[mkcostidx(0, j+1)].cost = start_graph[i].cost;
			graph[mkcostidx(j+1, 0)].cost = start_graph[i].cost;

			graph[mkcostidx(0, j+1)].steps = start_graph[i].steps;
			graph[mkcostidx(j+1, 0)].steps = start_graph[i].steps;
			j++;
		}
	}
	free(start_graph);

	for (i=0; i<G.gi.num_prizes; i++) {
		for (j=0; j<i; j++) {
			TSP_Node *node = &graph[mkcostidx(i+1, j+1)];
			TSP_Node *node_inv = &graph[mkcostidx(j+1, i+1)];

			node->path = a_star_search(G.gi.prizes[i], G.gi.prizes[j], &node->cost, &node->steps);

			node_inv->cost = node->cost;
			node_inv->steps = node->steps;
			node_inv->path = inv_vet(node->path, node->steps);
		}
	}

	for (i=0; i<G.gi.num_prizes; i++) {
			TSP_Node *node = &graph[mkcostidx(i+1, G.gi.num_prizes+1)];

			node->path = a_star_search(G.gi.prizes[i], G.gi.end, &node->cost, &node->steps);
	}

#if 0
	for (i=0; i<G.gi.num_prizes+2; i++) {
		for (j=0; j<G.gi.num_prizes+2; j++) {
			printf("% 7.1f", _cost[mkcostidx(i, j)]);
		}
		printf("\n");
	}
#endif
}

int main(int argc, const char *argv[])
{
#if defined(ALLEGRO)
	int i;
	int *path;
	int *goal, *ptr;
	float distance;
#endif

	srand(time(NULL));

	game_read();
	// print_map();
	calc_cost_matrix();

#if defined(ALLEGRO)
	gfx_init(600, 600, 3);

	path = ga_solve_tsp(100000, &distance);

	for (i = 0; i < G.gi.num_prizes+2; i++) {
		printf(" %d", path[i]);
	}
	printf("\n");

	printf("distance: %f\n", distance);
	while (1) {
		int j=0;

		for (j=0; j<G.gi.num_prizes+1; j++) {
			TSP_Node *node = &graph[mkcostidx(path[j], path[j+1])];

			goal = node->path;
			for(i = 0; i < node->steps; i++)
			{
				G.gi.cur[0] = node->path[i*2 + 0];
				G.gi.cur[1] = node->path[i*2 + 1];

				gfx_step();
			}
		}
	}
	gfx_end();
	free(graph);
#endif
	return 0;
}
