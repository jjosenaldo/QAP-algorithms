#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include "experiments.h"
#include "latex.h"

int main (int argc, char *argv[])
{
	std::srand(std::time(0));
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
			std::cout << "@ instance uploaded successfully: " << instance << std::endl;
			run_genetic_algorithm(instance);
		}

		instanceFile.close();
		criar_tabelas_latex ();
	}

	else 
	{
		std::cout << "Unable to open file " << file << std::endl;
		return 0;
	}

	return 0;
}