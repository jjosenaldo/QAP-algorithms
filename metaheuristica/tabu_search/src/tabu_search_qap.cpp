#include "tabu_search_qap.h"

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
		(f[pi[p]][[j]] - f[pi[p]][[i]] + f[pi[q]][[i]] - f[pi[q]][[j]])
		+ (d[i][p] - d[j][p] + d[j][q] - d[i][q]) *
		(f[pi[j]][[p]] - f[pi[i]][[p]] + f[pi[i]][[q]] - f[pi[j]][[q]]);

	return value;
}

int TsQAP::delta_value_linear(int i, int j)
{
	int** f = this->problem->get_matrix_of_flows();
	int** d = this->problem->get_matrix_of_distances();
	int* pi = this->best_candidate;
	
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

void TsQAP::update_delta_matrix(Pair op)
{
	int n = this->problem->get_number_of_facilities();
	int p = op.i, q = op.j;

	this->delta_matrix[p][q] = -this->delta_matrix[p][q];

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

void TsQAP::init_delta_matrix()
{
	int n = this->problem->get_number_of_facilities();

	this->delta_matrix = new int*[n];

	for(int i = 0; i < n; ++i)
	{
		this->delta_matrix[i] = new int[n];

		for(int j = 0; j < n; ++j)
			this->delta_matrix[i][j] = this->delta_value_linear(i, j);
	}

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
	
	Pair op;
	int aux;
	for (int i = 0; i < n/2; i+=2)
	{
		op.i = mutation[i];
		op.j = mutation[i+1];

		if ( not isForbidden(op) )
		{
			// realize mutation
			int* new_solution = solution;
			aux = new_solution[op.i];
			new_solution[op.i] = new_solution[op.j];
			new_solution[op.j] = aux;
		
			neighbors.push_back(new_solution);
		} 
	}

	return neighbors;
}

int TsQAP::calculate_fitness (int* solution)
{	
	return solution[0]*10;
}

bool TsQAP::isForbidden(Pair operation)
{
	for (unsigned int i=0; i < this->tabu_list.size(); i++)
		if (tabu_list[i] == operation)
			return true;
	return false;
}

void TsQAP::run()
{
	this->generate_inicial_solution(this->get_instance_qap()->get_number_of_facilities());
	this->set_best_candidate(this->get_current_best_solution());
	this->init_delta_matrix();

	bool stoppingCondition = false;
	int cont = 0;

	while (not stoppingCondition)
	{
		std::vector<int*> neighbors = this->get_unforbidden_neighbors(this->best_candidate);
		for (unsigned int i=0; i < neighbors.size(); i++)
			if ( this->calculate_fitness(neighbors[i]) > this->fitness_best_candidate )
				this->set_best_candidate(neighbors[i]);

		if (this->fitness_best_candidate > this->fitness_current_best_solution)
			this->set_current_best_solution(this->best_candidate);
		else cont++;

		if (cont == 30) stoppingCondition = true;
	}

}

/*Getters and Setters*/

int* TsQAP::get_current_best_solution()
{
	return current_best_solution;
}

void TsQAP::set_current_best_solution(int* new_best_solution)
{
	this->current_best_solution = new_best_solution;
	this->fitness_current_best_solution = this->calculate_fitness(new_best_solution);
}

int* TsQAP::get_best_candidate()
{
	return best_candidate;
}

void TsQAP::set_best_candidate(int* new_best_candidate)
{
	this->best_candidate = new_best_candidate;
	this->fitness_best_candidate = this->calculate_fitness(new_best_candidate);
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