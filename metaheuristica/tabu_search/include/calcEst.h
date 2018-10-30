#ifndef _CALCEST_H_
#define _CALCEST_H_ 

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

struct Dados {
	std::vector<int> custos;
	std::vector<int> tempos;
	std::string instance;
	int tamanho;
	int optimal_cost;
};

double calcVariancia ( std::vector<double> valores, double media);

template <typename T>
T calcMedia ( std::vector<T> valores )
{
	T media = (T) 0;
	for (unsigned int i=0; i < valores.size(); i++)
		media += valores[i];
	return media/valores.size();
}

void calcEstatisticas ( Dados dados );

#endif