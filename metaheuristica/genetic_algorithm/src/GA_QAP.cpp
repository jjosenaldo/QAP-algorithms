#include "GA_QAP.h"

GA_QAP::GA_QAP(QAP *problem, int size_initial_population)
{
	this->size_initial_population = size_initial_population;
	this->size_problem = problem->get_number_of_facilities();

	this->generate_initial_population();
}

GA_QAP::~GA_QAP()
{
	delete[] chromossome;
	for (unsigned int i=0; i < population.size(); i++)
		delete[] population[i];
}

void GA_QAP::generate_initial_population()
{
	for (int i=0; i < this->size_problem; i++)
		chromossome[i] = i;

	std::cout << "size: " << size_problem << std::endl;
	std::cout << "passou\n";

	for (int i=0; i < this->size_initial_population; i++)
	{
		std::random_shuffle(chromossome, chromossome + this->size_problem);
		population.push_back(chromossome);
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