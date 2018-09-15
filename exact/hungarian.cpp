#include "hungarian.h"

/**
 * @brief      main function of the algorithm.
 * 
 *             According to this step we need to check whether
 *             the matching is already perfect, if the answer is
 *             positive we just stop algorithm, otherwise we need
 *             to clear S, T and alternating tree and then find
 *             some exposed vertex from the X part. Also, in this
 *             step we are initializing a slack array.
 */
void augment ()
{

	//check wether matching is already perfect
	if (max_match == facilities) return;

	//the root vertex
	int root;

	//counters
	int x, y;

	// queue for bfs
	int queue_bfs[max_number_of_vertices];

	// write and read
	int write=0, read=0; 

	memset(S, false, sizeof(S)); // init set S
	memset(T, false, sizeof(T)); // init set T
	memset(prev, -1, sizeof(prev)); //init set prev - for the alternating tree

	for ( x=0; x < facilities; x++)
	{
		if (xy_match[x] == -1)
		{
			root = x;
			queue_bfs[write++] = root;
			prev[x] = -2;
			S[x] = true;
			break;
		}

		//initializing slack array
		for (int y=0; y < facilities; y++)
		{
			slack[y] = labels_x[root] + labels_y[y] - matrix_cost[root][y];
			slack_x[y] = root;
		}
	}	

	while (true)
	{
		//building tree with bfs cycle
		while (read < write)
		{
			// current vertex from X part
			x = queue_bfs[read++]; 

			//iterate through all edges in equality graph 
			for (y=0; y < facilities; y++)
			{
				if ((matrix_cost[x][y] == (labels_x[x] + labels_y[y])) && !T[y])
				{
					if (yx_match[y] == -1) break;
					T[y] = true;
					
					// add vertex yx[y], which is matched
					// with y, to the queue
					queue_bfs[write++] = yx_match[y];
				
					//add edges (x,y) and (y,yx[y]) to the tree
					add_to_tree(yx_match[y], x); 
				}
			}
			if (y < facilities) break; //augmenting path found!
		}			

		
		if (y < facilities) break; //augmenting path found!
		update_labels(); //augmenting path not found, so improve labeling
		write = read = 0;

		for (y=0; y < facilities; y++)
		{
			/**
			 * in this cycle we add edges that were added to the equality graph as a
			 * result of improving the labeling, we add edge (slackx[y], y) to the tree if
			 * and only if !T[y] && slack[y] == 0, also with this edge we add another one
			 * (y, yx[y]) or augment the matching, if y was exposed
				 */
			
			if (!T[y] && slack[y] == 0)
			{
				//exposed vertex in Y found - augmenting path exists!
				if (yx_match[y] == -1)
				{
					x = slack_x[y];
					break;
				} else 
				{
					T[y] = true; //else just add y to T
					if (!S[yx_match[y]])
					{
						//add vertex yx[y], which is matched with
						//y, to the queue /and add edges (x,y) and (y,
						//yx[y]) to the tree
						queue_bfs[write++] = yx_match[y];
						add_to_tree(yx_match[y], slack_x[y]);

					}
				}
			}
			if (y < facilities) break; //augmenting path found!
		}
		if (y < facilities) //we found augmenting path!
		{
			max_match++; //increment matching
			//in this cycle we inverse edges along augmenting path
			for (int cx = x, cy = y, ty; cx != -2; cx = prev[cx], cy = ty)
			{
				ty = xy_match[cx];
				yx_match[cy] = cx;
				xy_match[cx] = cy;
			}
			augment(); //recall function, go to step 1 of the algorithm
		}
	}
}

void update_labels()
{

	int x, y, delta = INF;

	// calculate delta using slack
	for (y=0; y < facilities; y++)
		if (!T[y]) delta = std::min(delta, slack[y]);

	// update X labels
	for (x=0; x < facilities; x++)
		if (S[x]) labels_x[x] -= delta;

	// update Y labels
	for (int y=0; y < facilities; y++)
		if (T[y]) labels_y[y] += delta;

	// update slack array
	for (int y=0; y < facilities; y++)
		if (!T[y]) slack[y] -= delta;

}

void add_to_tree (int x, int prev_x)
{
	S[x] = true; // add x to S
	prev[x] = prev_x; // we need this when augmenting

	//update slacks, because we add new vertex to S
	for (int i = 0; i < facilities; i++) 
	{
		if (labels_x[x] + labels_y[i] - matrix_cost[x][i] < slack[i])
		{
			slack[i] = labels_x[x] + labels_y[i] - matrix_cost[x][i];
			slack_x[i] = x;
		}
	}

}

int hungarian()
{

	int ret = 0; //weight of the optimal matching
	max_match = 0; //number of vertices in current matching
	facilities = max_number_of_vertices;
	
	memset(xy_match, -1, sizeof(xy_match));
	memset(yx_match, -1, sizeof(yx_match));
	
	init_labels(); //step 0
	augment(); //steps 1-3
	
	for (int x = 0; x < facilities; x++) //forming answer there
		ret += matrix_cost[x][xy_match[x]];
		
	return ret;
}

void init_labels()
{

	memset(labels_x, 0, sizeof(labels_x));
	memset(labels_y, 0, sizeof(labels_y));
	for (int x = 0; x < facilities; x++)
		for (int y = 0; y < facilities; y++)
			labels_x[x] = std::max(labels_x[x], matrix_cost[x][y]);
}



int main ()
{
	std::cout << hungarian() << std::endl;
	return 0;
}