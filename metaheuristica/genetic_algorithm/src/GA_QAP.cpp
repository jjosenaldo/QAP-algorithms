#include "GA_QAP.h"
#include "utils.h"

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
	this->mutation_rate = 20;
	this->improved = false;

	this->generate_initial_population();

	/** iniciar melhor solução atual */
	this->current_best_individual = this->population[0];
	this->fitness_of_current_best_individual = 
		this->problem->calculate_cost_of_solution(this->current_best_individual.perm);
	
	/** Buscar melhor indivíduo da população gerada*/
	for (unsigned int i=1; i < this->population.size(); i++)
		this->set_as_best(this->population[i]);
}

void GA_QAP::generate_initial_population()
{
	this->population.reserve(this->size_initial_population);

	int fitness_new_individual;

	for(int i=0; i < this->size_initial_population; i++)
	{
		int* perm_new_individual = new int[this->size_problem];

		for (int i=0; i < this->size_problem; i++)
			perm_new_individual[i] = i;

		std::random_shuffle(perm_new_individual, perm_new_individual + this->size_problem);
		fitness_new_individual = this->problem->calculate_cost_of_solution(perm_new_individual);

		population.push_back(Individual(this->size_problem, perm_new_individual, fitness_new_individual));		
	}
}

void GA_QAP::set_as_best(Individual individual)
{		
	if(individual.fitness < this->fitness_of_current_best_individual)
	{
		current_best_individual = individual;
		fitness_of_current_best_individual = individual.fitness;
		this->improved = true;
	}
}

void GA_QAP::print_population()
{
	for (unsigned int i=0; i < population.size(); i++ )
	{
		for (int j=0; j < size_problem; j++)
			std::cout << population[i].perm[j] << " ";
		std::cout << std::endl;
	}
}

Individual GA_QAP::selection ()
{
	std::vector<int> parents = raffle(0, this->population.size(), 2);
	Individual new_individual = this->crossover(this->population[parents[0]], this->population[parents[1]]);

	if(this->population[parents[0]].fitness < this->population[parents[1]].fitness)
		this->population[parents[1]] = new_individual;

	else
		this->population[parents[0]] = new_individual;

	set_as_best(new_individual);
	return new_individual;
}

void GA_QAP::swap(int num1, int num2, int* perm_individual)
{	
	int pos1 = 0;
	int pos2 = 0;
	
	for (int i=0; i < size_problem; i++)
	{
		if (perm_individual[i] == num1)
			pos1 = i;

		else if (perm_individual[i] == num2)
			pos2 = i;
	}

	int aux = perm_individual[pos1];
	perm_individual[pos1] = perm_individual[pos2];
	perm_individual[pos2] = aux;
}

void GA_QAP::verify_condition_path_swap (int* perm_child1, int* perm_child2, int position)
{
	if (perm_child1[position] != perm_child2[position])
	{
		this->swap(perm_child1[position], perm_child2[position], perm_child1);
		this->swap(perm_child1[position], perm_child2[position], perm_child2);
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
		int aux = population[positions[i]].perm[positions_to_swap[0]];
		population[positions[i]].perm[positions_to_swap[0]] = population[positions[i]].perm[positions_to_swap[1]];
		population[positions[i]].perm[positions_to_swap[1]] = aux;

		this->set_as_best(population[i]);
	}

	if (this->mutation_rate > 2)
		this->mutation_rate--;

}

Individual GA_QAP::crossover(Individual father, Individual mother)
{
	// /** Imprimir parâmetos iniciais */
	// std::cout << "indivíduos escolhidos\n";
	// print_solution(father, size_problem);
	// print_solution(mother, size_problem);

	/** Sortear uma posição aleatória para iniciar */
	std::vector<int> temp = raffle(0, size_problem, 1);
	int position = temp[0];

	/** Metodologia de path swap para crossover */
	int* perm_child1 = new int[this->size_problem];
	int* perm_child2 = new int[this->size_problem];

	std::copy(father.perm, father.perm + this->size_problem, perm_child1);
	std::copy(mother.perm, mother.perm + this->size_problem, perm_child2);

	if (position != 0)
	{
		for (int i=position; i < this->size_problem; i++ )
			this->verify_condition_path_swap(perm_child1, perm_child2, i);
		
		for (int i=0; i < position; i++)
			this->verify_condition_path_swap(perm_child1, perm_child2, i);

	} else
		for (int i=0; i < size_problem; i++)
			this->verify_condition_path_swap(perm_child1, perm_child2, i);
	 

	// std::cout << "child1 gerada: ";
	// print_solution(child1, size_problem);
	// std::cout << "child2 gerada: ";
	// print_solution(child2, size_problem);

	/** Avaliar melhor filho gerado */
	int fitness_child1 = this->problem->calculate_cost_of_solution(perm_child1);
	int fitness_child2 = this->problem->calculate_cost_of_solution(perm_child2);

	if (fitness_child1 <= fitness_child2)
	{
		delete[] perm_child2;
		return Individual(this->size_problem, perm_child1, fitness_child1);
	}
	else
	{
		delete[] perm_child1;
		return Individual(this->size_problem, perm_child2, fitness_child2);
	} 
		
}

