#ifndef _TABU_SEARCH_QAP_
#define _TABU_SEARCH_QAP_

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime> 
#include "qap.h"

struct Pair
{
	int i;
	int j;

	bool operator==(const Pair& rhs)
	{
		if (rhs.i == this->i and rhs.j == this->j)
			return true;
		return false;
	}
};

class TsQAP
{

private:

	/* It consists of the current best n-dimensional permutation vector in 
	 * which the i-th element corresponds to the facility assigned to the
	 * i-th location */
	int* current_best_solution;

	int fitness_current_best_solution;
	
	/**
	 * The candidate for a best solution
	 */
	int* best_candidate;

	int fitness_best_candidate;

	/**
	 * The tabu list
	 */
	std::vector<Pair> tabu_list;

	/**
	 * Max size of tabu list
	 */
	int max_size_tabu_list;

	/**
	 * The instance QAP
	 */
	QAP *problem;

	int** delta_matrix;

	void init_delta_matrix();

	void delta_value_linear(int i, int j);

public:

	/**
	 * @brief      Constructs the object.
	 *
	 * @param[in]  instance  The instance qap
	 * @param[in]  max_size  The maximum size of tabu list
	 */
	TsQAP(QAP *instance, int max_size);

	/**
	 * @brief      Destroys the object.
	 */
	~TsQAP();

	/**
	 * @brief      Generates a initial solution (i.e., a permutation vector),
	 *             that will define the first upper bound used in the main
	 *             algorithm
	 *
	 * @param      size_solution    the size problem          
	 */
	void generate_inicial_solution (int size_solution);


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

	/**
	 * @brief      Determines if forbidden.
	 *
	 * @param[in]  operation  The operation
	 *
	 * @return     True if forbidden, False otherwise.
	 */
	bool isForbidden(Pair operation);

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
	int get_fitness_best_candidate();
	
};

#endif