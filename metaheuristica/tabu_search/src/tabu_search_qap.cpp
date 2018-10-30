#include "tabu_search_qap.h"
#include <algorithm> // copy(), 
#include <climits> // INT_MAX
#include <iostream>

/*Constructor*/

TsQAP::TsQAP(QAP *instance, int max_size, std::string instance_name)
{
	this->instance_name = instance_name;
	this->problem = instance;
	this->n = instance->get_number_of_facilities();
	this->max_size_tabu_list = max_size;

	this->delta_matrix = new int*[n];
	for(int i = 0; i < n; ++i)
		this->delta_matrix[i] = new int[n];

	this->current_solution = new int[n];
	this->current_best_solution = new int[n];
}
TsQAP::~TsQAP()
{
	delete[] this->current_best_solution;
	delete[] this->current_solution;

	for(int i = 0; i < this->n; ++i)
		delete[] this->delta_matrix[i];

	delete[] this->delta_matrix;
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

void TsQAP::update_delta_matrix(std::pair<int,int> op)
{
	int n = this->problem->get_number_of_facilities();
	int p = op.first, q = op.second;

	this->delta_matrix[p][q] = -this->delta_matrix[p][q];
	this->delta_matrix[q][p] = this->delta_matrix[p][q];
	int new_value;

	for(int i = 0; i < n; ++i)
	{
		for(int j = 0; j < i; ++j)
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

	for(int i = 0; i < n; ++i)
		for(int j = i+1; j < n; ++j)
			this->delta_matrix[i][j] = this->delta_matrix[j][i];

}

std::pair<std::pair<int, int>, int> TsQAP::init_delta_matrix()
{
	int least_delta = INT_MAX;
	std::pair<int, int> least_neighbor = std::make_pair<int, int>(-1, -1);

	for(int i = 0; i < this->n; ++i)
	{
		for(int j = 0; j < i; ++j)
		{
			if (i == j)
				this->delta_matrix[i][j] = 0;
			else {
				int delta = this->delta_value_linear(i, j);
				this->delta_matrix[i][j] = delta;

				if(delta < least_delta)
				{
					least_delta = delta;
					least_neighbor.first = i;
					least_neighbor.second = j;
				}
			}

		}
	}

	for(int i = 0; i < this->n; ++i)
		for(int j = i+1; j < this->n; ++j)
			this->delta_matrix[i][j] = this->delta_matrix[j][i];

	return std::make_pair(least_neighbor, least_delta);

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

std::vector<int*> TsQAP::get_unforbidden_neighbors(int* solution)
{
	std::vector<int*> neighbors;
	int n = this->problem->get_number_of_facilities();

	int* mutation = 0;
	for (int i = 0; i < n; i++)
		mutation[i] = i;

	std::random_shuffle(mutation, mutation + n);
	
	std::pair<int,int> op;
	int aux;
	for (int i = 0; i < n/2; i+=2)
	{
		op.first = mutation[i];
		op.second = mutation[i+1];

		if ( not is_forbidden(op) )
		{
			// realize mutation
			int* new_solution = solution;
			aux = new_solution[op.first];
			new_solution[op.first] = new_solution[op.second];
			new_solution[op.second] = aux;
		
			neighbors.push_back(new_solution);
		} 
	}

	return neighbors;
}

int TsQAP::calculate_fitness (int* solution)
{	
	return solution[0]*10;
}

bool TsQAP::is_forbidden(std::pair<int,int> operation)
{
	std::unordered_map<std::pair<int,int>, int, pair_hash>::const_iterator got = this->tabu_list.find(operation);
	return !(got == this->tabu_list.end());
}

bool TsQAP::is_forbidden(int i, int j)
{
	return this->is_forbidden(std::make_pair(i, j)) || this->is_forbidden(std::make_pair(j, i));
}

std::pair<std::pair<int, int>, int> TsQAP::get_best_neighbor()
{
	bool better_non_tabu_found = false;
	int n = this->problem->get_number_of_facilities();
	int best_non_tabu_cost_found = INT_MAX;
	int best_tabu_cost_found = INT_MAX;
	std::pair<int, int> best_tabu_neighbor = std::make_pair<int,int>(-1,-1);
	std::pair<int, int> best_non_tabu_neighbor = std::make_pair<int,int>(-1,-1);

	for(int i = 0; i < n; ++i)
	{
		for(int j = 0; j < i && j != i; ++j)
		{
			int neighbor_cost = this->delta_matrix[i][j];

			// já encontramos um vizinho não-tabu melhor que a solução atual:
			// não precisa analisar os vizinhos tabu
			if(better_non_tabu_found)
			{
				if(this->is_forbidden(i,j))
					continue;

				// se o vizinho é o melhor não-tabu já visto
				if(neighbor_cost < best_non_tabu_cost_found)
				{
					best_non_tabu_cost_found = neighbor_cost;
					best_non_tabu_neighbor.first = i;
					best_non_tabu_neighbor.second = j;
				}
			}

			else
			{
				// verifica se o vizinho é tabu
				if(this->is_forbidden(i, j))
				{
					// std::cout << "neighbor <"<<i << ", "<<j<<"> is tabu\n";
					// o vizinho tabu satisfaz o critério de aspiração
					if(this->satisfies_aspiration_criteria1(this->fitness_current_solution + neighbor_cost))
					{
						// se o vizinho tabu é melhor do que o melhor vizinho tabu já visto
						if(neighbor_cost < best_tabu_cost_found)
						{
							best_tabu_neighbor.first = i;
							best_tabu_neighbor.second = j;	
							best_tabu_cost_found = neighbor_cost;
						}
						
					}
				}

				// o vizinho não é tabu
				else
				{
					// std::cout << "neighbor <"<<i << ", "<<j<<"> is NOT tabu\n";
					// se o vizinho é o melhor não-tabu já visto
					if(neighbor_cost < best_non_tabu_cost_found)
					{
						best_non_tabu_cost_found = neighbor_cost;
						best_non_tabu_neighbor.first = i;
						best_non_tabu_neighbor.second = j;

						// se o vizinho é melhor que a solução atual: não precisamos mais olhar
						// para os vizinhos tabu
						if(neighbor_cost < 0)
							better_non_tabu_found = true;
					}
				}
			}

		}
	}

	std::pair<std::pair<int, int>, int> result = std::make_pair(std::make_pair(-1, -1), -1);

	// a non-tabu neighbor is the best if: 
	// 1: it improves the current solution or
	// 2: no tabu neighbors satisfy the aspiration criteria
	if(better_non_tabu_found || !this->satisfies_aspiration_criteria1(best_tabu_cost_found))
	{
		result.first = best_non_tabu_neighbor;
		result.second = best_non_tabu_cost_found;
	}

	else
	{
		result.first = best_tabu_neighbor;
		result.second = best_tabu_cost_found;	
	}

	return result;
}

bool TsQAP::satisfies_aspiration_criteria1(int cost)
{
	return cost < this->fitness_current_best_solution;
}

void TsQAP::run()
{
	this->generate_initial_solution();
	std::pair<std::pair<int, int>, int> best_neighbor_swap = this->init_delta_matrix();

	// this->print_delta_matrix();std::cout << std::endl;
	// std::cout << "Best swap: <" << best_neighbor_swap.first.first 
	// 		<< ", " << best_neighbor_swap.first.second << ">, with delta = "
	// 		<< best_neighbor_swap.second << " and new fitness = " 
	// 		<< (best_neighbor_swap.second+this->fitness_current_solution) << std::endl << std::endl;

	this->set_current_solution(best_neighbor_swap.first);
	this->increment_fitness_current_solution(best_neighbor_swap.second);

	// Se o primeiro melhor vizinho é melhor que a soulução inicial,
	// então a solução melhor é setada com esse vizinho. 
	if(best_neighbor_swap.second < 0)
	{
		this->set_current_best_solution(best_neighbor_swap.first);
		this->increment_fitness_current_best_solution(best_neighbor_swap.second);
	}

	this->add_swap_to_tabu_list(best_neighbor_swap.first);

	const int MAX_ITERATIONS = 100*this->n;
	const int MAX_ITERATIONS_NOT_IMPROVED = 30*this->n;
	int current_iteration = 0;
	int iterations_not_improved = 0;

	while (++current_iteration < MAX_ITERATIONS && iterations_not_improved++ < MAX_ITERATIONS_NOT_IMPROVED)
	{
		// std::cout << "Current iteration: " << current_iteration << "\n####################################################################### " << std::endl;
		// std::cout << "Current solution: ";
		// for(int i = 0; i < this->n; ++i) std::cout << this->current_solution[i] <<" ";
		// std::cout << "\nFitness of the best solution: " << this->fitness_current_best_solution << std::endl;
		// std::cout << "Fitness of the current solution: " << this->fitness_current_solution << std::endl;
		// // this->print_tabu_list();

		// evaluates the neighborhood
		this->update_delta_matrix(best_neighbor_swap.first);

		best_neighbor_swap = this->get_best_neighbor();

		// this->print_delta_matrix();std::cout << std::endl;
		// this->print_naive_delta_matrix();std::cout << std::endl;

		// std::cout << "Best swap: <" << best_neighbor_swap.first.first 
		// 	<< ", " << best_neighbor_swap.first.second << ">, with delta = "
		// 	<< best_neighbor_swap.second << " and new fitness = " 
		// 	<< (best_neighbor_swap.second+this->fitness_current_solution) << std::endl;

		// updates current solution
		this->set_current_solution(best_neighbor_swap.first);
		this->increment_fitness_current_solution(best_neighbor_swap.second);

		if(this->fitness_current_solution < this->fitness_current_best_solution)
		{
			this->set_current_best_solution(best_neighbor_swap.first); 
			this->set_fitness_current_best_solution(this->fitness_current_solution);
			iterations_not_improved = 0;
		}

		else 
			++iterations_not_improved;

		this->add_swap_to_tabu_list(best_neighbor_swap.first);
		// std::cout << std::endl;
	}

}

void TsQAP::add_swap_to_tabu_list(std::pair<int, int> perturbation)
{
	for(std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator it = this->tabu_list.begin(); 
		it != this->tabu_list.end();)
	{
		if(--it->second == 0)
			it = this->tabu_list.erase(it);

		else
			 ++it;
	}

	this->tabu_list[perturbation] = this->max_size_tabu_list;
}

void TsQAP::hardcode_solution(int* solution)
{
	std::copy(solution, solution+n, this->current_solution);
}

void TsQAP::increment_fitness_current_solution(int delta)
{
	this->fitness_current_solution += delta;
}

void TsQAP::increment_fitness_current_best_solution(int delta)
{
	this->fitness_current_best_solution += delta;
}

/*Getters and Setters*/

int* TsQAP::get_current_best_solution()
{
	return current_best_solution;
}

void TsQAP::set_current_best_solution(int* new_best_solution)
{
	std::copy(new_best_solution, new_best_solution+this->n, this->current_best_solution);
}

int* TsQAP::get_current_solution()
{
	return current_solution;
}

void TsQAP::set_current_solution(int* new_current_solution)
{
	std::copy(new_current_solution, new_current_solution+this->n, this->current_solution);
}

void TsQAP::set_current_solution(std::pair<int, int> perturbation)
{
	std::iter_swap(this->current_solution + perturbation.first, this->current_solution + perturbation.second);
}

void TsQAP::set_current_best_solution(std::pair<int, int> perturbation)
{
	std::iter_swap(this->current_best_solution + perturbation.first, this->current_best_solution + perturbation.second);
}

int TsQAP::get_max_size_tabu_list()
{
	return max_size_tabu_list;
}

void TsQAP::set_max_size_tabu_list(int new_max_size)
{
	this->max_size_tabu_list = new_max_size;
}

QAP* TsQAP::get_instance_qap ()
{
	return this->problem;
}

int TsQAP::get_fitness_current_solution()
{
	return this->fitness_current_solution;
}

int TsQAP::get_fitness_current_best_solution()
{
	return fitness_current_best_solution;
}

int TsQAP::get_n()
{
	return this->n;
}

void TsQAP::set_fitness_current_solution(int new_fitness)
{
	this->fitness_current_solution = new_fitness;
}

void TsQAP::set_fitness_current_best_solution(int new_fitness)
{
	this->fitness_current_best_solution = new_fitness;	
}

void TsQAP::print_tabu_list()
{
	// std::cout << "Tabu list: ";
	// for(std::unordered_map<std::pair<int, int>, int, pair_hash>::iterator it = this->tabu_list.begin(); 
	// 	it != this->tabu_list.end(); ++it)
	// 	std::cout << "<" << it->first.first << ", " << it->first.second << "> : " << it->second << std::endl;
	// std::cout << std::endl;
}

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

void TsQAP::add_swap_to_tabu_list(int i, int j)
{
	this->add_swap_to_tabu_list(std::make_pair(i, j));
}

bool pair_equals(std::pair<int, int> p1, std::pair<int, int> p2)
{
	return p1 == p2 or (p1.first == p2.second and p1.second == p2.first);
}

std::string TsQAP::get_instance_name ()
{
	return this->instance_name;
}