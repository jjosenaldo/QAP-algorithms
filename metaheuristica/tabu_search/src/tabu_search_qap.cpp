#include "tabu_search_qap.h"

/*Constructor*/

TsQAP::TsQAP(QAP *instance, int max_size)
{
	this->problem = instance;
	this->max_size_tabu_list = max_size;
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

std::vector<int*> TsQAP::get_neighbors(int* solution)
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

	bool stoppingCondition = false;
	int cont = 0;

	while (not stoppingCondition)
	{
		std::vector<int*> neighbors = this->get_neighbors(this->best_candidate);
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