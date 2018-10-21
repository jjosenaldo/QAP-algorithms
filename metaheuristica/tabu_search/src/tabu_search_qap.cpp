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
	std::vector<int*> vector;
	vector.push_back(solution);
	return vector;
}

int TsQAP::calculate_fitness (int* solution)
{	
	return solution[0]*10;
}

void TsQAP::run()
{
	this->generate_inicial_solution(this->get_instance_qap()->get_number_of_facilities());
	this->set_best_candidate(this->get_current_best_solution());

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