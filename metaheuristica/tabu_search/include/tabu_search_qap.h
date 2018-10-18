#ifndef _TABU_SEARCH_QAP_
#define _TABU_SEARCH_QAP_

#include<vector>

class TsQAP
{

private:

	/* It consists of the current best n-dimensional permutation vector in 
	 * which the i-th element corresponds to the facility assigned to the
	 * i-th location */
	int* current_best_solution;
	
	/**
	 * The candidate for a best solution
	 */
	int* best_candidate;

	/**
	 * The tabu list
	 */
	// std::vector<int*> tabu_list;

	int max_size_tabu_list;

public:

	/**
	 * @brief      Constructs the object.
	 *
	 * @param      current_best_solution  The current best solution
	 */
	TsQAP(int* current_best_solution, int max_size);

	/**
	 * @brief      Destroys the object.
	 */
	~TsQAP();

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

	/**
	 * @brief      Gets the best candidate.
	 *
	 * @return     The best candidate.
	 */
	int* get_best_candidate();

	/**
	 * @brief      Sets the best candidate.
	 *
	 * @param      new_best_candidate  The new best candidate
	 */
	void set_best_candidate(int* new_best_candidate);

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
	 * @brief      Gets the neighbors.
	 *
	 * @param      solution  The solution
	 *
	 * @return     The neighbors of the informed solution.
	 */
	std::vector<int*> get_neighbors(int* solution);

	/**
	 * @brief      Calculates the fitness.
	 *
	 * @param      solution  The solution
	 *
	 * @return     The fitness of solution.
	 */
	int calculate_fitness (int* solution);

};

#endif