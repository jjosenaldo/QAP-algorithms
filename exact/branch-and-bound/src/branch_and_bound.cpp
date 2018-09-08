#include <algorithm> // std::fill, std::copy
#include <climits> // std:;INT_MAX
#include "branch_and_bound.h"

QAPBranch::QAPBranch(int n, int** d_mat, int** f_mat)
{
	this->n = n;
	this->d_mat = d_mat;
	this->f_mat = f_mat;

	this->generate_initial_solution();	
}

QAPBranch::~QAPBranch()
{
	delete[] this->current_best_solution;
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

void QAPBranch::recursive_search_tree_exploring(int current_cost, int current_solution_size, int* current_solution, bool* already_in_solution)
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
		int lower_bound = this->lower_bound_for_partial_solution(current_solution_size, current_solution);

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
	this->current_best_cost = INT_MAX;
	this->current_best_solution = new int[n];
	std::fill(current_best_solution, current_best_solution+n, -1);
}

// TO-DO
int QAPBranch::lower_bound_for_partial_solution(int partial_solution_size, int* partial_solution)
{
	// just to avoid parameter-not-used warnings!!!!!!!!!!
	return partial_solution_size+partial_solution[0];
}

int* QAPBranch::get_current_best_solution()
{
	return this->current_best_solution;
}

int QAPBranch::get_current_best_cost()
{
	return this->current_best_cost;
}