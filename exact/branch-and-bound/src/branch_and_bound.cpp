#include <algorithm> // std::copy, std::fill, std::random_shuffle, std::sort
#include <climits> // std:;INT_MAX
#include "branch_and_bound.h"
#include <functional>

QAPBranch::QAPBranch(int n, int** d_mat, int** f_mat)
{
	this->n = n;
	this->d_mat = d_mat;
	this->f_mat = f_mat;

	this->generate_initial_solution();	
	this->matrices_to_ordered_edge_vectors();
}

QAPBranch::~QAPBranch()
{
	delete[] this->current_best_solution;
}

void QAPBranch::matrices_to_ordered_edge_vectors()
{
	int number_of_edges = this->n * this->n; 

	this->f_edge_vector.reserve(number_of_edges);
	this->d_edge_vector.reserve(number_of_edges);

	int pos = 0;
	for(int i = 0; i < n; ++i)
	{
		for(int j = 0; j < n; ++j)
		{
			this->f_edge_vector[pos] = {i, j, this->f_mat[i][j]};
			this->d_edge_vector[pos++] = {i, j, this->d_mat[i][j]};
		}
	}

	// ascending sort
	std::sort(this->d_edge_vector.begin(), this->d_edge_vector.end());

	// descending sort
	std::sort(this->f_edge_vector.begin(), this->f_edge_vector.end(), greater_edge);
}

void QAPBranch::solve()
{

	int* current_solution = new int[n];

	bool* already_in_solution = new bool[n];
	std::fill(already_in_solution, already_in_solution+n, false);

	this->recursive_search_tree_exploring(0, 0, current_solution, already_in_solution);

	delete[] current_solution;
	delete[] already_in_solution;
}

void QAPBranch::recursive_search_tree_exploring(int current_cost, 
	int current_solution_size, int* current_solution, bool* already_in_solution)
{
	// full solution (leaf): check if it is better than the best already found
	if(current_solution_size == n)
	{
		// current solution is better: update best solution
		if(current_cost < this->current_best_cost)
		{
			this->current_best_cost = current_cost;

			std::copy(current_solution, current_solution+n, this->current_best_solution);
		}
	}

	// empty partial solution: no need for analyzing solution feasibility
	else if(current_solution_size == 0)
	{
		for(int i = 0; i < this->n; ++i)
		{
			current_solution[0] = i;
			already_in_solution[i] = true;

			this->recursive_search_tree_exploring(0, 1, current_solution, already_in_solution);

			already_in_solution[i] = false;
		}
		
	}

	// non-empty partial solution
	else
	{
		// analyze solution feasibility
		int lower_bound = this->lower_bound_for_partial_solution(current_solution_size, already_in_solution, current_cost);

		// current solution can't get better than the best known so far: its
		// branch must be pruned off
		if(lower_bound > current_best_cost)
		{
			return;
		}

		// explore the current node's children
		else
		{	
			for(int i = 0; i < this->n; ++i)
			{
				if(!already_in_solution[i])
				{
					// the i-th facility is assigned to the (current_solution_size)-th location
					current_solution[current_solution_size] = i;
					already_in_solution[i] = true;

					// compute the cost increase, i.e., the product d_{a,b}*f_{pi(a), pi(b)}
					// for all 0 <= a,b < n such that a,b<=current_solution_sizes
					int cost_increase = 0;
					for(int j = 0; j < current_solution_size; ++j)
						cost_increase += d_mat[j][current_solution_size]*f_mat[current_solution[j]][i]
									   + d_mat[current_solution_size][j]*f_mat[i][current_solution[j]];

					// explore the subsolution branch
					this->recursive_search_tree_exploring(current_cost + cost_increase, current_solution_size+1, current_solution, already_in_solution);

					// removes the element of the solution to analyze its siblings
					already_in_solution[i] = false;
				}
			}
		}	
	}

}

// TO-DO
void QAPBranch::generate_initial_solution()
{

	this->current_best_solution = new int[this->n];

	for(int i = 0; i < this->n; ++i)
		this->current_best_solution[i] = i;

	
	std::random_shuffle(this->current_best_solution, this->current_best_solution + this->n);
}

// TO-DO
int QAPBranch::lower_bound_for_partial_solution(int partial_solution_size, bool* already_in_solution, int current_partial_cost)
{
	
	std::vector<Edge>::iterator it_d_vector = this->d_edge_vector.begin();
	std::vector<Edge>::iterator it_f_vector = this->f_edge_vector.begin();

	int cost_increase = 0;
	int remaining_nodes = this->n - partial_solution_size;

	while(remaining_nodes >= 0)
	{
		while(already_in_solution[(*it_d_vector).v1] || already_in_solution[(*it_d_vector).v2])
			++it_d_vector;

		while(already_in_solution[(*it_f_vector).v1] || already_in_solution[(*it_f_vector).v2])
			++it_f_vector;

		cost_increase += (*it_d_vector).weight * (*it_f_vector).weight;
		--remaining_nodes;
	}

	return cost_increase + current_partial_cost;
}

int* QAPBranch::get_current_best_solution()
{
	return this->current_best_solution;
}

int QAPBranch::get_current_best_cost()
{
	return this->current_best_cost;
}

bool greater_edge(Edge e1, Edge e2){return e2 < e1;}
