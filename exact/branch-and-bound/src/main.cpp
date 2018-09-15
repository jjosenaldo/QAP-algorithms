#include "branch_and_bound.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

int main()
{
	std::srand(std::time(0));
	int n;
	std::cin >> n;
	int** d_mat = new int*[n];
	int** f_mat = new int*[n];

	for(int i = 0; i < n; ++i)
	{
		d_mat[i] = new int[n];
		f_mat[i] = new int[n];
	}

	for(int i = 0; i < n; ++i)
	{
		for(int j = 0; j < n; ++j)
			std::cin >> d_mat[i][j];
	}

	for(int i = 0; i < n; ++i)
	{
		for(int j = 0; j < n; ++j)
			std::cin >> f_mat[i][j];
	}

	QAPBranch qapBranch = QAPBranch( n,  d_mat,  f_mat);
	qapBranch.solve();

	int* sol = qapBranch.get_current_best_solution();
	std::cout << qapBranch.get_number_of_nodes() <<  "\n";
	for(int i = 0; i < n; ++i) std::cout << sol[i] << " ";
	std::cout << "\n";
	return 0;
}