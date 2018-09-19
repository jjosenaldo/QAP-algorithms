#include <algorithm> // std::fill, std::max, std::max_element, std::matching
#include <iostream> // std::cout, std::endl
#include "hungarian.h"

// infinity
#define INF 1e8

int N;
int max_match;
int* label_x;
int* label_y;
int* match_xy;
int* match_yx;
bool* S;
bool* T;
int* slack;
int* slack_causer;
int* prev_on_tree;

void add_to_tree(int current, int prev, int** cost)
{
	int slack_for_new_node;

	// adds the current vertex to S
	S[current] = true;

	prev_on_tree[current] = prev;

	// update slacks since we added a vertex to S
	for(int i = 0; i < N; ++i)
	{
		slack_for_new_node = label_x[current] + label_y[i] - cost[current][i];

		if(slack_for_new_node < slack[i])
		{
			slack[i] = slack_for_new_node;
			slack_causer[i] = current;
		}
	}
}

void init_labels(int** cost)
{
	std::fill(label_y, label_y+N, 0);
	std::fill(label_x, label_x+N, -INF);

	for(int i = 0; i < N; ++i)
		for(int j = 0; j < N; ++j)
			label_x[i] = std::max(label_x[i], cost[i][j]);

	std::fill(match_xy, match_xy+N, -1);
	std::fill(match_yx, match_yx+N, -1);
}

void update_labels()
{
	int delta = INF;

	// find delta
	for(int i = 0; i < N; ++i)
	{
		// "i" is in X\T
		if(!T[i])
			delta = std::min(delta, slack[i]);
	}

	// update labels
	for(int i = 0; i < N; ++i)
	{
		if(S[i])
			label_x[i] -= delta; 

		if(T[i])
			label_y[i] += delta;
	}

	// update slacks
	for(int i = 0; i < N; ++i)
	{
		if(!T[i])
			slack[i] -= delta;
	}
}

void augment(int** cost)
{
	if(max_match == N)
		return;

	// queue for BFS
	int* bfs_queue = new int[N];

	// index for writing in the BFS queue
	int queue_write = 0;

	// index for reading in the BFS queue
	int queue_reading = 0;

	// clear S and T
	std::fill(S, S+N, false);
	std::fill(T, T+N, false);

	int root;

	// clear alternating tree
	std::fill(prev_on_tree, prev_on_tree+N, -1);

	// looks for an exposed vertex
	for(int i = 0; i < N; ++i)
	{
		//exposed vertex in X: add it to S, make it the root of the tree, add it to the BFS queue
		if(match_xy[i] == -1)
		{
			bfs_queue[queue_write++] = i;
			root = i;
			prev_on_tree[root] = -2;
			S[root] = true;
			break;
		}
	}

	// update slacks (X = {x}, so it's the slack causer for everyone in Y)
	for(int i = 0; i < N; ++i)
	{
		slack[i] = label_x[root] + label_y[i] - cost[root][i];
		slack_causer[i] = root;
	}

	int current;
	int i;

	while(true)
	{
		// build tree with bfs
		while(queue_reading < queue_write)
		{
			current = bfs_queue[queue_reading++];

			// iterates over the (current,i) edges of the equality subgraph that aren't in T
			for(i = 0; i < N; ++i)

				if(cost[current][i] == label_x[current] + label_y[i] && !T[i])
				{
					// "i" vertex is exposed on Y: an augmenting path was found
					if(match_yx[i] == -1)
						break;

					// adds "i" to T
					T[i] = true;

					// adds the match of "i" to the queue
					bfs_queue[queue_write++] = match_yx[i];

					// adds the edges (current,i) and (i,match_yx[i]) to the alternating tree
					add_to_tree(match_yx[i], current, cost);
				}
	
			// augmenting path found				
			if(i < N) 
				break;
		}

		// augmenting path found
		if(i < N)
			break;

		// improve the labels
		update_labels();

		queue_reading = 0;
		queue_write = 0;

		for(i = 0; i < N; ++i)
			// edges added to the equality subgraph after the label improving
			if(!T[i] && slack[i] == 0)
			{
				// "i" is exposed in Y: augmenting path found
				if(match_yx[i] == -1)
				{
					current = slack_causer[i];
					break;
				}

				// "i" is not exposed
				else
				{
					// adds "i" to T
					T[i] = true;

					if(!S[ match_yx[i] ])
					{
						// adds the match of "i" to the queue
						bfs_queue[queue_write++] = match_yx[i];

						add_to_tree(match_yx[i], slack_causer[i], cost);
					}
				}	
			}

		// augmenting path found
		if(i < N)
			break;
	}

	// augmenting path found
	if(i < N)
	{
		// increment the matching in one edge
		++max_match;

		// invert the edges along the path
		for (int cx = current, cy = i, ty; cx != -2; cx = prev_on_tree[cx], cy = ty)
		{
			ty = match_xy[cx];
			match_yx[cy] = cx;
			match_xy[cx] = cy;
		}

		// try to augment again
		augment(cost);
	}

	delete[] bfs_queue;
}

int hungarian_least_cost(int n, int** matrix)
{
	init_global_variables(n);

	int maximum = -INF;
	for(int i = 0; i < n; ++i)
		maximum = std::max(*std::max_element(matrix[i], matrix[i]+n), maximum);

	for(int i = 0; i < n; ++i)
		for(int j = 0; j < n; ++j)
				matrix[i][j] = maximum - matrix[i][j];

	init_labels(matrix);
	augment(matrix);

	int cost = 0;

	// print matching
	for(int i = 0; i < N; ++i)
	{
		cost += -(matrix[i][match_xy[i]]-maximum);
		//cost += matrix[i][match_xy[i]];
		//std::cout << "(" << i << ", " << match_xy[i] << ")\n";
		
	}

	deallocate_global_variables();	

	return cost;

}

void deallocate_global_variables()
{
	delete[] label_x;
	delete[] label_y;
	delete[] match_xy;
	delete[] match_yx;
	delete[] S;
	delete[] T;
	delete[] slack;
	delete[] slack_causer;
	delete[] prev_on_tree;
}

void init_global_variables(int n)
{
	N = n;
	max_match = 0;
	label_x = new int[n];
	label_y = new int[n];
	match_xy = new int[n];
	match_yx = new int[n];
	S = new bool[n];
	T = new bool[n];
	slack = new int[n];
	slack_causer = new int[n];
	prev_on_tree = new int[n];

	std::fill(match_xy, match_xy+N, -1);
	std::fill(match_yx, match_yx+N, -1);	
}