void GA_QAP::improve_solution ( int index )
{
	Individual individual = this->population[index];
	int least_neighbor[2];
	least_neighbor[0] = -1;
	least_neighbor[1] = -1;
	int delta, least_delta = INT_MAX;

	for(int i = 0; i < size_problem; ++i)
	{
		for(int j = 0; j < i; ++j)
		{
			delta = this->delta_value_linear(i, j, individual.perm);
			
			if(delta < least_delta)
			{
				least_delta = delta;
				least_neighbor[0] = i;
				least_neighbor[1] = j;
			}
		}
	}

	if(least_delta < 0)
		std::iter_swap(individual.perm +least_neighbor[0], individual.perm +least_neighbor[1]);
}

int GA_QAP::delta_value_linear(int i, int j, int* pi)
{
	int** f = this->problem->get_matrix_of_flows();
	int** d = this->problem->get_matrix_of_distances();
	
	int value = 0;

	for(int k = 0; k < size_problem; ++k)
	{
		if(k != i && k != j)
		{
			value += (d[k][i]-d[k][j]) * (f[pi[k]][pi[j]] - f[pi[k]][pi[i]])
				+ (d[i][k] - d[j][k]) * (f[pi[j]][pi[k]] - f[pi[i]][pi[k]]);
		}
	}

	value += (d[i][j] - d[j][i]) * (f[pi[j]][pi[i]] - f[pi[i]][pi[j]]);

	return value;
}

void GA_QAP::local_optimization (int current_iteration, int MUTATION_PERIOD)
{
	const double PERCENTAGE = 0.2;
	int left = (current_iteration / MUTATION_PERIOD) % (100 / (int)(PERCENTAGE * 100) ) * (int)(PERCENTAGE*100);
	int right = (left+(int)(this->size_initial_population * PERCENTAGE)) % this->size_initial_population;
	
	for(int i = left; i != right; i = (i+1)%this->size_initial_population)
		this->improve_solution( i );
//	std::cout << left << " "<< right << std::endl;
}

void GA_QAP::run()
{
	const int MAX_ITERATIONS_WITHOUT_IMPROVEMENT = 100*this->size_problem;
	const int MUTATION_PERIOD = this->size_problem * 20;

	int cont_mutation = 0;
	int iterations_without_improvement = 0;
	int current_iteration = 0;
	int last_iteration_with_improvement = 0;
	int biggest_delay_between_improvements = 0;
	
	while (iterations_without_improvement <= MAX_ITERATIONS_WITHOUT_IMPROVEMENT)
	{
		++current_iteration;

		this->selection();
		
		if(this->improved)
		{
			biggest_delay_between_improvements = std::max(biggest_delay_between_improvements, current_iteration-last_iteration_with_improvement);
			last_iteration_with_improvement = current_iteration;
			this->improved = false;
			iterations_without_improvement = 0;
		}

		else
			++iterations_without_improvement;

		cont_mutation = (++cont_mutation == MUTATION_PERIOD) ? 0 : cont_mutation;

		if(!cont_mutation)
		{
			this->local_optimization(current_iteration, MUTATION_PERIOD);
			this->mutation();
		}
	}

	//std::cout << this->size_problem << " " << last_iteration_with_improvement << " " << biggest_delay_between_improvements <<  std::endl;

	// std::cout << "The best solution found: ";
	// print_solution(this->current_best_individual, size_problem);
	// std::cout << "Cost of best solution found: ";
	// std::cout << fitness_of_current_best_individual << std::endl;
}


int GA_QAP::get_fitness_current_best_solution()
{
	return this->fitness_of_current_best_individual;
}

int * GA_QAP::get_best_solution()
{
	return this->current_best_individual.perm;
}

