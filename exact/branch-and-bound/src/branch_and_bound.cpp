#include <algorithm> // std::copy, std::fill, std::iter_swap, std::random_shuffle, std::sort
#include <climits> // std:;INT_MAX
#include <iostream> // std::cout 
#include <functional> // std::greater<int>
#include "branch_and_bound.h"
#include "hungarian.h"

QAPBranch::QAPBranch(int n, int** d_mat, int** f_mat)
{
	this->n = n;
	this->d_mat = d_mat;
	this->f_mat = f_mat;
	this->number_of_nodes = 0;

	this->generate_initial_solution();	
	this->calculate_total_nodes();

	this->nonvisited_solutions = new int[n];
	std::fill(nonvisited_solutions, nonvisited_solutions+n, 0);
}

QAPBranch::~QAPBranch()
{
	delete[] this->current_best_solution;
	delete[] this->nonvisited_solutions;
}

void QAPBranch::solve()
{
	int* current_solution = new int[n];

	bool* already_in_solution = new bool[n];
	std::fill(already_in_solution, already_in_solution+n, false);

	this->las_vegas_recursive_search_tree_exploring(0, 0, current_solution, already_in_solution);

	delete[] current_solution;
	delete[] already_in_solution;
}

void QAPBranch::recursive_search_tree_exploring(int current_cost, 
	int current_solution_size, int* current_solution, bool* already_in_solution)
{
	++this->number_of_nodes;
	// std::cout << "\nnode: #" << this->number_of_nodes << "\n";
	// std::cout << "current_cost: " << current_cost << std::endl;
	// std::cout << "best_cost: " << this->current_best_cost << std::endl;
	// std::cout << "solution:";
	// for(int i = 0; i < current_solution_size; ++i) std::cout << " " <<current_solution[i];
	// std::cout << "\n";

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
		int lower_bound;
		bool lower_bound_evaluated = false;

		if(current_solution_size < this->n-1)
		{
			// analyze solution feasibility
			lower_bound = this->lower_bound_for_partial_solution(current_solution_size, already_in_solution, current_cost);
			// std::cout << "lower_bound:" << lower_bound << std::endl;
			lower_bound_evaluated = true;
		}

		// current solution can't get better than the best known so far: its
		// branch must be pruned off
		if(lower_bound_evaluated && lower_bound > this->current_best_cost)
		{
			++this->nonvisited_solutions[current_solution_size];
			// std::cout << "---------------------------------\n";
			// std::cout << "NÃO ABRIU UM NÓ!!!\n";
			// std::cout << "---------------------------------\n";
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
					// for all 0 <= a,b < n such that a,b<=current_solution_size
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
	this->current_best_cost = 0;
	this->current_best_solution = new int[this->n];

	for(int i = 0; i < this->n; ++i)
		this->current_best_solution[i] = i;

	std::random_shuffle(this->current_best_solution, this->current_best_solution + this->n);
	// for(int i = 0; i < this->n; ++i)
	// 	std::cout << this->current_best_solution[i] << " ";
	// std::cout << std::endl;

	for(int i = 0; i < this->n; ++i)
	{
		for(int j = 0; j < this->n; ++j)
		{
			this->current_best_cost += this->f_mat[this->current_best_solution[i]][this->current_best_solution[j]] 
			*this->d_mat[i][j];
		}
	}
}	

// TO-DO
int QAPBranch::lower_bound_for_partial_solution(int partial_solution_size, bool* already_in_solution, int current_partial_cost)
{	
	int remaining_facilities = this->n - partial_solution_size;
	int** new_f = new int*[remaining_facilities];
	int** new_d = new int*[remaining_facilities];
	int* f_diagonal = new int[remaining_facilities];
	int* d_diagonal = new int[remaining_facilities];

	for(int i = 0; i < remaining_facilities; ++i)
	{
		new_f[i] = new int[remaining_facilities-1];
		new_d[i] = new int[remaining_facilities-1];
	}

	int pointer_row = 0, pointer_col;
	for(int i = partial_solution_size; i < this->n; ++i)
	{
		pointer_col = 0;

		for(int j = partial_solution_size; j < this->n; ++j)
			if(i != j)
				new_d[pointer_row][pointer_col++] = this->d_mat[i][j];
			else
				d_diagonal[pointer_row] = this->d_mat[i][j];

		std::sort(new_d[i-partial_solution_size], new_d[i-partial_solution_size] + remaining_facilities -1);

		++pointer_row;
	}

	pointer_row = 0;
	for(int i = 0; i < this->n; ++i)
	{
		if(already_in_solution[i])
			continue;

		pointer_col = 0;

		for(int j = 0; j < this->n; ++j)
			if(!already_in_solution[j])
			{
				if(i != j)
					new_f[pointer_row][pointer_col++] = this->f_mat[i][j];
				else
					f_diagonal[pointer_row] = this->f_mat[i][j];
			}

		std::sort(new_f[pointer_row], new_f[pointer_row] + remaining_facilities-1, std::greater<int>());

		++pointer_row;
	}

	int** min_prod = new int*[remaining_facilities];
	for(int i = 0; i < remaining_facilities; ++i)
	{
		min_prod[i] = new int[remaining_facilities];
		std::fill(min_prod[i], min_prod[i] + remaining_facilities, 0);
	}

	for(int i = 0; i < remaining_facilities; ++i)
		for(int j = 0; j < remaining_facilities; ++j)
			for(int k = 0; k < remaining_facilities-1; ++k)
				min_prod[i][j] += new_d[j][k]*new_f[i][k];

	int** g = new int*[remaining_facilities];

	for(int i = 0; i < remaining_facilities; ++i)
	{
		g[i] = new int[remaining_facilities];

		for(int j = 0; j < remaining_facilities; ++j)
		 	g[i][j] = f_diagonal[i] * d_diagonal[j] + min_prod[i][j];
	}

	int lap = hungarian_least_cost(remaining_facilities, g);
	// std::cout << "lap: " << lap << std::endl;

	for(int i = 0; i < remaining_facilities; ++i)
	{
		delete[] new_f[i];
		delete[] new_d[i];
		delete[] g[i];
		delete[] min_prod[i];
	}

	delete[] new_f;
	delete[] new_d;
	delete[] g;
	delete[] min_prod;
	delete[] f_diagonal;
	delete[] d_diagonal;

	return current_partial_cost+lap;
}

int* QAPBranch::get_current_best_solution()
{
	return this->current_best_solution;
}

int QAPBranch::get_current_best_cost()
{
	return this->current_best_cost;
}

int QAPBranch::get_number_of_nodes()
{
	return this->number_of_nodes;
}

int* QAPBranch::get_non_visited_nodes()
{
	return this->nonvisited_solutions;
}

void QAPBranch::calculate_non_visited_nodes()
{
	this->total_non_visited_nodes = 0;

	for(int p = n-2; p >= 0; --p)
	{
		int total_per_node = 0;
		int parcel = p+1;
		int factor = p;

		for(int i = 1; i <= p+1; ++i)
		{
			total_per_node += parcel;
			parcel *= (factor--);
		}

		this->total_non_visited_nodes += total_per_node * this->nonvisited_solutions[p];
	}
}

int QAPBranch::get_total_non_visited_nodes()
{
	return this->total_non_visited_nodes;
}

bool greater_edge(Edge e1, Edge e2)
{
	return e2 < e1;
}

void QAPBranch::las_vegas_recursive_search_tree_exploring(int current_cost, int current_solution_size, 
										 int* current_solution, bool* already_in_solution)
{
	++this->number_of_nodes;
	// std::cout << "\nnode: #" << this->number_of_nodes << "\n";
	// std::cout << "current_cost: " << current_cost << std::endl;
	// std::cout << "best_cost: " << this->current_best_cost << std::endl;
	// std::cout << "solution:";
	// for(int i = 0; i < current_solution_size; ++i) std::cout << " " <<current_solution[i];
	// std::cout << "\n";

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

			this->las_vegas_recursive_search_tree_exploring(0, 1, current_solution, already_in_solution);

			already_in_solution[i] = false;
		}
		
	}

	// non-empty partial solution
	else
	{
		int lower_bound;
		bool lower_bound_evaluated = false;

		if(current_solution_size < this->n-1)
		{
			// analyze solution feasibility
			lower_bound = this->lower_bound_for_partial_solution(current_solution_size, already_in_solution, current_cost);
			// std::cout << "lower_bound:" << lower_bound << std::endl;
			lower_bound_evaluated = true;
		}

		// current solution can't get better than the best known so far: its
		// branch must be pruned off
		if(lower_bound_evaluated && lower_bound > this->current_best_cost)
		{
			++this->nonvisited_solutions[current_solution_size];
			// std::cout << "---------------------------------\n";
			// std::cout << "NÃO ABRIU UM NÓ!!!\n";
			// std::cout << "---------------------------------\n";
			return;
		}

		// explore the current node's children
		else
		{	
			std::vector<std::pair<int, int>> cost_increases;

			for(int i = 0; i < this->n; ++i)
			{
				if(!already_in_solution[i])
				{
					// compute the cost increase, i.e., the product d_{a,b}*f_{pi(a), pi(b)}
					// for all 0 <= a,b < n such that a,b<=current_solution_size
					int cost_increase = 0;
					for(int j = 0; j < current_solution_size; ++j)
						cost_increase += d_mat[j][current_solution_size]*f_mat[current_solution[j]][i]
									   + d_mat[current_solution_size][j]*f_mat[i][current_solution[j]];

					cost_increases.push_back(std::make_pair(i, cost_increase));
				}
			}

			// order children by cost_increase
			std::sort(cost_increases.begin(), cost_increases.end(), [](auto& p1, auto& p2){
				return p1.second < p2.second;
			});

			int remaining_facilities = this->n - current_solution_size;
			int first_child;

			if(remaining_facilities > 3)
			{
				first_child = rand() % (remaining_facilities / 3);
				std::iter_swap(cost_increases.begin(), cost_increases.begin() + first_child);
			}

			for(std::pair<int, int> child : cost_increases)
			{
				// the i-th facility is assigned to the (current_solution_size)-th location
				current_solution[current_solution_size] = child.first;
				already_in_solution[child.first] = true;

				// explore the subsolution branch
				this->las_vegas_recursive_search_tree_exploring(current_cost + child.second, current_solution_size+1, current_solution, already_in_solution);

				// removes the element of the solution to analyze its siblings
				already_in_solution[child.first] = false;
			}
		}	
	}
}

void QAPBranch::calculate_total_nodes()
{
	long long fator_n = n;
	for (int i=n-1; i >= 1; i--)
		fator_n *= i;

	long long total_nodes = 0;
	long long fator_i;
	for (int i=0; i <= n; i++)
	{
		if ( i == 0 ) fator_i = 1;
		else
		{
			fator_i = i;
			for (int j=i-1; j >= 1; j--)
				fator_i *= j;
		}
		
		total_nodes += fator_n/fator_i;
	}

	this->number_total_of_nodes = total_nodes;
}

double QAPBranch::percential_non_visited_node ()
{
	double percential;

	percential = 100.00 - (100.00 * number_of_nodes)/number_total_of_nodes;
	return percential;
}