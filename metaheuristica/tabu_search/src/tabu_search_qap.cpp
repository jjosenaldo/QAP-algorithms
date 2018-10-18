#include "tabu_search_qap.h"

TsQAP::TsQAP(int* current_best_solution, int max_size)
{
	this->current_best_solution = current_best_solution;
	this->max_size_tabu_list = max_size;
}

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