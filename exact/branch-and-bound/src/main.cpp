#include "branch_and_bound.h"
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace std::chrono;

void runQAP ( std::string instance )
{

	int n;
	int** d_mat;
	int** f_mat;

	std::ifstream instanceStream (instance);
	std::string aux;

	if (instanceStream.is_open())
	{
		std::getline(instanceStream, aux);
		n = std::stoi(aux);

		d_mat = new int*[n];
		f_mat = new int*[n];


		for(int i = 0; i < n; ++i)
		{
			d_mat[i] = new int[n];
			f_mat[i] = new int[n];
		}

		// jump
		std::getline(instanceStream, aux);

		for(int i = 0; i < n; ++i)
		{
			std::getline(instanceStream, aux);
			std::vector<std::string> numbers;

			std::istringstream iss(aux);
				std::copy(std::istream_iterator<std::string>(iss),
     				  std::istream_iterator<std::string>(),
     				  std::back_inserter(numbers));
			
			for(int j = 0; j < n; ++j)
				d_mat[i][j] = stoi(numbers[j]);
		}

		// jump
		std::getline(instanceStream, aux);

		for(int i = 0; i < n; ++i)
		{
			std::getline(instanceStream, aux);
			std::vector<std::string> numbers;

			std::istringstream iss(aux);
				std::copy(std::istream_iterator<std::string>(iss),
     				  std::istream_iterator<std::string>(),
     				  std::back_inserter(numbers));
			
			for(int j = 0; j < n; ++j)
				f_mat[i][j] = stoi(numbers[j]);

		}

		
		instanceStream.close();
	}
	else 
	{
		printf("%s\n", "Unable to open file");
		exit(0);
	}	

	printf("%s\n", "Running QAP..." );
	std::srand(std::time(0));
	QAPBranch qapBranch = QAPBranch( n,  d_mat,  f_mat);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	qapBranch.solve();
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	int duration_in_milisseconds = duration_cast<milliseconds>(t2 - t1).count();

	std::cout << "\nelapsed time: " << duration_in_milisseconds << " ms\n";

	int* sol = qapBranch.get_current_best_solution();
	std::cout << "number of visited nodes: " << qapBranch.get_number_of_nodes() <<  "\nsolution: ";
	for(int i = 0; i < n; ++i) std::cout << sol[i] + 1 << " ";
	std::cout << "\n\n";

	int* non_visited_nodes = qapBranch.get_non_visited_nodes();
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

int main(int argc, char *argv[])
{
	
	std::string file;
	
	if (argc == 2)
		file = argv[1];
	
	else
	{
		printf("%s\n", "ERROR - invalid argument\n");
		return 0;
	} 
	
	std::ifstream instanceFile (file);
	std::string instance;
	if (instanceFile.is_open())
	{
		while (!instanceFile.eof())
		{
			std::getline(instanceFile, instance);
			printf("%s\n", "@ instance uploaded successfully" );
			std::cout << instance << std::endl;
			runQAP(instance);
		}
		instanceFile.close();
	}
	else 
	{
		printf("%s\n", "Unable to open file");
		return 0;
	}

	return 0;
}