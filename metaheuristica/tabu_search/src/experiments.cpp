#include "experiments.h"

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

void run_tabu_search( std::string instance_name )
{
	read_instance(instance_name);
	
	QAP qap = QAP( n, d_mat, f_mat);
	TsQAP ts = TsQAP(&qap, n);

	std::cout << "Running Tabu Search in QAP...\n";
	for(int i=0; i<30; i++){
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		ts.run();
		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		int duration_in_milisseconds = duration_cast<milliseconds>(t2 - t1).count();

		std::cout << "Execution finished!" << "\n\n";

		std::ofstream out("archives/resultExecution.txt", std::ofstream::app);
		if (!out.is_open()) std::cout << "\nNão abriu o arquivo!\n";

		out << instance_name << "\n";
		out << "REPORT\n--------------------------------------------------";
		out << std::endl << "Elapsed time: " << duration_in_milisseconds << " ms" << std::endl;

		int* sol = ts.get_current_best_solution();
		out << "Best solution found: ";
		for(int i = 0; i < n; ++i)
			out << sol[i] + 1 << " ";

		std::ifstream opt_solution_stream(PATH_INSTANCE_PREFIX + FILE_OPT_PREFIX +  instance_name + FILE_OPT_SUFFIX);
		int opt_cost;
		opt_solution_stream >> opt_cost;

		int cost_solution_found = qap.calculate_cost_of_solution(sol);
		out << "\nCost of best solution found: " << cost_solution_found;
		out << "\nReal best cost: " << opt_cost;
		out << "\nDiference of best cost: " << opt_cost - cost_solution_found;

		out << "\n-------------------------------------------------" << std::endl;
		out.close();
	}

	for(int i = 0; i < n; ++i)
	{
		delete[] d_mat[i];
		delete[] f_mat[i];
	}

	delete[] d_mat;
	delete[] f_mat;
	delete[] opt_solution;
}