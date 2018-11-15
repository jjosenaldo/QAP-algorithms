#include "GA_QAP.h"

void print_solution (int* solution, int size_problem)
{
	for (int i=0; i < size_problem; i++)
		std::cout << solution[i] << " ";
	std::cout << std::endl;
}

std::vector<int> raffle(int lower_bound, int upper_bound, int amount )
{
	std::vector<int> values;
	for (int i = lower_bound; i < upper_bound; i++)
		values.push_back(i);
	std::random_shuffle(values.begin(), values.end());

	std::vector<int> drawn;
	for (int i=0; i < amount; i++)
		drawn.push_back(values[i]);
	return drawn;
}

GA_QAP::GA_QAP(QAP *problem, int size_initial_population)
{
	this->size_initial_population = size_initial_population;
	this->size_problem = problem->get_number_of_facilities();
	this->problem = problem;
	this->mutation_rate = 20;
	this->improved = false;

	chromossome = new int[size_problem];
	this->generate_initial_population();

	/** iniciar melhor solução atual */
	this->current_best_individual = this->population[0];
	this->fitness_of_current_best_individual = 
		this->problem->calculate_cost_of_solution(this->current_best_individual);
	
	/** Buscar melhor indivíduo da população gerada*/
	for (unsigned int i=1; i < this->population.size(); i++)
		this->is_dominate(this->population[i]);
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

	for (int i=0; i < this->size_initial_population; i++)
	{
		int* new_individual = new int[this->size_problem];
		std::copy(chromossome, chromossome + this->size_problem, new_individual);
		std::random_shuffle(new_individual, new_individual + this->size_problem);
		population.push_back(new_individual);		
	}
}

bool GA_QAP::is_dominate(int* individual)
{
	int fitness = this->problem->calculate_cost_of_solution(individual);
		
	if (fitness < this->fitness_of_current_best_individual)
	{
		current_best_individual = individual;
		fitness_of_current_best_individual = fitness;
		this->improved = true;
		return true;
	}
	return false;
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
	int index_first = rand(this->population.size()-1);
	int index_second = rand(this->population.size());

	if(index_second == index_first)
		index_second = this->population.size() - 1;

	int* new_individual = this->crossover(population[index_first], population[index_second]);

	this->population.push_back(new_individual);
	this->is_dominate(new_individual);
	return new_individual;
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

void GA_QAP::mutation()
{
	int total = (this->mutation_rate/100.00) * this->population.size();
	std::vector<int> positions = raffle(0, this->population.size()-1, total );

	for (unsigned int i = 0; i < positions.size(); i++)
	{
		std::vector<int> positions_to_swap = raffle(0, this->size_problem, 2);
		
		/** Trocar os genes selecionados */
		int aux = population[positions[i]][positions_to_swap[0]];
		population[positions[i]][positions_to_swap[0]] = population[positions[i]][positions_to_swap[1]];
		population[positions[i]][positions_to_swap[1]] = aux;

		this->is_dominate(population[i]);
	}

	if (this->mutation_rate > 2)
		this->mutation_rate--;

}

int* GA_QAP::crossover(int* father, int* mother)
{
	// /** Imprimir parâmetos iniciais */
	// std::cout << "indivíduos escolhidos\n";
	// print_solution(father, size_problem);
	// print_solution(mother, size_problem);

	/** Sortear uma posição aleatória para iniciar */
	std::vector<int> temp = raffle(0, size_problem, 1);
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
		for (int i=0; i < size_problem; i++)
			this->verify_condition_path_swap(child1, child2, i);
	 

	// std::cout << "child1 gerada: ";
	// print_solution(child1, size_problem);
	// std::cout << "child2 gerada: ";
	// print_solution(child2, size_problem);

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

void GA_QAP::improve_solution ( int* solution )
{
	/**
	 * TODO
	 * Monta a vizinhaça de uma solução
	 * Busca o melhor vizinho dessa solução
	 * Troca a solução atual por seu melhor vizinho encontrado
	 */
}

void GA_QAP::local_optimization ()
{
	int individuals = 0.2 * this->population.size();
	
	int cont = individuals;
	int i = position_local_optimization;
	
	while (cont > 0)
	{
		if (i == (int) this->population.size())
			i = 0;

		this->improve_solution( this->population[i] );

		cont--;
		i++;
	}

	this->position_local_optimization = i;
}

void GA_QAP::run()
{
	int count_mutation = 0;
	int count_stop = 0;

	bool stopping_condition = false;
	
	while (!stopping_condition)
	{
		this->selection();
		
		if (this->improved)
		{
			this->improved = false;
			count_stop = 0;
		}else count_stop++;

		if (count_mutation == 10)
		{
			this->mutation();
			count_mutation = 0;
		}else count_mutation++;

		if(count_stop >= (100*this->size_problem))
			stopping_condition = true;		
	}

	// std::cout << "The best solution found: ";
	// print_solution(this->current_best_individual, size_problem);
	// std::cout << "Cost of best solution found: ";
	// std::cout << fitness_of_current_best_individual << std::endl;
}


int GA_QAP::get_fitness_current_best_solution()
{
	return this->fitness_of_current_best_individual;
}
