#ifndef BRANCH_AND_BOUND_H
#define BRANCH_AND_BOUND_H

#include <vector>

struct Edge
{
	int v1;
	int v2;
	int weight;

	bool operator<(const Edge& rhs) const
    {
    	if(weight < rhs.weight) return true;
    	if(rhs.v1 < rhs.v2) return true;
    	if(rhs.v1 == rhs.v2) return rhs.v1 < rhs.v2;
    	return false;
    }
};

bool greater_edge(Edge e1, Edge e2);

/**
 * @brief      Class that solves the Quadratic Assignment Problem using a
 *             branch-and-bound technique.
 */
class QAPBranch
{

private:
	std::vector<Edge> f_edge_vector;
	std::vector<Edge> d_edge_vector;

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

	int lower_bound_for_partial_solution(int partial_solution_size, bool* already_in_solution, int current_partial_cost);

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
	void recursive_search_tree_exploring(int current_cost, int current_solution_size, 
										 int* current_solution, bool* already_in_solution);

	void matrices_to_ordered_edge_vectors();
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