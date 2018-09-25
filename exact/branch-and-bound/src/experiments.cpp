#include "branch_and_bound.h"
#include "experiments.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std::chrono;

int n;
int** d_mat;
int** f_mat;

void read_instance( std::string instance_path )
{
	std::ifstream instance_stream (PATH_INSTANCE_PREFIX +  instance_path + FILE_INSTANCE_SUFFIX);

	if (instance_stream.is_open())
	{
		instance_stream >> n;

		d_mat = new int*[n];
		f_mat = new int*[n];

		for(int i = 0; i < n; ++i)
		{
			d_mat[i] = new int[n];
			f_mat[i] = new int[n];
		}

		for(int i = 0; i < n; ++i)
			for(int j = 0; j < n; ++j)
				instance_stream >> d_mat[i][j];

		for(int i = 0; i < n; ++i)
			for(int j = 0; j < n; ++j)
				instance_stream >> f_mat[i][j];
		
		instance_stream.close();
	}

	else 
	{
		std::cout << "Unable to open instance " << instance_path << std::endl;
		exit(0);
	}
}

void runQAP ( std::string instance_path )
{
	read_instance(instance_path);

	std::cout << "Running QAP\n";

	QAPBranch qap_branch = QAPBranch( n,  d_mat,  f_mat);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	qap_branch.solve();
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	int duration_in_milisseconds = duration_cast<milliseconds>(t2 - t1).count();

	std::cout << "\nelapsed time: " << duration_in_milisseconds << " ms\n";

	int* sol = qap_branch.get_current_best_solution();
	std::cout << "number of visited nodes: " << qap_branch.get_number_of_nodes() <<  "\nsolution: ";
	for(int i = 0; i < n; ++i) std::cout << sol[i] + 1 << " ";
	std::cout << "\n\n";

	int* non_visited_nodes = qap_branch.get_non_visited_nodes();
	std::cout << "discarded subsolutions: (size, quantity):\n";
	
	for(int i = 0; i < n; ++i)
	{
		std::cout << "(" << i << ", " << non_visited_nodes[i] << ")\n";
	}

	for(int i = 0; i < n; ++i)
	{
		delete[] d_mat[i];
		delete[] f_mat[i];
	}

	delete[] d_mat;
	delete[] f_mat;
}