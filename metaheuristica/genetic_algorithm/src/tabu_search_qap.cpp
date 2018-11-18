#include "tabu_search_qap.h"
#include <algorithm> // copy(), 
#include <climits> // INT_MAX
#include <iostream>

/*Constructor*/

Operation::Operation() : Operation(-10000, 10000){ }

Operation::Operation(int i, int j) {this->set(i, j); }

int Operation::get_min() {return this->miN; }

int Operation::get_max() {return this->maX; }

void Operation::set(int i, int j)
{
	if(i == j)
	{
		std::cout << "ERROR: i = j = " << i << " on Operation::set()\n";
		std::exit(0);
	}

	else if(i < j)
	{
		this->miN = i;
		this->maX = j;
	}

	else
	{
		this->miN = j;
		this->maX = i;
	}
}

TsQAP::TsQAP(QAP *instance, int max_size, std::string instance_name)
{
	this->instance_name = instance_name;
	this->problem = instance;
	this->n = instance->get_number_of_facilities();
	this->max_size_tabu_list = max_size;
	this->current_iteration = 1;
	this->current_solution = new int[n];
	this->current_best_solution = new int[n];

	this->init_matrix(this->last_seen_matrix);
}

TsQAP::~TsQAP()
{
	delete[] this->current_best_solution;
	delete[] this->current_solution;
}

void TsQAP::init_matrix(std::vector<std::vector<int>>& matrix)
{
	matrix.reserve(this->n);

	for(int i = 0; i < this->n; ++i)
	{
		std::vector<int> line;
		line.reserve(i);

		for(int j = 0; j < i;  ++j)
			line.push_back(-1);	

		matrix.push_back(line);
	}
}

bool TsQAP::is_tabu(Operation operation)
{
	int tabu_iteration = this->last_seen_matrix[operation.get_max()][operation.get_min()];

	if(tabu_iteration == -1)
		return false;

	else
		return this->current_iteration - tabu_iteration <= this->max_size_tabu_list;
}

bool TsQAP::is_tabu(int i, int j) { return this->is_tabu(Operation(i, j));}

Operation TsQAP::get_best_neighbor()
{
	Operation best_non_tabu_neighbor = Operation();
	Operation best_tabu_neighbor = Operation();

	int best_non_tabu_delta = INT_MAX;
	int best_tabu_delta = INT_MAX;

	int neighbor_cost, i, j;

	for(auto it_line = this->delta_matrix.begin(); it_line != this->delta_matrix.end(); ++it_line)
	{
		for(auto it_col = it_line->begin(); it_col != it_line->end(); ++it_col)
		{
			neighbor_cost = *it_col;
			i = it_line - this->delta_matrix.begin();
			j = it_col - it_line->begin();

			if(this->is_tabu(i, j))
			{
				if(neighbor_cost < best_tabu_delta)
				{
					best_tabu_neighbor.set(i, j);
					best_tabu_delta = neighbor_cost;
				}	
			}

			else
			{
				if(neighbor_cost < best_non_tabu_delta)
				{
					best_non_tabu_neighbor.set(i, j);
					best_non_tabu_delta = neighbor_cost;
				}	
			}
		}
	}

	if(best_non_tabu_delta < best_tabu_delta)
		return best_non_tabu_neighbor;

	else if(this->satisfies_aspiration_criteria1(best_tabu_delta + this->fitness_current_solution))
		return best_tabu_neighbor;

	else
		return best_non_tabu_neighbor;
}

int TsQAP::delta_value_constant(int i, int j, int p, int q)
{
	int old_delta = this->get_delta(Operation(i, j));
	int** f = this->problem->get_matrix_of_flows();
	int** d = this->problem->get_matrix_of_distances();
	int* pi = this->current_solution;

	int value = old_delta + (d[p][i] - d[p][j] + d[q][j] - d[q][i]) *
		(f[pi[p]][pi[j]] - f[pi[p]][pi[i]] + f[pi[q]][pi[i]] - f[pi[q]][pi[j]])
		+ (d[i][p] - d[j][p] + d[j][q] - d[i][q]) *
		(f[pi[j]][pi[p]] - f[pi[i]][pi[p]] + f[pi[i]][pi[q]] - f[pi[j]][pi[q]]);

	return value;
}

int TsQAP::delta_value_linear(int i, int j)
{
	int** f = this->problem->get_matrix_of_flows();
	int** d = this->problem->get_matrix_of_distances();
	int* pi = this->current_solution;
	
	int value = 0;

	for(int k = 0; k < this->n; ++k)
	{
		if(k != i && k != j)
		{
			value += (d[k][i]-d[k][j]) * (f[pi[k]][pi[j]] - f[pi[k]][pi[i]])
				+ (d[i][k] - d[j][k]) * (f[pi[j]][pi[k]] - f[pi[i]][pi[k]]);
		}
	}
	value += (d[i][j] - d[j][i]) * (f[pi[j]][pi[i]] - f[pi[i]][pi[j]]);

	return value;
}

