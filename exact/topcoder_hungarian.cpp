#include <bits/stdc++.h>
using namespace std;

#define N 3
#define INF 1e8

int cost[N][N] = {{7, 4, 3}, {3, 1, 2}, {3, 0, 0}};
int max_match;
int label_x[N], label_y[N];
int match_xy[N], match_yx[N];
bool S[N], T[N];
int slack[N];
int slack_causer[N];
int prev_on_tree[N];

void add_to_tree(int current, int prev)
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

void init_labels()
{
	fill(label_y, label_y+N, 0);
	fill(label_x, label_x+N, -INF);

	for(int i = 0; i < N; ++i)
		for(int j = 0; j < N; ++j)
			label_x[i] = max(label_x[i], cost[i][j]);

	fill(match_xy, match_xy+N, -1);
	fill(match_yx, match_yx+N, -1);
}

void update_labels()
{
	int delta = INF;

	// find delta
	for(int i = 0; i < N; ++i)
	{
		// "i" is in X\T
		if(!T[i])
			delta = min(delta, slack[i]);
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

void augment()
{
	if(max_match == N)
		return;

	// queue for BFS
	int bfs_queue[N];

	// index for writing in the BFS queue
	int queue_write = 0;

	// index for reading in the BFS queue
	int queue_reading = 0;

	// clear S and T
	fill(S, S+N, false);
	fill(T, T+N, false);

	int root;

	// clear alternating tree
	fill(prev_on_tree, prev_on_tree+N, -1);

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
					add_to_tree(match_yx[i], current);
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

						add_to_tree(match_yx[i], slack_causer[i]);
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
		augment();
	}
}

void hungarian()
{
	max_match = 0;

	fill(match_xy, match_xy+N, -1);
	fill(match_yx, match_yx+N, -1);

	init_labels();
	augment();

	// print matching
	for(int i = 0; i < N; ++i)
		cout << "(" << i << ", " << match_xy[i] << ")\n";

}

main()
{
	hungarian();
}