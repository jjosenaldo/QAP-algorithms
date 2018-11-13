#include "GA_QAP.h"

GA_QAP::GA_QAP(QAP *problem, int size_initial_population)
{
	this->size_initial_population = size_initial_population;
	this->size_problem = problem->get_number_of_facilities();

	chromossome = new int[size_problem];
	this->generate_initial_population();
}

GA_QAP::~GA_QAP()
{
	std::cout << population.size();
	delete[] chromossome;
	for (unsigned int i=0; i < population.size(); i++)
		delete[] population[i];
}

void GA_QAP::generate_initial_population()
{
	std::cout << "size: " << this->size_problem << std::endl;

	for (int i=0; i < this->size_problem; i++)
		chromossome[i] = i;

	for (int i=0; i < this->size_initial_population; i++)
	{
		int* new_individual = new int[this->size_problem];
		std::copy(chromossome, chromossome + this->size_problem, new_individual);
		std::random_shuffle(new_individual, new_individual + this->size_problem);
		population.push_back(new_individual);		
	}
}

void GA_QAP::print_population()
{
	for (unsigned int i=0; i < population.size(); i++ )
	{
		for (int j=0; j < size_problem; j++)
			std::cout << population[i][j] << " ";
		std::cout << std::endl;
	}
}

void GA_QAP::run()
{
	this->print_population();
}