void TsQAP::update_delta_matrix(Operation operation)
{
	int p = operation.get_max(), q = operation.get_min();
	Operation index = Operation(p, q);

	this->set_delta(index, -this->get_delta(index));

	int new_value, i, j;

	for(auto it_line = this->delta_matrix.begin(); it_line != this->delta_matrix.end(); ++it_line)
	{
		for(auto it_col = it_line->begin(); it_col != it_line->end(); ++it_col)
		{
			i = it_line - this->delta_matrix.begin();
			j = it_col - it_line->begin();
			index.set(i, j);

			if((i == p && j == q) || (i == q && j == p))
				new_value = this->get_delta(index);

			else if(i == p || j == q || i == q || j == p)
				new_value = this->delta_value_linear(i, j);

			else
				new_value = this->delta_value_constant(i, j, p, q);

			this->set_delta(index, new_value);
		}
	}
}

Operation TsQAP::init_delta_matrix()
{
	this->init_matrix(this->delta_matrix);

	int delta, least_delta = INT_MAX, i, j;

	Operation least_neighbor = Operation();
	Operation index = Operation();

	for(auto it_line = this->delta_matrix.begin(); it_line != this->delta_matrix.end(); ++it_line)
	{
		for(auto it_col = it_line->begin(); it_col != it_line->end(); ++it_col)
		{
			i = it_line - this->delta_matrix.begin();
			j = it_col - it_line->begin();
			index.set(i, j);

			delta = this->delta_value_linear(i, j);
			this->set_delta(index, delta);

			if(delta < least_delta)
			{
				least_delta = delta;
				least_neighbor.set(i, j);
			}
		}
	}

	return least_neighbor;

}

/*Principal methods*/

void TsQAP::generate_initial_solution()
{
	// generate initial "current solution" randomly
	for(int i = 0; i < this->n; ++i)
		this->current_solution[i] = i;

	std::random_shuffle(this->current_solution, this->current_solution + this->n);

	// copies the initial solution into the best one
	std::copy(this->current_solution, this->current_solution+this->n, this->current_best_solution);

	// sets the fitnesses
	int initial_fitness = this->problem->calculate_cost_of_solution(this->current_solution);
	this->set_fitness_current_solution(initial_fitness);
	this->set_fitness_current_best_solution(initial_fitness);

	std::cout << "Initial solution, with fitness = " << initial_fitness << ":\n";
	for(int i = 0; i < this->n; ++i)
		std::cout << this->current_solution[i] << " ";
	 std::cout << std::endl;
}

void TsQAP::generate_initial_solution( int * solution)
{
	// generate initial "current solution" randomly
	for(int i = 0; i < this->n; ++i)
		this->current_solution[i] = solution[i];

	// copies the initial solution into the best one
	std::copy(this->current_solution, this->current_solution+this->n, this->current_best_solution);

	// sets the fitnesses
	int initial_fitness = this->problem->calculate_cost_of_solution(this->current_solution);
	this->set_fitness_current_solution(initial_fitness);
	this->set_fitness_current_best_solution(initial_fitness);

	std::cout << "Initial solution, with fitness = " << initial_fitness << ":\n";
	for(int i = 0; i < this->n; ++i)
		std::cout << this->current_solution[i] << " ";
	 std::cout << std::endl;
}

// std::vector<int*> TsQAP::get_unforbidden_neighbors(int* solution)
// {
// 	std::vector<int*> neighbors;
// 	int n = this->problem->get_number_of_facilities();

// 	int* mutation = 0;
// 	for (int i = 0; i < n; i++)
// 		mutation[i] = i;

// 	std::random_shuffle(mutation, mutation + n);
	
// 	std::pair<int,int> op;
// 	int aux;
// 	for (int i = 0; i < n/2; i+=2)
// 	{
// 		op.first = mutation[i];
// 		op.second = mutation[i+1];

// 		if ( not is_forbidden(op) )
// 		{
// 			// realize mutation
// 			int* new_solution = solution;
// 			aux = new_solution[op.first];
// 			new_solution[op.first] = new_solution[op.second];
// 			new_solution[op.second] = aux;
		
// 			neighbors.push_back(new_solution);
// 		} 
// 	}

// 	return neighbors;
// }

bool TsQAP::satisfies_aspiration_criteria1(int cost) { return cost < this->fitness_current_best_solution; }

void TsQAP::apply_operation(Operation operation, int* permutation) { std::iter_swap(permutation + operation.get_max(), permutation + operation.get_min()); }

int TsQAP::get_delta(Operation operation){ return this->delta_matrix[operation.get_max()][operation.get_min()];}

