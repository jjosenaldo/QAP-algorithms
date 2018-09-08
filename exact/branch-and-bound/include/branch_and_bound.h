#ifndef BRANCH_AND_BOUND_H
#define BRANCH_AND_BOUND_H

/**
 * @brief      Class that solves the Quadratic Assignment Problem using a
 *             branch-and-bound technique.
 */
class QAPBranch
{

private:

	/* Number of facilities/locations */
	int n;

	/* Matrix of flows between facilities */
	int** f_mat;

	/* Matrix of distances between locations */
	int** d_mat;

	/* Best assignment that the algorithm have found so far. It consists of a
	 * n-dimensional permutation vector in which the i-th element corresponds
	 * to the facility assigned to the i-th location */
	int* current_best_solution;

	/* Cost associated to the best assignment encountered so far  */
	int current_best_cost;

	/**
	 * @brief      Generates a initial solution (i.e., a permutation vector),
	 *             that will define the first upper bound used in the main
	 *             algorithm
	 */
	void generate_initial_solution();


	/**
	 * @brief      Gets a lower bound for a partial solution so that the
	 *             correspondig branch in the tree may possibly be pruned off.
	 *
	 * @param[in]  partial_solution_size  The size of the partial solution
	 *                                    (i.e., the number of facilities
	 *                                    already assigned)
	 * @param      partial_solution       The partial solution itself
	 *
	 * @return     A lower bound for the cost that the given partial solution may have
	 */
	int lower_bound_for_partial_solution(int partial_solution_size, int* partial_solution);

	/**
	 * @brief      Explores a given node of the search tree, corresponding to a
	 *             partial solution for the prpblem.
	 *
	 * @param[in]  current_cost           Cost of the solution
	 * @param[in]  current_solution_size  Number of facilities already assigned
	 * @param      current_solution       n-dimensional permutation vector
	 *                                    corresponding to the partial solution
	 * @param      already_in_solution    n-dimensional vector in which the i-th
	 *                                    position is true iff the i-th
	 *                                    facilitiy was assigned to a location
	 *                                    already
	 */
	void recursive_search_tree_exploring(int current_cost, int current_solution_size, int* current_solution, bool* already_in_solution);

public:

	/**
	 * @brief      Constructs the object.
	 *
	 * @param[in]  n      Number of facilities/locations
	 * @param      d_mat  Distance matrix
	 * @param      f_mat  Flux matrix
	 */
	QAPBranch(int n, int** d_mat, int** f_mat);

	/**
	 * @brief      Destroys the object.
	 */
	~QAPBranch();

	/**
	 * @brief      Finds the optimal permutation vector for the problem.
	 */
	void solve();

	/**
	 * @brief      Gets the best solution found so far.
	 *
	 * @return    The best solution so far.
	 */
	int* get_current_best_solution();

	/**
	 * @brief      Gets the cost corresponding to the best solution so far.
	 *
	 * @return     The cost of the best solution so far.
	 */
	int get_current_best_cost();


};

#endif