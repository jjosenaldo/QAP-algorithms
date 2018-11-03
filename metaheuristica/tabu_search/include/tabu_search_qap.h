#ifndef _TABU_SEARCH_QAP_
#define _TABU_SEARCH_QAP_

#include <algorithm>
#include <cstdlib>
#include <ctime> 
#include <vector>
#include <iostream>
#include <string>
#include "qap.h"

class Operation
{

private: 
	int miN;
	int maX;

public:
	Operation();

	Operation(int i, int j);

	int get_min();

	int get_max();

	void set(int i, int j);
};

class TsQAP
{

private:

	int n;

	std::string instance_name;

	/* It consists of the current best n-dimensional permutation vector in 
	 * which the i-th element corresponds to the facility assigned to the
	 * i-th location */
	int* current_best_solution;

	int fitness_current_best_solution;
	
	/**
	 * The candidate for a best solution
	 */
	int* current_solution;

	int fitness_current_solution;

	/**
	 * The tabu list
	 */
	std::vector<std::vector<int>> last_seen_matrix;

	/**
	 * Max size of tabu list
	 */
	int max_size_tabu_list;

	/**
	 * The instance QAP
	 */
	QAP *problem;

	std::vector<std::vector<int>> delta_matrix;

	int current_iteration;

	Operation init_delta_matrix();

	int delta_value_linear(int i, int j);

	int delta_value_constant(int i, int j, int p, int q);

	void update_delta_matrix(Operation operation);

	void apply_operation(Operation operation, int* array);

	void init_matrix(std::vector<std::vector<int>>& matrix);

	Operation get_best_neighbor();

	/**
	 * @brief      Generates a initial solution (i.e., a permutation vector),
	 *             that will define the first upper bound used in the main
	 *             algorithm
	 *
	 * @param      size_solution    the size problem          
	 */
	void generate_initial_solution();

	bool is_tabu(int i, int j);

	bool is_tabu(Operation operation);

	bool satisfies_aspiration_criteria1(int cost);

	void set_fitness_current_solution(int new_cost);

	void set_fitness_current_best_solution(int new_cost);

	void increment_fitness_current_best_solution(int delta);

	void increment_fitness_current_solution(int delta);

	void init_last_seen_matrix();

	void set_last_seen(int i, int j);

	void set_last_seen(Operation index);

	int get_delta(Operation index);

	void set_delta(Operation index, int new_value);

	/**
	 * @brief      Sets the current best solution.
	 *
	 * @param      new_best_solution  The new best solution
	 */
	void set_current_best_solution(int* new_best_solution);

	/**
	 * @brief      Sets the best candidate.
	 *
	 * @param      new_current_solution  The new best candidate
	 */
	void set_current_solution(int* new_current_solution);
public:

	/**
	 * @brief      Constructs the object.
	 *
	 * @param[in]  instance  The instance qap
	 * @param[in]  max_size  The maximum size of tabu list
	 */
	TsQAP(QAP *instance, int max_size, std::string instance_name);

	/**
	 * @brief      Destroys the object.
	 */
	~TsQAP();



	/**
	 * @brief      Gets the neighbors.
	 *
	 * @param      solution  The solution
	 *
	 * @return     The neighbors of the informed solution.
	 */
	std::vector<int*> get_unforbidden_neighbors(int* solution);

	/**
	 * @brief      Calculates the fitness.
	 *
	 * @param      solution  The solution
	 *
	 * @return     The fitness of solution.
	 */
	int calculate_fitness (int* solution);

	

	/**
	 * @brief      run tabu search
	 *
	 * @param[in]  qap  The instance QAP 
	 */
	void run();


	int get_fitness_current_best_solution();
	
	void print_tabu_list();

	void add_swap_to_tabu_list(int i, int j);

	void print_delta_matrix();

	void print_naive_delta_matrix();

	std::string get_instance_name ();
};

#endif