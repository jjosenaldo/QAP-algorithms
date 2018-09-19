#include "branch_and_bound.h"
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std::chrono;

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

	printf("%s\n", "Running QAP..." );
	QAPBranch qapBranch = QAPBranch( n,  d_mat,  f_mat);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	qapBranch.solve();
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	int duration_in_milisseconds = duration_cast<milliseconds>(t2 - t1).count();

	std::cout << "\nelapsed time: " << duration_in_milisseconds << " ms\n";

	int* sol = qapBranch.get_current_best_solution();
	std::cout << "number of visited nodes: " << qapBranch.get_number_of_nodes() <<  "\nsolution: ";
	for(int i = 0; i < n; ++i) std::cout << sol[i] << " ";
	std::cout << "\n";

	for(int i = 0; i < n; ++i)
	{
		delete[] d_mat[i];
		delete[] f_mat[i];
	}

	delete[] d_mat;
	delete[] f_mat;


	return 0;
}