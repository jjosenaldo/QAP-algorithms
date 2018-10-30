#include "calcEst.h"

double calcVariancia ( std::vector<int> valores, double media )
{
	double var = 0;
	for (unsigned int i=0; i < valores.size(); i++)
		var += pow((valores[i]-media),2);
	return var/(valores.size()-1);
}

double calcDistanciamento ( int valor, int referencia )
{
	return ( (valor*100.00)/referencia )-100.00;
}

void calcEstatisticas ( Dados dados )
{
	std::sort(dados.custos.begin(), dados.custos.end());
	int size_cost = dados.custos.size();

	int min_cost = dados.custos[0];
	int max_cost = dados.custos[size_cost-1];
	int mediana = 0;
	int solucoes_otimas_encontrada = 0;

	for (unsigned int i=0; i < dados.custos.size(); i++)
		if (dados.custos[i] == dados.optimal_cost) 
			solucoes_otimas_encontrada++;

	double percentual_optimal_solutions_found = (solucoes_otimas_encontrada*100.00)/dados.custos.size(); 

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
	out << "REPORT\n--------------------------------------------------\n";
	out << "Instance size: " << dados.tamanho << std::endl;
	out << "Average elapsed time: " << media_tempo << " ms" << std::endl;
	out << "Best solution: " << dados.optimal_cost << std::endl;
	out << "Percentual of optimal solutions found: " << percentual_optimal_solutions_found << "%" << std::endl;
	out << "Min found cost: " << calcDistanciamento(min_cost, dados.optimal_cost) << "%" << std::endl;
	out << "Max found cost: " << calcDistanciamento(max_cost, dados.optimal_cost) << "%" << std::endl;
	out << "Average found cost: " << calcDistanciamento(media_custo, dados.optimal_cost) << "%" << std::endl; 
	out << "Median: " << calcDistanciamento(mediana, dados.optimal_cost) << "%" << std::endl;
	out << "Standard deviation of cost: " << dp_custo << std::endl;
	out << "--------------------------------------------------\n";
	out.close();

}