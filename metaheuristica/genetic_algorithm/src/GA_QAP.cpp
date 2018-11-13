#include "GA_QAP.h"

void print_solution (int* solution, int size_problem)
{
	for (int i=0; i < size_problem; i++)
		std::cout << solution[i] << " ";
	std::cout << std::endl;
}

GA_QAP::GA_QAP(QAP *problem, int size_initial_population)
{
	this->size_initial_population = size_initial_population;
	this->size_problem = problem->get_number_of_facilities();
	this->problem = problem;

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

int* GA_QAP::selection ()
{
	std::vector<int> temp;
	for (unsigned int i=0; i < population.size(); i++)
		temp.push_back(i);

	std::random_shuffle(temp.begin(), temp.end());	
	return this->crossover(population[temp[0]], population[temp[1]]);
}

void GA_QAP::swap(int num1, int num2, int* individual)
{	
	int pos1 = 0;
	int pos2 = 0;
	
	for (int i=0; i < size_problem; i++)
	{
		if (individual[i] == num1)
			pos1 = i;

		else if (individual[i] == num2)
			pos2 = i;
	}

	int aux = individual[pos1];
	individual[pos1] = individual[pos2];
	individual[pos2] = aux;
}

void GA_QAP::verify_condition_path_swap (int* child1, int* child2, int position)
{
	if (child1[position] != child2[position])
	{
		this->swap(child1[position], child2[position], child1);
		this->swap(child1[position], child2[position], child2);
	}
}


int* GA_QAP::crossover(int* father, int* mother)
{
	/** Imprimir parâmetos iniciais */
	std::cout << "indivíduos escolhidos\n";
	for (int i=0; i < size_problem; i++)
		std::cout << father[i] << " ";
	std::cout << std::endl;

	for (int i=0; i < size_problem; i++)
		std::cout << mother[i] << " ";
	std::cout << std::endl;

	/** Sortear uma posição aleatória para iniciar */
	std::vector<int> temp;
	for (int i=0; i < size_problem; i++)
		temp.push_back(i);
	std::random_shuffle(temp.begin(), temp.end());		
	int position = temp[0];

	/** Metodologia de path swap para crossover */
	int* child1 = new int[this->size_problem];
	int* child2 = new int[this->size_problem];

	std::copy(father, father + this->size_problem, child1);
	std::copy(mother, mother + this->size_problem, child2);

	if (position != 0)
	{
		for (int i=position; i < this->size_problem; i++ )
			this->verify_condition_path_swap(child1, child2, i);
		
		for (int i=0; i < position; i++)
			this->verify_condition_path_swap(child1, child2, i);

	} else
	{
		for (int i=0; i < size_problem; i++)
			this->verify_condition_path_swap(child1, child2, i);
	} 

	std::cout << "child1 gerada: ";
	print_solution(child1, size_problem);
	std::cout << "child2 gerada: ";
	print_solution(child2, size_problem);

	/** Avaliar melhor filho gerado */
	int fitness_child1 = this->problem->calculate_cost_of_solution(child1);
	int fitness_child2 = this->problem->calculate_cost_of_solution(child2);

	if (fitness_child1 <= fitness_child2)
	{
		delete[] child2;
		return child1;
	}
	else
	{
		delete[] child1;
		return child2;
	} 
		
}

void GA_QAP::run()
{
	this->print_population();
	int* aux = new int[this->size_problem];
	aux = this->selection();
	std::cout << "new child generate\n";
	print_solution(aux, size_problem);
	delete[] aux;
}