void TsQAP::set_delta(Operation operation, int val){ this->delta_matrix[operation.get_max()][operation.get_min()] = val;}

void TsQAP::run(int * solution)
{
	this->generate_initial_solution(solution);
	Operation best_neighbor_swap;

	int neighbor_delta;

	const int MAX_ITERATIONS = 100*this->n;
	const int MAX_ITERATIONS_NOT_IMPROVED = 30*this->n;
	int iterations_not_improved = 0;
	bool first_iteration = true;

	while (this->current_iteration < MAX_ITERATIONS && iterations_not_improved < MAX_ITERATIONS_NOT_IMPROVED)
	{
		// std::cout << "#######################################################################\n";
		// std::cout << "Current iteration: " << current_iteration  << std::endl;
		// std::cout << "Current solution: ";
		// for(int i = 0; i < this->n; ++i) std::cout << this->current_solution[i] <<" ";
		// std::cout << "\nCurrent best solution: ";
		// for(int i = 0; i < this->n; ++i) std::cout << this->current_best_solution[i] <<" ";
		// std::cout << "\nFitness of the current solution: " << this->fitness_current_solution << std::endl;
		// std::cout << "Fitness of the best solution: " << this->fitness_current_best_solution << std::endl;

		// evaluates the neighborhood
		if(first_iteration)
		{
			first_iteration = false;
			best_neighbor_swap = this->init_delta_matrix();
		}

		else
		{
			this->update_delta_matrix(best_neighbor_swap);
			best_neighbor_swap = this->get_best_neighbor();
		}
			

		// std::cout <<  "Delta matrix updated!\n";
		// this->print_delta_matrix();std::cout << std::endl;
		// this->print_naive_delta_matrix();std::cout << std::endl;


		// std::cout << "Best swap: <" << best_neighbor_swap.get_max() 
		// 	<< ", " << best_neighbor_swap.get_min() << ">, with delta = "
		// 	<< this->get_delta(best_neighbor_swap) << " and new fitness = " 
		// 	<< (this->get_delta(best_neighbor_swap)+this->fitness_current_solution) << std::endl;
			
		neighbor_delta = this->get_delta(best_neighbor_swap);

		// updates current solution
		this->apply_operation(best_neighbor_swap, this->current_solution);
		this->increment_fitness_current_solution(neighbor_delta);

		// std::cout << "updated current solution\n";

		if(this->fitness_current_solution < this->fitness_current_best_solution)
		{
			this->set_current_best_solution(this->current_solution);
			this->set_fitness_current_best_solution(this->fitness_current_solution);
			iterations_not_improved = 0;

			// std::cout << "updated best solution\n";
		}

		else 
			++iterations_not_improved;

		this->set_last_seen(best_neighbor_swap);

		// std::cout << "last seen set\n\n";
		++this->current_iteration;
	}

}

void TsQAP::increment_fitness_current_solution(int delta) { this->fitness_current_solution += delta; }

/*Getters and Setters*/

int TsQAP::get_fitness_current_best_solution() { return fitness_current_best_solution; }

void TsQAP::set_fitness_current_solution(int new_fitness) { this->fitness_current_solution = new_fitness; }

void TsQAP::set_fitness_current_best_solution(int new_fitness) { this->fitness_current_best_solution = new_fitness;	 }

void TsQAP::set_current_best_solution(int* new_best_solution) {std::copy(new_best_solution, new_best_solution + this->n, this->current_best_solution);}

void TsQAP::set_current_solution(int* new_current_solution)
{
	std::copy(new_current_solution, new_current_solution + this->n, this->current_solution);	
}

void TsQAP::print_delta_matrix()
{
	std::cout << "DELTA_MATRIX\n";
	for(auto it = this->delta_matrix.begin(); it != this->delta_matrix.end(); ++it)
	{
		for(auto it2 = it->begin(); it2 != it->end(); ++it2)
			std::cout << (*it2) << " ";
		
		std::cout << std::endl;
	}
}

void TsQAP::print_naive_delta_matrix()
{
	// std::cout << "NAIVE DELTA_MATRIX (but the correct one)\n";
	for(int i = 0; i < this->n; ++i)
	{
		for(int j = 0; j < this->n; ++j)
		{
			std::iter_swap(this->current_solution + i, this->current_solution + j);
			std::cout << (this->problem->calculate_cost_of_solution(this->current_solution)-this->fitness_current_solution) << " ";
			std::iter_swap(this->current_solution + i, this->current_solution + j);
		}

		std::cout << std::endl;
	}
}

void TsQAP::set_last_seen(Operation operation) { this->last_seen_matrix[operation.get_max()][operation.get_min()] = this->current_iteration; }

void TsQAP::set_last_seen(int i, int j) { this->set_last_seen(Operation(i, j)); }

std::string TsQAP::get_instance_name () { return this->instance_name; }