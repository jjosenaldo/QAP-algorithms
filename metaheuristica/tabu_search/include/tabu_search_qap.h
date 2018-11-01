#ifndef _TABU_SEARCH_QAP_
#define _TABU_SEARCH_QAP_

#include <algorithm>
#include <cstdlib>
#include <ctime> 
#include <vector>
#include <iostream>
#include <string>
#include "qap.h"

bool pair_equals(std::pair<int, int> p1, std::pair<int, int> p2);

// based on Casey's answer on https://stackoverflow.com/questions/20590656/error-for-hash-function-of-pair-of-ints
struct pair_hash {
    std::size_t operator () (const std::pair<int,int> &p) const {
        auto h1 = std::hash<int>{}(p.first);
        auto h2 = std::hash<int>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};

class Operation
{

private: 
	int first;
	int second;

public:
	Operation();

	Operation(int i, int j);

	int get_first();

	int get_second();

	void set_first(int i);

	void set_second(int i);

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

	void hardcode_solution(int* solution);

	void init_last_seen_matrix();

	void set_last_seen(int i, int j);

	void set_last_seen(Operation operation);

	int get_delta(Operation operation);

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

	/**
	 * @brief      Gets the current best solution.
	 *
	 * @return     The current best solution.
	 */
	int* get_current_best_solution();

	/**
	 * @brief      Sets the current best solution.
	 *
	 * @param      new_best_solution  The new best solution
	 */
	void set_current_best_solution(int* new_best_solution);

	void set_current_best_solution(std::pair<int, int> perturbation);

	/**
	 * @brief      Gets the best candidate.
	 *
	 * @return     The best candidate.
	 */
	int* get_current_solution();

	/**
	 * @brief      Sets the best candidate.
	 *
	 * @param      new_current_solution  The new best candidate
	 */
	void set_current_solution(int* new_current_solution);

	/**
	 * @brief      Gets the maximum size tabu list.
	 *
	 * @return     The maximum size tabu list.
	 */
	int get_max_size_tabu_list();

	/**
	 * @brief      Sets the maximum size tabu list.
	 *
	 * @param[in]  new_max_size  The new maximum size
	 */
	void set_max_size_tabu_list(int new_max_size);

	// std::vector<int*> get_tabu_list();

	/**
	 * @brief      Gets the instance qap.
	 *
	 * @return     The instance qap.
	 */
	QAP* get_instance_qap ();


	/**
	 * @brief      Gets the fitness best candidate.
	 *
	 * @return     The fitness best candidate.
	 */
	int get_fitness_current_solution();

	int get_fitness_current_best_solution();
	
	void print_tabu_list();

	void add_swap_to_tabu_list(int i, int j);

	void print_delta_matrix();

	void print_naive_delta_matrix();

	std::string get_instance_name ();

	int get_n();
};

#endif