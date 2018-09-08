#ifndef BRANCH_AND_BOUND_H
#define BRANCH_AND_BOUND_H

class QAPBranch
{

private:
	int n;
	int** f_mat;
	int** d_mat;

	int* current_best_solution;
	int current_best_cost;

	void generate_initial_solution();

	int lower_bound_for_partial_solution(int partial_solution_size, int* partial_solution);

	void recursive_search_tree_exploring(int current_cost, int current_solution_size, int* current_solution, bool* already_in_solution);

public:
	QAPBranch(int n, int** d_mat, int** f_mat);

	~QAPBranch();

	void solve();

	int* get_current_best_solution();

	int get_current_best_cost();


};

#endif