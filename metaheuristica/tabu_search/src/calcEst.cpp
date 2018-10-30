#include "calcEst.h"

double calcVariancia ( std::vector<int> valores, double media )
{
	double var = 0;
	for (unsigned int i=0; i < valores.size(); i++)
		var += pow((valores[i]-media),2);
	return var/(valores.size()-1);
}

void calcEstatisticas ( Dados dados )
{
	std::sort(dados.custos.begin(), dados.custos.end());
	int size_cost = dados.custos.size();

	int min_cost = dados.custos[0];
	int max_cost = dados.custos[size_cost-1];
	int mediana = 0;

	if ( size_cost%2 == 0)
	{
		mediana = dados.custos[size_cost/2 - 1] + dados.custos[size_cost/2];
		mediana /= 2;
	} else mediana = dados.custos[size_cost/2];

	double media_custo = calcMedia<int>(dados.custos);
	double media_tempo = calcMedia<int>(dados.tempos);
	double dp_custo = sqrt(calcVariancia(dados.custos, media_custo));


	std::ofstream out("archives/resultExecution.txt", std::ofstream::app);
	if (!out.is_open()) std::cout << "\nNão abriu o arquivo!\n";

	out << dados.instance << "\n";
	out << "REPORT\n--------------------------------------------------";
	out << std::endl << "Average elapsed time: " << media_tempo << " ms" << std::endl;
	out << "min found cost: " << min_cost << std::endl;
	out << "max found cost: " << max_cost << std::endl;
	out << "Average found cost: " << media_custo << std::endl; 
	out << "Median: " << mediana << std::endl;
	out << "Standard deviation of cost: " << dp_custo << std::endl;
	out << "--------------------------------------------------\n";
	out.close();

}