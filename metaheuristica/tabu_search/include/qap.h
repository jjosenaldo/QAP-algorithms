#ifndef _QAP_H_
#define _QAP_H_

class QAP
{

private:
	/* Number of facilities/locations */
	int n;

	/* Matrix of flows between facilities */
	int** f_mat;

	/* Matrix of distances between locations */
	int** d_mat;

	/* It consists of the best n-dimensional permutation vector in 
	 * which the i-th element corresponds to the facility 
	 * assigned to the i-th location */
	int* best_solution;

	/* The best cost for the instance  */
	int best_cost;

	/**
	 * @brief      Generates a initial solution (i.e., a permutation vector),
	 *             that will define the first upper bound used in the main
	 *             algorithm
	 */
	void generate_initial_solution();

public:

	/**
	 * @brief      Constructs the object.
	 *
	 * @param[in]  n      Number of facilities/locations
	 * @param      d_mat  Distance matrix
	 * @param      f_mat  Flow matrix
	 */
	QAP(int n, int** d_mat, int** f_mat);

	/**
	 * @brief      Constructs the object.
	 *
	 * @param[in]  n          Number of facilities/locations
	 * @param      d_mat      The distance matrix
	 * @param      f_mat      The flow matrix
	 * @param[in]  best_cost  The best cost
	 */
	QAP(int n, int** d_mat, int** f_mat, int best_cost);

	/**
	 * @brief      Constructs the object.
	 *
	 * @param[in]  n              Number of facilities/locations
	 * @param      d_mat          The distance matrix
	 * @param      f_mat          The flow matrix
	 * @param      best_solution  The best solution
	 * @param[in]  best_cost      The best cost
	 */
	QAP(int n, int** d_mat, int** f_mat, int* best_solution, int best_cost);

	/**
	 * @brief      Destroys the object.
	 */
	~QAP();

	/**
	 * @brief      Gets the number of facilities.
	 *
	 * @return     The number of facilities.
	 */
	int get_number_of_facilities();

	/**
	 * @brief      Gets the matrix of flows.
	 *
	 * @return     The matrix of flows.
	 */
	int** get_matrix_of_flows();

	/**
	 * @brief      Gets the matrix of distances.
	 *
	 * @return     The matrix of distances.
	 */
	int** get_matrix_of_distances();

	/**
	 * @brief      Gets the best solution.
	 *
	 * @return     The best solution.
	 */
	int* get_best_solution();

	/**
	 * @brief      Sets the best solution.
	 *
	 * @param      new_best_solution  The new best solution
	 */
	void set_best_solution(int* new_best_solution);

	/**
	 * @brief      Gets the best cost.
	 *
	 * @return     The best cost.
	 */
	int get_best_cost();

	/**
	 * @brief      Sets the best cost.
	 *
	 * @param[in]  new_best_cost  The new best cost
	 */
	void set_best_cost(int new_best_cost);

};

#endif