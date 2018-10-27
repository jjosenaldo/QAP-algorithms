#include <bits/stdc++.h>
#include "qap.h"
#include "tabu_search_qap.h"
using namespace std;

const std::string PATH_INSTANCE_PREFIX = "../../instances/";
const std::string FILE_INSTANCE_SUFFIX = ".dat";
const std::string FILE_OPT_PREFIX =  "[opt]";
const std::string FILE_OPT_SUFFIX =  ".dat";

int main()
{
	int n;
	int** d_mat;
	int** f_mat;
	std::string instance_name = "beth";

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

	QAP qap = QAP( n, d_mat, f_mat);
	TsQAP ts = TsQAP(&qap, n);
	
	ts.run();

	for(int i = 0; i < n; ++i)
	{
		delete[] d_mat[i];
		delete[] f_mat[i];
	}

	delete[] d_mat;
	delete[] f_mat;

	return 0;
}