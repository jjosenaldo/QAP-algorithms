#include "tabu_search_qap.h"
#include <climits> // INT_MAX

/*Constructor*/

TsQAP::TsQAP(QAP *instance, int max_size)
{
	this->problem = instance;
	this->max_size_tabu_list = max_size;
}

int TsQAP::delta_value_constant(int i, int j, int p, int q)
{
	int old_delta = this->delta_matrix[i][j];
	int** f = this->problem->get_matrix_of_flows();
	int** d = this->problem->get_matrix_of_distances();
	int* pi = this->best_candidate;

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
	int* pi = this->best_candidate;
	int n = this->problem->get_number_of_facilities();
	
	int value = 0;

	for(int k = 0; k < n; ++k)
	{
		if(k != i and k != j)
		{
			value += (d[k][i]-d[k][j]) * (f[pi[k]][pi[j]] - f[pi[k]][pi[i]])
				+ (d[i][k] - d[j][k]) * (f[pi[j]][pi[k]] - f[pi[i]][pi[k]]);
		}
	}

	value += (d[i][i]-d[j][j]) * (f[pi[j]][pi[j]] - f[pi[i]][pi[i]])
				+ (d[i][j] - d[j][i]) * (f[pi[j]][pi[i]] - f[pi[i]][pi[j]]);

	return value;
}

void TsQAP::update_delta_matrix(std::pair<int,int> op)
{
	int n = this->problem->get_number_of_facilities();
	int p = op.first, q = op.second;

	this->delta_matrix[p][q] = -this->delta_matrix[p][q];
	this->delta_matrix[q][p] = -this->delta_matrix[p][q];

	for(int i = 0; i < n; ++i)
	{
		for(int j = 0; j < n; ++j)
		{
			if(i == p and j == q)
				continue;

			else if(i == p or j == q)
				this->delta_value_linear(i, j);

			else
				this->delta_value_constant(i, j, p, q);
		}	
		
	}
}

std::pair<int, int> TsQAP::init_delta_matrix()
{
	int n = this->problem->get_number_of_facilities();
	int least_delta = INT_MAX;
	std::pair<int, int> least_neighbor = std::make_pair<int, int>(-1, -1);

	this->delta_matrix = new int*[n];

	for(int i = 0; i < n; ++i)
	{
		this->delta_matrix[i] = new int[n];

		for(int j = 0; j < n; ++j)
		{
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

	return least_neighbor;

}

/*Principal methods*/

void TsQAP::generate_inicial_solution (int size_solution)
{
	int* solution = this->get_current_best_solution();
	for (int i=0; i < size_solution; i++)
		solution[i] = i;

	std::random_shuffle(solution, solution + size_solution);
	this->set_current_best_solution(solution);
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

		if ( not isForbidden(op) )
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

bool TsQAP::isForbidden(std::pair<int,int> operation)
{
	for (unsigned int i=0; i < this->tabu_list.size(); i++)
		if (tabu_list[i] == operation)
			return true;
	return false;
}

void TsQAP::run()
{
	this->generate_inicial_solution(this->get_instance_qap()->get_number_of_facilities());
	this->set_best_candidate(this->current_best_solution);
	pair<int, int> best_neighbor_swap = this->init_delta_matrix();
	
	this->set_best_candidate(best_neighbor_swap);

	if(this->delta_matrix[best_neighbor_swap.first][best_neighbor_swap.second] < 0)
	{
		this->set_current_best_solution(this->best_candidate);
	}

	this->add_swap_to_tabu_list(best_neighbor_swap);

	bool stoppingCondition = false;
	int cont = 0;

	while (not stoppingCondition)
	{
		// std::vector<int*> neighbors = this->get_unforbidden_neighbors(this->best_candidate);
		// for (unsigned int i=0; i < neighbors.size(); i++)
		// 	if ( this->calculate_fitness(neighbors[i]) > this->fitness_best_candidate )
		// 		this->set_best_candidate(neighbors[i]);
		add_solution_to_tabu_list();

		if (this->fitness_best_candidate > this->fitness_current_best_solution)
			this->set_current_best_solution(this->best_candidate);
		else cont++;

		if (cont == 30) stoppingCondition = true;
	}

}

void TsQAP::add_swap_to_tabu_list(std::pair<int, int> perturbation)
{
	for(std::unordered_map<std::pair<int, int>, int>::iterator it = tabu_list.begin(); 
		it != tabu_list.end(); ++it)
	{
		if(--it->second == 0)
			tabu_list.erase(it);
	}

	tabu_list[perturbation] = this->max_size_tabu_list;
}

/*Getters and Setters*/

int* TsQAP::get_current_best_solution()
{
	return current_best_solution;
}

void TsQAP::set_current_best_solution(int* new_best_solution)
{
	this->current_best_solution = new_best_solution;
}

int* TsQAP::get_best_candidate()
{
	return best_candidate;
}

void TsQAP::set_best_candidate(int* new_best_candidate)
{
	this->best_candidate = new_best_candidate;
}

void TsQAP::set_best_candidate(std::pair<int, int> perturbation)
{
	std::iter_swap(this->best_candidate + perturbation.first, this->best_candidate + perturbation.second);
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

int TsQAP::get_fitness_best_candidate()
{
	return this->fitness_best_candidate;
}

bool pair_equals(std::pair<int, int> p1, std::pair<int, int> p2)
{
	return p1 == p2 or (p1.first == p2.second and p1.second == p2.first);
}