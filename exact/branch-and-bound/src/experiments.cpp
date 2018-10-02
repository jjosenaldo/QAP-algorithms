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
int* opt_solution;

void read_instance( std::string instance_name )
{
	std::ifstream instance_stream (PATH_INSTANCE_PREFIX +  instance_name + FILE_INSTANCE_SUFFIX);

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
		std::cout << "Unable to open instance " << instance_name << std::endl;
		exit(0);
	}
}

void runQAP ( std::string instance_name )
{
	read_instance(instance_name);

	QAPBranch qap_branch = QAPBranch( n,  d_mat,  f_mat);

	std::cout << "Running QAP...\n";

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	qap_branch.solve();
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	int duration_in_milisseconds = duration_cast<milliseconds>(t2 - t1).count();

	std::cout << "Execution finished!" << "\n\n";

	std::ofstream out("archives/resultExecution.txt", std::ofstream::app);
	if (!out.is_open()) std::cout << "\nNão abriu o arquivo!\n";
	
	out << instance_name << "\n";
	out << "REPORT\n--------------------------------------------------";
	out << std::endl << "Elapsed time: " << duration_in_milisseconds << " ms" << std::endl;

	int* sol = qap_branch.get_current_best_solution();
	out << "Number of visited nodes: " << qap_branch.get_number_of_nodes() << std::endl << "Solution found: ";

	for(int i = 0; i < n; ++i)
		out << sol[i] + 1 << " ";

	out << std::endl << "Cost found: " << qap_branch.get_current_best_cost() << std::endl;

	std::ifstream opt_solution_stream(PATH_INSTANCE_PREFIX + FILE_OPT_PREFIX +  instance_name + FILE_OPT_SUFFIX);

	int opt_cost;
	opt_solution = new int[n];
	opt_solution_stream >> opt_cost;

	for(int i = 0; i < n; ++i)
		opt_solution_stream >> opt_solution[i];

	if(qap_branch.get_current_best_cost() == opt_cost)
	{
		std::cout << "The cost found is optimal! =)\n";
		out << "The cost found is optimal! =)\n";
	}

	else
	{
		std::cout << "The cost found is not optimal. =(" << std::endl;
		std::cout << "The optimal cost is actually: " << opt_cost /** << ", and the optimal solution is: "*/ << std::endl; 
		
		out << "The cost found is not optimal. =(" << std::endl;
		out << "The optimal cost is actually: " << opt_cost  << std::endl; 
	}

	out << "Percential of non-visited nodes: " << qap_branch.percential_non_visited_node() << " %\n";

	out << "-------------------------------------------------" << std::endl;
	out.close();

	for(int i = 0; i < n; ++i)
	{
		delete[] d_mat[i];
		delete[] f_mat[i];
	}

	delete[] d_mat;
	delete[] f_mat;
	delete[] opt_solution;
}