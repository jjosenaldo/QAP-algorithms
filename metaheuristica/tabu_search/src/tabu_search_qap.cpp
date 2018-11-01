#include "tabu_search_qap.h"
#include <algorithm> // copy(), 
#include <climits> // INT_MAX
#include <iostream>

/*Constructor*/

Operation::Operation() : Operation(-1, 1){ }

Operation::Operation(int i, int j) {this->set(i, j); }

int Operation::get_first() {return this->first; }

int Operation::get_second() {return this->second; }

void Operation::set_second(int i) {this->set(this->first, i);}

void Operation::set_first(int i) {this->set(i, this->second);}

void Operation::set(int i, int j)
{
	if(i == j)
	{
		std::cout << "ERROR: i = j = " << i << " on Operation::set()\n";
		std::exit(0);
	}

	this->first = std::min(i, j);
	this->second = std::max(i, j);
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
	matrix.reserve(this->n - 1);
	int row_size;

	for(int i = 0; i < this->n - 1; ++i)
	{
		row_size = this->n - i - 1;

		matrix[i].reserve(row_size);

		for(int j = 0; j < row_size;  ++j)
			matrix[i].push_back(-1);
	}
}

bool TsQAP::is_tabu(Operation operation)
{
	int tabu_iteration = this->last_seen_matrix[operation.get_first()][operation.get_second()];

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

	for(int i = 0; i < n; ++i)
	{
		for(int j = i+1; j < n; ++j)
		{
			int neighbor_cost = this->delta_matrix[i][j];

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

	if(best_tabu_delta < best_non_tabu_delta)
		return best_non_tabu_neighbor;

	else if(this->satisfies_aspiration_criteria1(best_tabu_delta + this->fitness_current_solution))
		return best_tabu_neighbor;

	else
		return best_non_tabu_neighbor;
}

int TsQAP::delta_value_constant(int i, int j, int p, int q)
{
	int old_delta = this->delta_matrix[i][j];
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
	int p = operation.get_first(), q = operation.get_second();

	this->delta_matrix[p][q] = -this->delta_matrix[p][q];
	int new_value;

	for(int i = 0; i < this->n-1; ++i)
	{
		for(int j = i+1; j < n; ++j)
		{
			if((i == p && j == q) || (i == q && j == p))
				new_value = this->delta_matrix[i][j];

			else if(i == p || j == q || i == q || j == p)
				new_value = this->delta_value_linear(i, j);

			else
				new_value = this->delta_value_constant(i, j, p, q);

			this->delta_matrix[i][j] = new_value;
		}	
	}
}

Operation TsQAP::init_delta_matrix()
{
	this->init_matrix(this->delta_matrix);

	int least_delta = INT_MAX;
	Operation least_neighbor = Operation();

	for(int i = 0; i < this->n - 1; ++i)
	{
		for(int j = i+1; j < this->n; ++j)
		{	
			int delta = this->delta_value_linear(i, j);

			this->delta_matrix[i][j] = delta;

			if(delta < least_delta)
			{
				least_delta = delta;
				least_neighbor.set_first(i);
				least_neighbor.set_second(j);
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

	/*int* hardcoded = new int[this->n];
	hardcoded[0] = 1;
	hardcoded[1] = 0;
	hardcoded[2] = 3;
	hardcoded[3] = 2;
	this->hardcode_solution(hardcoded);*/

	// sets the fitnesses
	int initial_fitness = this->problem->calculate_cost_of_solution(this->current_solution);
	this->set_fitness_current_solution(initial_fitness);
	this->set_fitness_current_best_solution(initial_fitness);

	// std::cout << "Initial solutioh, with fitness = " << initial_fitness << ":\n";
	// for(int i = 0; i < this->n; ++i)
	// 	std::cout << this->current_solution[i] << " ";
	// std::cout << std::endl << std::endl;
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

int TsQAP::calculate_fitness (int* solution) {	return solution[0]*10; }

bool TsQAP::satisfies_aspiration_criteria1(int cost) { return cost < this->fitness_current_best_solution; }

void TsQAP::apply_operation(Operation operation, int* permutation) { std::iter_swap(permutation + operation.get_first(), permutation + operation.get_second()); }

int TsQAP::get_delta(Operation operation){ return this->delta_matrix[operation.get_first()][operation.get_second()];}

void TsQAP::run()
{
	this->generate_initial_solution();
	Operation best_neighbor_swap = this->init_delta_matrix();

	// this->print_delta_matrix();std::cout << std::endl;
	// std::cout << "Best swap: <" << best_neighbor_swap.first.first 
	// 		<< ", " << best_neighbor_swap.first.second << ">, with delta = "
	// 		<< best_neighbor_swap.second << " and new fitness = " 
	// 		<< (best_neighbor_swap.second+this->fitness_current_solution) << std::endl << std::endl;
	int neighbor_delta = this->get_delta(best_neighbor_swap);

	this->apply_operation(best_neighbor_swap, this->current_solution);
	this->increment_fitness_current_solution(neighbor_delta);

	// Se o primeiro melhor vizinho é melhor que a soulução inicial,
	// então a solução melhor é setada com esse vizinho. 
	if(neighbor_delta < 0)
	{
		this->apply_operation(best_neighbor_swap, this->current_best_solution);
		this->increment_fitness_current_best_solution(neighbor_delta);
	}

	this->set_last_seen(best_neighbor_swap);

	const int MAX_ITERATIONS = 100*this->n;
	const int MAX_ITERATIONS_NOT_IMPROVED = 30*this->n;
	int iterations_not_improved = 0;

	while (this->current_iteration < MAX_ITERATIONS && iterations_not_improved++ < MAX_ITERATIONS_NOT_IMPROVED)
	{
		// std::cout << "Current iteration: " << current_iteration << "\n####################################################################### " << std::endl;
		// std::cout << "Current solution: ";
		// for(int i = 0; i < this->n; ++i) std::cout << this->current_solution[i] <<" ";
		// std::cout << "\nFitness of the best solution: " << this->fitness_current_best_solution << std::endl;
		// std::cout << "Fitness of the current solution: " << this->fitness_current_solution << std::endl;
		// // this->print_tabu_list();

		// evaluates the neighborhood
		this->update_delta_matrix(best_neighbor_swap);

		best_neighbor_swap = this->get_best_neighbor();
		neighbor_delta = this->get_delta(best_neighbor_swap);

		// this->print_delta_matrix();std::cout << std::endl;
		// this->print_naive_delta_matrix();std::cout << std::endl;

		// std::cout << "Best swap: <" << best_neighbor_swap.first.first 
		// 	<< ", " << best_neighbor_swap.first.second << ">, with delta = "
		// 	<< best_neighbor_swap.second << " and new fitness = " 
		// 	<< (best_neighbor_swap.second+this->fitness_current_solution) << std::endl;

		// updates current solution
		this->apply_operation(best_neighbor_swap, this->current_solution);
		this->increment_fitness_current_solution(neighbor_delta);

		if(this->fitness_current_solution < this->fitness_current_best_solution)
		{
			this->apply_operation(best_neighbor_swap, this->current_best_solution);
			this->increment_fitness_current_best_solution(neighbor_delta);
			iterations_not_improved = 0;
		}

		else 
			++iterations_not_improved;

		this->set_last_seen(best_neighbor_swap);
		++this->current_iteration;
	}

}


void TsQAP::hardcode_solution(int* solution) { std::copy(solution, solution+n, this->current_solution); }

void TsQAP::increment_fitness_current_solution(int delta) { this->fitness_current_solution += delta; }

void TsQAP::increment_fitness_current_best_solution(int delta) { this->fitness_current_best_solution += delta; }

/*Getters and Setters*/

int* TsQAP::get_current_best_solution() { return current_best_solution; }

int* TsQAP::get_current_solution() { return current_solution; }

int TsQAP::get_max_size_tabu_list() { return max_size_tabu_list; }

void TsQAP::set_max_size_tabu_list(int new_max_size) { this->max_size_tabu_list = new_max_size; }

QAP* TsQAP::get_instance_qap () { return this->problem; }

int TsQAP::get_fitness_current_solution() { return this->fitness_current_solution; }

int TsQAP::get_fitness_current_best_solution() { return fitness_current_best_solution; }

int TsQAP::get_n() { return this->n; }

void TsQAP::set_fitness_current_solution(int new_fitness) { this->fitness_current_solution = new_fitness; }

void TsQAP::set_fitness_current_best_solution(int new_fitness) { this->fitness_current_best_solution = new_fitness;	 }

void TsQAP::print_delta_matrix()
{
	std::cout << "DELTA_MATRIX\n";
	for(int i = 0; i < this->n; ++i)
	{
		for(int j = 0; j < this->n; ++j)
			std::cout << this->delta_matrix[i][j] << " ";
		std::cout << std::endl;
	}
}

void TsQAP::print_naive_delta_matrix()
{
	std::cout << "NAIVE DELTA_MATRIX (but the correct one)\n";
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

void TsQAP::set_last_seen(Operation operation) { this->last_seen_matrix[operation.get_first()][operation.get_second()] = this->current_iteration; }

void TsQAP::set_last_seen(int i, int j) { this->set_last_seen(Operation(i, j)); }

std::string TsQAP::get_instance_name () { return this->instance_name; }

bool pair_equals(std::pair<int, int> p1, std::pair<int, int> p2) { return p1 == p2 or (p1.first == p2.second and p1.second == p2.first); }
