#ifndef _CALCEST_H_
#define _CALCEST_H_ 

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iomanip>      // std::setprecision


struct Dados {
	std::vector<int> custos;
	std::vector<int> tempos;
	std::string instance;
	int tamanho;
	int optimal_cost;
};

double calcVariancia ( std::vector<double> valores, double media);

double calcDistanciamento ( int valor, int referencia );

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