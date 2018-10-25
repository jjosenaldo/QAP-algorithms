#include "qap.h"

QAP::QAP(int n, int** d_mat, int** f_mat)
{
	this->n = n;
	this->d_mat = d_mat;
	this->f_mat = f_mat;
}

QAP::QAP(int n, int** d_mat, int** f_mat, int best_cost)
{
	this->n = n;
	this->d_mat = d_mat;
	this->f_mat = f_mat;
	this->best_cost = best_cost;
}

QAP::QAP(int n, int** d_mat, int** f_mat, int* best_solution, int best_cost)
{
	this->n = n;
	this->d_mat = d_mat;
	this->f_mat = f_mat;
	this->best_solution = best_solution;
	this->best_cost = best_cost;
}

QAP::~QAP()
{
	delete[] this->best_solution;
}

int QAP::get_number_of_facilities()
{
	return this->n;
}

int** QAP::get_matrix_of_flows()
{
	return this->f_mat;
}

int** QAP::get_matrix_of_distances()
{
	return this->d_mat;
}

int* QAP::get_best_solution()
{
	return this->best_solution;
}

void QAP::set_best_solution(int* new_best_solution)
{
	this->best_solution = new_best_solution;
}

int QAP::get_best_cost()
{
	return this->best_cost;
}

void QAP::set_best_cost(int new_best_cost)
{
	this->best_cost = new_best_cost;
}

int QAP::calculate_cost_of_solution(int* solution)
{
	int cost = 0;
	for (int i=0; i < this->n; i++)
		cost += d_mat[i][solution[i]]*f_mat[i][solution[i]];
	return cost;
}