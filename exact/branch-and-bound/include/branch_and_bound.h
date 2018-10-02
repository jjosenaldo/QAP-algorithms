#ifndef BRANCH_AND_BOUND_H
#define BRANCH_AND_BOUND_H

#include <utility>
#include <vector>


/**
 * @brief      A simple weighted edge.
 */
struct Edge
{
	int v1;
	int v2;
	int weight;

	bool operator<(const Edge& rhs) const
    {
    	return weight < rhs.weight;
    }
};


/**
 * @brief      Compares two edges and return the "greater" of them in some
 *             sense. The function is used to sort a Edge vector in
 *             decreasing order.
 *
 * @param[in]  e1    first edge
 * @param[in]  e2    second edge
 *
 * @return     the greater edge
 */
bool greater_edge(Edge e1, Edge e2);

/**
 * @brief      Class that solves the Quadratic Assignment Problem using a
 *             branch-and-bound technique.
 */
class QAPBranch
{

private:

	long long number_total_of_nodes;

	int number_of_nodes;
	/* Facility matrix as a vector of Edge */
	std::vector<Edge> f_edge_vector;

	/* Location matrix as a vector of Edge */
	std::vector<Edge> d_edge_vector;

	/* Number of facilities/locations */
	int n;

	/* Matrix of flows between facilities */
	int** f_mat;

	/* Matrix of distances between locations */
	int** d_mat;

	int* nonvisited_solutions;

	/* Best assignment that the algorithm have found so far. It consists of a
	 * n-dimensional permutation vector in which the i-th element corresponds
	 * to the facility assigned to the i-th location */
	int* current_best_solution;

	/* Cost associated to the best assignment encountered so far  */
	int current_best_cost;

	int total_non_visited_nodes;

	/**
	 * @brief      Generates a initial solution (i.e., a permutation vector),
	 *             that will define the first upper bound used in the main
	 *             algorithm
	 */
	void generate_initial_solution();


	/**
	 * @brief      Defines a lower bound from a partially built solution. The
	 *             lower bound consists of a value such that when the solution
	 *             is fully built, if its cost exceeds that value, it should be
	 *             discarded since it won't yield an optimal solution.
	 *
	 * @param[in]  partial_solution_size  The number of facilities already allocated in the current solution
	 * @param      already_in_solution    Bool array to check if a given facility is in the solution
	 * @param[in]  current_partial_cost   The cost of the solution so far
	 *
	 * @return     the lower bound of the current partial solution
	 */
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

	void las_vegas_recursive_search_tree_exploring(int current_cost, int current_solution_size, 
										 int* current_solution, bool* already_in_solution);

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

	int get_number_of_nodes();

	int* get_non_visited_nodes();

	int get_total_non_visited_nodes();

	void calculate_non_visited_nodes();

	void calculate_total_nodes();

	double percential_non_visited_node ();

};

#endif